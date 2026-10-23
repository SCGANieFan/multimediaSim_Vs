#include "StreamServer.h"
#include "StreamClient.h"
#include "SmfDef.gbl.h"
#include "BackGround.h"
#include "SmfHash.h"
#include "ShmHook.h"
#include "SmfFCC.h"
#include "SmfICS.h"
#include "smf_debug_ext.h"

using namespace smf;
using namespace smf::msg;
using namespace smf::ssvc;

StreamServer::StreamServer(uint64_t id, eStream msg)
	:_msg(msg)
{
	_id = id;
	memset(_streams, 0, sizeof(_streams));
	Global<StreamServer>(eGlobal::ssvc, this);
}

StreamServer::~StreamServer() {
	Status(EStatus::null);
	Global<StreamServer>(eGlobal::ssvc, 0);
}

bool StreamServer::stop() {
	Base::stop();
	Clear();
	return true;
}
Object::ParamTable* StreamServer::GetParamTable() const {
	return _params_add ? _params_add : (ParamTable*)&_params;
}

bool StreamServer::set(uint32_t key, void* val) {
	switch (key) {
	case Hash("pipelines"):Deserialize(_pipelines, (const char*)val); return true;
	case Hash("params"):Deserialize(_params, (const char*)val); return true;
	}
	return Base::set(key, val);
}

bool StreamServer::processMessage(smf_message_t& msg) {
	dbgSsvcPXL("%s,%s", Name(), (char*)&msg.id);
	switch (msg.id) {
	case fcc64("setdev"): 
		return Device((uint8_t)msg.data[0], (uint8_t)msg.data[1],(MediaDeviceCfg_t*)msg.data[2]);
	case fcc64("notify"):
		return Notify((Object*)msg.laster, (eNotify)msg.sub, (void*)msg.data[0], msg.data[1]);
	case fcc64("eos"): 
		return Notify((Object*)msg.laster, eNotify::eos, &((Object*)msg.creater)->Error(), sizeof(smf_error_t));
	case fcc64("err"):
	case fcc64("error"): 
		return Notify((Object*)msg.laster, eNotify::error, &((Object*)msg.creater)->Error(), sizeof(smf_error_t));
	case fcc64("libsevt"):
		return Notify((Object*)msg.laster, (eNotify)msg.data[0], (void*)msg.data[1], msg.data[2]);
	case fcc64("meta"):
		return Notify((Object*)msg.laster, eNotify::meta, (void*)&msg.data[0], 16);
	case fcc64("media"):
		return Notify((Object*)msg.laster, eNotify::media, (void*)&msg.data[0], 16);
	default:
		dbgSsvcPXL("%s,%s", Name(), (char*)&msg.id);
		break;
	}
	return Base::processMessage(msg);
}
bool StreamServer::init(IMsgService* svc, bool enable) {
	if (enable) {
		auto client = Global<StreamClient>(eGlobal::sclient);
		if (client) {
			client->Set(this,_msg);
		}
	}
	return true;
}
Result StreamServer::Process(smf_msg_t* msg) {
	auto svc = (eStream)(msg->cmd & 0xff0);
	auto cmd = (eStream)(msg->cmd & 0x00f);
	switch (svc) {
	case eStream::audioPlayer:
	case eStream::audioRecord:
	case eStream::streamServer:
		if ((_msg != eStream::null) && (svc != _msg)) {
			return Result::Other;
		}
		switch (cmd) {
		case eStream::add:				return (Result)Response(Add(msg), msg);
		case eStream::remove:			return (Result)Response(Remove(msg), msg);
		case eStream::set:				return (Result)Response(Set(msg), msg);
		case eStream::get:				return (Result)Get(msg);
		default:return Result::Other;
		}
		break;
	case eStream::streamCommand:
		switch ((eStream)msg->cmd) {
		//case eStream::jobResponse:	return Result::True;
		//case eStream::deviceResponse:	return Result::True;
		//case eStream::notifyResponse:	return Result::True;
		default:return Result::Other;
		}
		break;
	default: 
		break;
	}
	return Result::Other;
}

bool StreamServer::Notify(Object* obj, eNotify notify, void* data, uint32_t size) {
	//returnIf(true, _msg == eStream::null);
	returnIfErrC(false, !obj);
	auto stream = getStream(obj);
	//returnIfErrC(false, stream < 0);
	if (stream <= 0) {
		stream = _cfg.GetStream(obj->ID());
		returnIfErrCS(false, stream <= 0, "%s", obj->Name());
	}
	auto& item = _streams[stream];
	auto seqno = item.seqno;
	auto cpuid = item.cpuid;
	TMsg<XMsgNotify<120>> msg;
	memset(&msg, 0, sizeof(msg));
	msg.size = sizeof(msg);
	msg.cmd = (uint16_t)eStream::notify;
	msg.cpudst = cpuid;
	msg.svcid = (uint8_t)stream;
	msg.flags = (uint8_t)notify;
	msg.data_size = (uint16_t)size;
	msg.seqNO = seqno;
	dbgSsvcPXL("[%02x,%02x]%02x", stream, seqno, (uint32_t)notify);
	returnIfErrCS(false, size > (sizeof(msg) - sizeof(TMsg<MsgNotify_t>)), "size=%d", size);
	if (data && size) {
		memcpy(msg.data, data, size);
	}
	return Send(&msg);
}

