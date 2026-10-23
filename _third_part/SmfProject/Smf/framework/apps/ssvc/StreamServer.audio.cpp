#include "StreamServer.audio.h"
#include "StreamClient.h"
#include "SmfHash.h"
#include "SmfFCC.h"
#include "SmfDef.gbl.h"
#include "BackGround.h"
#include "smf_debug_ext.h"

using namespace smf;
using namespace smf::msg;
using namespace smf::ssvc;

AudioServer::AudioServer(uint64_t id, eStream msg, uint64_t name)
	:StreamServer(id, msg)
	,_name(name)
{
	_rate = 48000;
	_ch = 2;
	_bits = 16;
	_fsize = 1920; // 2048;// 1920;//10ms
}

AudioServer::~AudioServer() {
	Status(EStatus::null);
}

bool AudioServer::createScript() {
	dbgExtPXL(1,"%s,%u/%u/%u/%u,%u/%u",&_name, _rate, _ch, _bits, _fsize, _volume, _mute);
	if (Child(_name))return true;
	uint32_t params[]{
		_rate //0
		, _ch //1
		, _bits //2
		, _fsize//3
		, 0//(uint32_t)&_imtx//4
		, 0//(uint32_t)&_omtx//5
		, _volume //6
		, _mute //7
		, (uint32_t)&_name //8
	};
	returnIfErrC(false, !_script);
	//dbgTestPXL(_script);
	auto ele = Layout::Create(_script, this, params);
	returnIfErrC(false, !ele);
	return ((Element*)ele)->Status(StatusNext(), EStatusChange::up);
}

bool AudioServer::set(uint32_t key, void* val) {
	switch (key) {
	case Hash("rate"):_rate=(uint32_t)val; return true;
	case Hash("ch"):_ch = (uint32_t)val; return true;
	case Hash("bits"):_bits = (uint32_t)val; return true;
	case Hash("fsize"):_fsize = (uint32_t)val; return true;
	case Hash("vol"): 
	case Hash("volume"): _volume = (uint32_t)val; if (Child(_name))Child(_name)->Set("volume", _volume); return true;
	case Hash("mute"): _mute = (uint32_t)val; if (Child(_name))Child(_name)->Set("mute", _mute); return true;
	case Hash("script"):_script = (const char*)val; return true;
	case Hash("src"):Deserialize(_pipelines,(const char*)val); return true;
	case Hash("sink"):Deserialize(_pipelines,(const char*)val); return true;
	}
	return Base::set(key, val);
}

bool AudioServer::add(MsgAdd_t& val, streams_t& item, const char* script) {
	auto params = (uint32_t*)&val;
	val.para[0] = _rate;
	val.para[1] = _ch;
	val.para[2] = _bits;
	val.para[3] = _fsize;
	smf::unique_ptr<Element> ele{ (Element*)Layout(script).Create(this, params) };
	returnIfErrC(false, !ele);
	if (ele->Flags().Check(IS_Source)) {
		dbgExtPXL(1, ele->Name());
		//unique_lock<mutex> lck(_imtx);
		returnIfErrC(false, !createScript());
		returnIfErrC(false, !ele->LinkTo((const char*)&_name));
		_srcCnt++;
	}
	else if (ele->Flags().Check(IS_Sink)) {
		dbgExtPXL(1, ele->Name());
		//unique_lock<mutex> lck(_omtx);
		returnIfErrC(false, !createScript());
		returnIfErrC(false, !ele->LinkFrom((const char*)&_name));
		_sinkCnt++;
	}
	else {
		dbgExtPXL(1, ele->Name());
		_otherCnt++;
	}
	returnIfErrC(false, !ele->Status(StatusNext(), EStatusChange::up));
	//
	item.obj = ele.get();
	ele.release();
	return true;
}

bool AudioServer::remove(MsgRemove_t& val, streams_t& item) {
	auto name = val.name;
	auto obj = Child(name);
	returnIfWarnC(false, !obj);
	auto svc = (Service*)obj;
	auto ele = (Element*)obj;
	//status down
	if (obj->Flags().Check(IS_Service)) {
		svc->Status(EStatus::ready, EStatusChange::down);
	}
	//NOTE: avoid msg thread death lock when get omtx, dma may stop thus ele thread will push data in endless loop.
	if (obj->Flags().Check(IS_Source)) {//dbgTestPL();
		//unique_lock<mutex> lck(_imtx); //dbgTestPL();
		ele->DislinkOutput();
		if (_srcCnt) {
			_srcCnt--;
		}
	}
	else if (obj->Flags().Check(IS_Sink)) {//dbgTestPL();
		//unique_lock<mutex> lck(_omtx); //dbgTestPL();
		ele->DislinkInput();
		if (_sinkCnt) {
			_sinkCnt--;
		}
	}
	else {
		dbgWarnPXL(name);
	}
	//check and destroy link element
	if (!_srcCnt && !_sinkCnt && obj->Flags().CheckAny(IS_Source | IS_Sink)) {
		Delete(_name);
	}
	//stream locker
	AtomicLocker alck(item.lock);
	item.obj = 0;
	//status down
	if (obj->Flags().Check(IS_Service)) {
		svc->Status(EStatus::null, EStatusChange::down);
	}
	//destroy pipeline
	delete obj;
	//
	return true;
}