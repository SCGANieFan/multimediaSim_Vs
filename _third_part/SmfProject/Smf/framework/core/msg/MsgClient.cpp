#include "MsgClient.h"
#include "SmfPorting.h"
#include "unique_ptr.h"
#include "SmfMsg.h"
#include "SmfICS.h"
#include "SmfDef.h"
#include "SmfDef.gbl.h"

using namespace smf;
using namespace smf::msg;
using namespace smf::msg::core;

EXTERNC void smf_msg_client_core_register() {
	Object::Register<MsgClient>("mclient-core");
}

MsgClient::MsgClient() {
	Global<MsgClient>(eGlobal::msgcore, this);
}
MsgClient::~MsgClient() {
	Global<MsgClient>(eGlobal::msgcore, 0);
}

bool MsgClient::init(IMsgService* svc, bool enable) {
	return true;
}

Result MsgClient::Process(smf_msg_t* msg) {
	switch ((eCore)msg->cmd) {
	case eCore::set:return (Result)Response(msgSet(msg),msg);
	case eCore::setResponse:return Result::True;
	case eCore::config:return (Result)Response(msgConfig(msg),msg);
	case eCore::configResponse:return Result::True;
	case eCore::invoke:return (Result)Response(msgInvoke(msg), msg);
	case eCore::invokeResponse:return Result::True;
	default:return Result::Other;
	}
}

bool MsgClient::msgSet(smf_msg_t* msg) {
	auto val = MsgGetBody<set_t>(msg);
	auto flags = msg->flags;
	auto path = val->buff + val->path;
	auto para = val->buff + val->params;
	auto vals = flags ? (((void**)para)[0]) : ((void*)para);
	return Root().Set(path, vals);
}

bool MsgClient::msgConfig(smf_msg_t* msg) {
	auto val = MsgGetBody<config_t>(msg);
	auto script = val->buff + val->script;
	auto params = val->buff + val->params;
	return Root().Add(script,params);
}
bool MsgClient::msgInvoke(smf_msg_t*msg) {
	auto val = MsgGetBody<invoke_t>(msg);
	if (val->cb) {
		val->cb(val->priv);
	}
	return true;
}

bool MsgClient::MsgSet(uint8_t cpuid, const char* path, const void* params, int param_size, bool isval) {
	returnIfErrC(false, !path);
	auto slen = strlen(path) + 1;
	auto size = slen + param_size + sizeof(TMsg<set_t>);
	VoidBuffer buff(size);
	returnIfErrC(false, !buff);
	auto msg = buff.get<TMsg<set_t>>();
	memset(msg, 0, size);
	msg->cmd = (uint16_t)eCore::set;
	msg->size = size;
	msg->cpudst = cpuid;
	msg->flags = isval ? 1 : 0;
	msg->params = 0;
	msg->path = param_size;
	auto msg_path = msg->buff + msg->path;
	auto msg_params = msg->buff + msg->params;
	//strcpy(msg_path, path);
	memcpy(msg_path, path, slen);
	if (params && param_size) {
		memcpy(msg_params, params, param_size);
	}
	return Send(msg);
}
bool MsgClient::MsgSet(uint8_t cpuid, const char* path, uint32_t vals) {
	return MsgSet(cpuid, path, &vals, 4, true);
}
bool MsgClient::MsgConfig(uint8_t cpuid, const char* script, const void* params, int param_size) {
	returnIfErrC(false, !script);
	auto slen = strlen(script) + 1;
	auto size = slen + param_size + sizeof(TMsg<config_t>);
	VoidBuffer buff(size);
	returnIfErrC(false, !buff);
	auto msg = buff.get<TMsg<config_t>>();
	memset(msg, 0, size);
	msg->cmd = (uint16_t)eCore::config;
	msg->size = size;
	msg->cpudst = cpuid;
	msg->params = 0;
	msg->script = param_size;
	auto msg_script = msg->buff + msg->script;
	auto msg_params = msg->buff + msg->params;
	//strcpy(msg_script, script);
	memcpy(msg_script, script, slen);
	if (params && param_size) {
		memcpy(msg_params, params, param_size);
	}
	return Send(msg);
}
bool MsgClient::MsgInvoke(smf_cb_priv0 cb, void* priv) {
	TMsg<invoke_t> msg;
	msg.cmd = (uint16_t)eCore::config;
	msg.size = sizeof(msg);
	msg.cpudst = get_cpu_idx();
	msg.cb = cb;
	msg.priv = priv;
	return Send(&msg);
}
bool MsgClient::MsgInvokeFree(void* buff) {
	return MsgInvoke([](void* priv) {
		if (priv)Free(priv);
		}, buff);
}
bool MsgClient::MsgInvokeDelete(Object* obj) {
	return MsgInvoke([](void* priv) {
		if(priv)delete (Object*)priv;
		},obj);
}