bool StreamServer::Add(smf_msg_t* msg) {
	auto& val = *MsgGetBody<MsgAdd_t>(msg);
	auto stream = msg->svcid;
	auto seqno = msg->flags;
	auto& item = _streams[stream];
	auto script = _pipelines[stream];
	dbgExtPXL(1,"[%02x,%02x,%u]%s,%u", stream, seqno, item.refs, &val.name, msg->string);
	returnIfErrC(false, stream > eStreamMax);
	returnIfErrC(false, !script);
	dbgExtPXL(1,script);
	if (item.refs) {
		item.refs++;
		dbgExtPXL(1,"%u", item.refs);
		return true;
	}
	//
	item.seqno = seqno;
	item.cpuid = msg->cpusrc;
	item.lock = false;
	//string script params
	SimpleTableC<const char*, 8> params_add;
	if (msg->string) {
		auto str = (const char*)val.params;
		dbgExtPXL(1,str);
		if (Deserialize(params_add, str)) {
			_params_add = &params_add;
		}
	}
	//
	returnIfErrC(false, !Status(EStatus::play, EStatusChange::up));
	returnIfErrC(false, !add(val, item, script));
	//
	_params_add = 0;
	item.refs = 1;
	_add_count++;
	return true;
}

bool StreamServer::Remove(smf_msg_t* msg) {
	auto& val = *MsgGetBody<MsgRemove_t>(msg);
	auto stream = msg->svcid;
	auto name = val.name;
	auto seqno = msg->flags;
	auto& item = _streams[stream];
	dbgExtPXL(1,"[%02x,%02x,%02x,%u]%s", stream, seqno, item.seqno, item.refs, &name);
	if (item.refs > 1) {
		item.refs--;
		dbgSsvcPXL("%u", item.refs);
		return true;
	}
	//
	remove(val, item);
	//
	item.obj = 0;
	item.cpuid = 0;
	item.seqno = 0;
	item.refs = 0;
	//
	if (!--_add_count) {
		Status(EStatus::ready, EStatusChange::down);
	}
	return true;
}

bool StreamServer::Set(smf_msg_t* msg) {
	auto& val = *MsgGetBody<MsgSet_t>(msg);
	auto stream = msg->svcid;
	auto setid = msg->flags;
	auto path = val.data + val.path;
	auto data = val.data;
	dbgSsvcPXL("[%02x,%02x]%s", stream, setid, path);
	auto& item = _streams[stream];
	auto obj = item.obj;
	if (!obj) {
		auto name = _cfg.GetName64(stream);
		returnIfErrC(false, !name);
		obj = Child(name);
		returnIfErrC(false, !obj);
	}
	if (val.data_pointer) {
		data = (char*)addr_remap_to_cpu((const void*)val.data_pointer);
		cache_invalid(data, val.data_size);
		smf_pair_t pair{ data,val.data_size };
		char buff[64];
		sprintf(buff, "%s_pair", path);
		return obj->Set(buff, &pair);
	}
	if (val.is_value) {
		data = *(char**)data;
	}
	return obj->Set(path, data);
}

bool StreamServer::Get(smf_msg_t* msg) {
	auto& val = *MsgGetBody<MsgGet_t>(msg);
	auto stream = msg->svcid;
	auto setid = msg->flags;
	auto path = val.data;
	dbgSsvcPXL("[%02x,%02x]%s", stream, setid, path);
	auto& item = _streams[stream];
	auto obj = item.obj;
	if (!obj) {
		auto name = _cfg.GetName64(stream);
		returnIfErrC(false, !name);
		obj = Child(name);
		returnIfErrC(false, !obj);
	}
	//
	uint64_t buff[512/8];
	auto rsp = (TMsg<MsgGet_t>*) buff;
	memcpy(buff, msg, sizeof(TMsg<MsgGet_t>));
	rsp->size = sizeof(TMsg<MsgGet_t>) + val.data_size;
	returnIfErrC(false, rsp->size > sizeof(buff));
	auto data = rsp->data;
	returnIfErrC(false, !obj->Get(path, data));
	return Response(true, rsp);
}

bool StreamServer::Process(uint8_t stream, smf_frame_t* frm) {
	AtomicLocker alck(_streams[stream].lock);
	auto obj = _streams[stream].obj;
	returnIfErrC(false, !obj);
	return obj->Run(frm);
}

int StreamServer::getStream(Object* obj) {
	for (int i = 0; i < eStreamMax; i++)
		if (obj == _streams[i].obj)
			return i;
	return -1;
}

bool StreamServer::add(MsgAdd_t& val, streams_t& item, const char* script) {
	auto params = (uint32_t*)&val;
	smf::unique_ptr<Element> ele{ (Element*)Layout(script).Create(this, params) };
	returnIfErrC(false, !ele);
	returnIfErrC(false, !ele->Status(StatusNext(), EStatusChange::up));
	item.obj = ele.get();
	ele.release();
	return true;
}

bool StreamServer::remove(MsgRemove_t& val, streams_t& item) {
	auto name = val.name;
	auto obj = Child(name);
	returnIfWarnC(false, !obj);
	if (obj->Flags().Check(IS_Service)) {
		((Service*)obj)->Status(EStatus::ready,EStatusChange::down);
	}
	//stream locker
	AtomicLocker alck(item.lock);
	item.obj = 0;
	//
	if (obj->Flags().Check(IS_Service)) {
		((Service*)obj)->Status(EStatus::null, EStatusChange::down);
	}
	obj->Parent(0);
	//
	delete obj;
	return true;
}
