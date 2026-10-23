#include "StreamClient.h"
#include "HookDefine.h"
#include "SmfFCC.h"
#include "SmfMsg.h"
#include "SmfDef.gbl.h"
#include "smf_debug_ext.h"

using namespace smf;
using namespace smf::msg;
using namespace smf::ssvc;
#define CPU_MAX 2

EXTERNC void  smf_stream_client_register() {
	Object::Register<StreamClient>("mclient-stream-client");
}

StreamClient::StreamClient() {
	_id = fcc64("sclient");
	Global<StreamClient>(eGlobal::sclient, this);
	memset(_streams, 0, sizeof(_streams));
	memset(_servers, 0, sizeof(_servers));
	memset(_notifys, 0, sizeof(_notifys));
}
StreamClient::~StreamClient() {
	Global<StreamClient>(eGlobal::sclient, 0);
}
bool StreamClient::Set(StreamServer* server, eStream es) {
	switch (es) {
	case eStream::null:_servers[0] = _servers[1] = server; break;
	case eStream::audioPlayer:_servers[0] = server; break;
	case eStream::audioRecord:_servers[1] = server; break;
	default:break;
	}
	return true;
}
bool StreamClient::set(uint32_t key, void* val) {
	switch (key) {
	case Hash("job"):if (val)MsgRouteRegister(eStream::job); return true;
	case Hash("device"):if (val)MsgRouteRegister(eStream::device); return true;
	default:return _cfg.set(key, val) || IMsgClient::set(key, val);
	}
}

bool StreamClient::Start(uint32_t stream
	, const void* param, uint32_t size
	, uint32_t timeout, bool is_string
) {
	returnIfErrC(0, !_cfg.CheckStream(stream));
	returnIfErrC(0, (size>(sizeof(_msgStack)-sizeof(TMsg<MsgAdd_t>))));
	stream_t& item = _streams[stream];
	{
		unique_lock<mutex> lck(_mtx);
		auto name = _cfg.GetName64(stream);
		auto msgid = _cfg.GetMsgID(stream);
		auto reentry = _cfg.GetReEntry(stream);
		uint8_t cpuids[2]{ _cfg.GetCpuID(stream, 0) ,  _cfg.GetCpuID(stream, 1) };
		returnIfErrC(0, !name);
		if (!item.cnt) {
			uint8_t idx = 0;
			for (int i = 0; i < 2; i++) {
				if (cpuids[i]) {
					idx++;
				}
			}
			item.cnt = idx;
		}
		//
		uint8_t seqno = 0;
		{
			ssvc::Locker lck(100);
			seqno = _cfg.GetSeqNO(stream);
			auto refs = _cfg.GetRefs(stream);
			returnIfErrC(false, (!reentry) && refs);
			dbgSsvcPXL("[%02x]%s[%02x,%02x]%p/%u,%u,%u,timeout:%u", stream, (const char*)&name, refs, seqno, param, size, is_string, item.cnt, timeout);
			if (refs) {
				_cfg.RefsIncrease(stream);
				//dbgSsvcPXL("[%02x]%s[%02x,%02x]%p/%u,%u", stream, (const char*)&name, refs, seqno, param, size, is_string);
				return true;
			}
			else {
				_cfg.RefsIncrease(stream);
			}
			_cfg.SeqNOIncrease(stream);
		}
		//
		SysFreqRequest();
		for (int i = 0; i < 2; i++) {
			auto cpuid = cpuids[i];
			if (cpuid) {
				returnIfErrC(false, !PowerOn(cpuid));
			}
		}
		//
		auto& msg = *(TMsg<MsgAdd_t>*)_msgStack;
		memset(&msg, 0, sizeof(_msgStack));
		msg.size = sizeof(TMsg<MsgAdd_t>) + size;
		msg.cmd = (uint16_t)Combine((eStream)msgid, eStream::add);
		msg.svcid = (uint8_t)stream;
		msg.flags = (uint8_t)((seqno + 1) & 0x3);
		msg.name = name;
		msg.string = is_string;
		auto msg_params = msg.params;
		//
		if (param && size) {
			memcpy(msg_params, param, size);
		}
		else {
			msg.string = 0;
		}
		//
		item.idx = 0;
		item.result = true;
		for (int i = 0; i < 2; i++) {
			auto cpuid = cpuids[i];
			if (cpuid) {
				msg.cpudst = cpuid;
				returnIfErrC(false, !Send(&msg));
			}
		}
	}
	//
	JobHold();
	//
	if (timeout) {
		auto idx = 0;
		while (idx < timeout) {
			if (item.idx >= item.cnt) {
				return item.result;
			}
			sleep_for(5);
			idx += 5;
		}
		dbgWarnPXL("%03x,%u,%u/%u", stream, timeout, item.idx, item.cnt);
		return false;
	}
	return true;
}

bool StreamClient::Stop(uint32_t stream, bool force) {
	returnIfErrC(false, !_cfg.CheckStream(stream));
	unique_lock<mutex> lck(_mtx);
	auto& item = _streams[stream];
	auto name = _cfg.GetName64(stream);
	auto msgid = _cfg.GetMsgID(stream);
	uint8_t cpuids[2]{ _cfg.GetCpuID(stream, 0), _cfg.GetCpuID(stream,1) };
	returnIfErrC(false, !name);
	uint8_t seqno = 0;
	{
		ssvc::Locker lck(100);
		seqno = _cfg.GetSeqNO(stream);
		auto refs = _cfg.GetRefs(stream);
		dbgSsvcPXL("[%02x]%s[%02x,%02x]%u", stream, (const char*)&name, refs, seqno, force);
		if (force) {
			//dbgWarnPL();
			_cfg.RefsClear(stream);
		}
		else if (!refs) {
			//dbgWarnPL();
			return true;
		}
		else if (refs > 1) {
			_cfg.RefsReduce(stream);
			//dbgWarnPXL("[%02x]%s[%02x,%02x", stream, (const char*)&name, refs, seqno);
			return true;
		}
		else {
			_cfg.RefsClear(stream);
		}
	}
	//
	TMsg<MsgRemove_t> msg;
	memset(&msg, 0, sizeof(msg));
	msg.size = sizeof(msg);
	msg.cmd = (uint16_t)Combine((eStream)msgid, eStream::remove);
	msg.svcid = (uint8_t)stream;
	msg.flags = (uint8_t)seqno;
	msg.name = name;
	//
	item.idx = 0;
	item.result = true;
	//
	for (int i = 0; i < 2; i++) {
		auto cpuid = cpuids[i];
		if (cpuid) {
			msg.cpudst = cpuid;
			returnIfErrC(0, !Send(&msg));
			returnIfErrC(0, !PowerOff(cpuid));
		}
	}
	JobRelease();
	return true;
}

bool StreamClient::Pause(uint32_t stream) {
	dbgSsvcPXL("[%02x]", stream);
	return Set32(stream, 0, "stsup", 3);
}
bool StreamClient::Resume(uint32_t stream) {
	dbgSsvcPXL("[%02x]", stream);
	return Set32(stream, 0, "stsdown", 2);
}

bool StreamClient::Push(uint32_t stream, smf_frame_t* frm) {
	//dbgSsvcPXL("[%02x]", stream);
	returnIfErrC(false, !_cfg.CheckStream(stream));
	returnIfErrC(false, !_servers[0]);
	return _servers[0]->Process(stream, frm);
}

bool StreamClient::Pull(uint32_t stream, smf_frame_t*& frm) {
	//dbgSsvcPXL("[%02x]", stream);
	returnIfErrC(false, !_cfg.CheckStream(stream));
	returnIfErrC(false, !_servers[1]);
	return _servers[1]->Process(stream, frm);
}

bool StreamClient::Set(uint32_t stream, uint32_t cpuid, const char* path, void* data, uint32_t size) {
	return Set(stream, cpuid, 0, path, data, size, false);
}

bool StreamClient::Set(uint32_t stream, uint32_t setid, void* data, uint32_t size) {
	return Set(stream, 0, setid, 0, data, size, false);
}
bool StreamClient::Get(uint32_t stream, uint32_t setid, void* data, uint32_t& size, uint32_t timeout) {
	return Get(stream, 0, setid, 0, 0, 0, data, size, timeout);
}
bool StreamClient::Get(uint32_t stream, uint32_t setid, smf_cb_data cb, void* priv) {
	return Get(stream, 0, setid, 0, cb, priv, 0, 256, 0);
}
bool StreamClient::Set32(uint32_t stream, uint32_t cpuid, const char* path, uint32_t value) {
	return Set(stream, cpuid, 0, path, &value, 4, true);
}
bool StreamClient::Set32(uint32_t stream, uint32_t setid, uint32_t value) {
	return Set(stream, 0,setid,0, &value, 4, true);
}
bool StreamClient::Get32(uint32_t stream, uint32_t setid, uint32_t& value, uint32_t timeout) {
	return Get(stream, 0, setid, 0, 0, 0, &value, 4, timeout);
}
bool StreamClient::SetVolume(uint32_t stream, uint32_t volume) {
	dbgSsvcPXL("%02x:%u", stream, volume);
	return Set32(stream, 0, "volume", volume);
}
bool StreamClient::SetMute(uint32_t stream, bool mute) {
	dbgSsvcPXL("%02x:%u", stream, mute);
	return Set32(stream, 0, "mute", mute);
}
bool StreamClient::SetVolumeReduce(uint32_t stream, bool reduce) {
	dbgSsvcPXL("%02x:%u", stream, reduce);
	return Set32(stream, 0, "volumeReduce", reduce);
}
bool StreamClient::SetMuteForce(uint32_t stream, bool force) {
	dbgSsvcPXL("%02x:%u", stream, force);
	return Set32(stream, 0, "muteForce", force);
}
bool StreamClient::Seek(uint32_t stream, uint32_t timepoint) {
	return Set32(stream, 0, "seekToMs", timepoint);
}
bool StreamClient::Set(uint32_t stream, uint32_t cpuid, uint32_t setid, const char* path
	, void* data, uint32_t size
	, bool is_value
){
	returnIfErrC(false, !_cfg.CheckStream(stream));
	returnIfErrC(false, setid >= eSetidMax);
	returnIfErrC(false, !data);
	returnIfErrC(false, !size);
	//returnIfErrC(false, size >= sizeof(TMsg<MsgSetGet_t>::data));
	returnIfErrC(false, !path && !setid);
	auto name = _cfg.GetName(stream);
	auto svc = _cfg.GetMsgID(stream);
	returnIfErrC(false, !name);
	if (!path) {
		path = _cfg.GetSetPath(setid);
		returnIfErrC(false, !path);
	}
	if (!cpuid) {
		cpuid = _cfg.GetSetCpuID(setid);
		if(!cpuid)
			cpuid = _cfg.GetCpuID(stream,0);
	}
	returnIfErrC(false, !cpuid);
	//
	auto plen = strlen(path) + 1;
	//returnIfErrC(false, (size + plen >= sizeof(_msgStack) - sizeof(TMsg<MsgSetGet_t>)));
	auto dsize = size;
	if (sizeof(TMsg<MsgSet_t>) + plen + dsize > sizeof(_msgStack)) {
		dbgWarnPXL("data overflow:%u+%u+%u>%u", sizeof(TMsg<MsgSet_t>), plen, dsize, sizeof(_msgStack));
		dsize = 0;
	}
	unique_lock<mutex> lck(_mtx);
	auto& msg = *(TMsg<MsgSet_t>*)_msgStack;
	memset(&msg, 0, sizeof(_msgStack));
	msg.size = sizeof(TMsg<MsgSet_t>) + dsize + plen;
	msg.cmd = (uint16_t)Combine((eStream)svc, eStream::set);
	msg.svcid = (uint8_t)stream;
	msg.flags = (uint8_t)setid;
	msg.cpudst = cpuid;
	msg.is_value = is_value;
	msg.data_size = size;
	msg.path = dsize;
	//path
	auto msg_path = msg.data + msg.path;
	memcpy(msg_path, path, plen);
	dbgSsvcPXL("[%02x,%02x]%s", stream, setid, (const char*)(msg_path));

	if (dsize) {
		memcpy(msg.data, data, size);
	}
	else {
		auto buff = IPool::Shared().Alloc(size);
		returnIfErrC(false, !buff);
		memcpy(buff, data, size);
		cache_flush(buff, size);
		msg.data_pointer = (uint64_t)addr_remap_to_dev(buff);
		Invoke([](void* buff) {IPool::Shared().Free(buff); return true; }, buff, 1000);
	}

	returnIfErrC(0, !Send(&msg));
	return true;
}

bool StreamClient::Get(uint32_t stream, uint32_t cpuid, uint32_t setid, const char* path
	, smf_cb_data cb, void* priv
	, void* buff, uint32_t buff_max
	, uint32_t timeout
) {
	returnIfErrC(false, !_cfg.CheckStream(stream));
	returnIfErrC(false, !cb && !buff);
	returnIfErrC(false, cb && buff);
	returnIfErrC(false, setid >= eSetidMax);
	//returnIfErrC(false, size >= sizeof(TMsg<MsgSetGet_t>::data));
	returnIfErrC(false, !path && !setid);
	auto name = _cfg.GetName(stream);
	auto svc = _cfg.GetMsgID(stream);
	returnIfErrC(false, !name);
	if (!path) {
		path = _cfg.GetSetPath(setid);
		returnIfErrC(false, !path);
	}
	if (!cpuid) {
		cpuid = _cfg.GetSetCpuID(setid);
		if (!cpuid)
			cpuid = _cfg.GetCpuID(stream, 0);
	}
	returnIfErrC(false, !cpuid);
	//
	auto plen = strlen(path) + 1;
	semaphore sema{ 0,false };
	if (buff && !timeout)timeout = 1000;
	if (cb) timeout = 0;
	//returnIfErrC(false, (size + plen >= sizeof(_msgStack) - sizeof(TMsg<MsgSetGet_t>)));
	{
		unique_lock<mutex> lck(_mtx);
		auto& msg = *(TMsg<MsgGet_t>*)_msgStack;
		memset(&msg, 0, sizeof(_msgStack));
		msg.size = sizeof(TMsg<MsgGet_t>) + plen;
		msg.cmd = (uint16_t)Combine((eStream)svc, eStream::get);
		msg.svcid = (uint8_t)stream;
		msg.flags = (uint8_t)setid;
		msg.cpudst = cpuid;
		msg.callback = (uint64_t)cb;
		msg.callback_priv = (uint64_t)priv;
		msg.buff = (uint64_t)buff;
		msg.data_size = buff_max;
		//path
		auto msg_path = msg.data;
		memcpy(msg_path, path, plen);
		dbgSsvcPXL("[%02x,%02x]%s", stream, setid, (const char*)(msg_path));
		if (buff && timeout) {
			sema.initialize();
			msg.sema = (uint64_t)&sema;
			msg.timepoint = get_timestamp() + timeout;
		}
		returnIfErrC(0, !Send(&msg));
	}
	if (sema) {
		sema.wait_for(timeout);
	}
	return true;
}

bool StreamClient::Hook(uint32_t pointer, bool start) {
	dbgSsvcPXL("%02x,%u",pointer,start);
	unique_lock<mutex> lck(_mtx);
	smf_msg_t msg;
	memset(&msg, 0, sizeof(msg));
	msg.size = sizeof(msg);
	msg.cmd = (uint16_t)eHook::hookServer;
	msg.svcid = pointer;
	msg.flags = start ? 1 : 0;
	return Send(&msg);
}

bool StreamClient::HookCallback(smf_hook_cb callback, void* priv) {
	dbgSsvcPL();
	Tuple2<smf_hook_cb, void*> tuple{ callback, priv };
	return svc()->Set("hooksvc/hook", &tuple);
}

bool StreamClient::MessageCallback(smf_stream_message_callback_e id, smf_stream_message_cb callback, void* priv, bool invoke) {
	dbgSsvcPXL("%u,%u", id, invoke);
	unique_lock<mutex> lck(_mtx);
	_notifys[id].func = callback;
	_notifys[id].priv = priv;
	_flagsExt.Set(eFlagsExt8::NotifyInvoke, invoke);
	return true;
}

bool StreamClient::Send(SmfMsg_t* msg) const {
	msg->seqno = svc()->SeqNO();
	return svc()->Process(msg) == Result::True;
}

StreamConfig& StreamClient::GetConfig()const {
	return (StreamConfig&)_cfg;
}

bool StreamClient::Start(uint32_t stream, const char* param, uint32_t timeout) {
	//dbgTestPXL("[%02xx]%s,%u", stream, param, timeout);
	auto len = param ? (strlen(param) + 1) : 0;
	return Start(stream, param, len, timeout, true);
}

bool StreamClient::Start(const char* script) {
	uint32_t params[]{ 0,0,0 };
	Layout layout{ script,false,true,false };
	returnIfErrC(false, !layout.Parse([](void* priv, const char* keys, void* vals, char rst) {
		uint32_t* params = (uint32_t*)priv;
		switch (Hash(keys)) {
		case Hash("stream"):params[0] = (uint32_t)vals; break;
		case Hash("timeout"):params[1] = (uint32_t)vals; break;
		case Hash("params"):params[2] = (uint32_t)vals; break;
		default:dbgWarnPSL(keys); break;
		}
		},params));
	return Start(params[0], (const char*)params[2], params[1]);
}

uint32_t StreamClient::GetFree(uint32_t begin, uint32_t end)const {
	unique_lock<mutex> lck(_mtx);
	for (auto i = begin; i <= end; i++) {
		if (!_cfg.GetRefs(i)) {
			return i;
		}
	}
	return 0;
}

uint8_t StreamClient::GetSeqNO(uint32_t stream)const {
	unique_lock<mutex> lck(_mtx);
	return _cfg.GetSeqNO(stream);
}

bool StreamClient::Check(uint32_t stream)const {
	return _cfg.CheckStream(stream);
}
