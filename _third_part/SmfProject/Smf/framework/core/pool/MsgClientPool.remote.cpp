#include "MsgClientPool.remote.h"
#include "MsgClientPool.msg.h"
#include "SmfPorting.h"
#include "SmfHash.h"

using namespace smf;
using namespace smf::msg;
using namespace smf::msg::pool;
using namespace smf::msg::pool::remote;

EXTERNC void smf_msg_client_pool_remote_register() {
	Object::Register<MsgClientPool>("mclient-rpool0",(void*)POOL::Pool0);
}
EXTERNC void smf_msg_client_pool0_remote_register() {
	Object::Register<MsgClientPool>("mclient-rpool0", (void*)POOL::Pool0);
}
EXTERNC void smf_msg_client_pool1_remote_register() {
	Object::Register<MsgClientPool>("mclient-rpool1", (void*)POOL::Pool1);
}
EXTERNC void smf_msg_client_pool2_remote_register() {
	Object::Register<MsgClientPool>("mclient-rpool2", (void*)POOL::Pool2);
}
EXTERNC void smf_msg_client_pool3_remote_register() {
	Object::Register<MsgClientPool>("mclient-rpool3", (void*)POOL::Pool3);
}

MsgClientPool::MsgClientPool() {
	_flags.Set(IS_ASync);
}
void MsgClientPool::initialize(Reflection::item_t* item) {
	IMsgClient::initialize(item);
	_msg = (POOL)(uint32_t)item->param;
	_named.Init((_msg == POOL::Pool0) ? 0 : 1);
}
bool MsgClientPool::init(IMsgService* svc, bool en) {
	returnIfErrC(false, !(uint16_t)_msg);
	if (en) {
		auto pool = _pool;
		if (!pool)pool = &IPool::Globle();
		returnIfErrC(false, !_named.Init(pool, _memCnt));
		MsgRouteRegister(_msg + POOL::Alloc);
		MsgRouteRegister(_msg + POOL::Free);
	}
	else {

	}
	return true;
}
bool MsgClientPool::set(uint32_t keys, void* vals) {
	switch (keys) {
	case Hash("cnt"): _memCnt = (int)vals; return true;
	case Hash("pool"): _pool = IPool::Find((const char*)vals); return true;
	default:return IMsgClient::set(keys,vals);
	}
}
Result MsgClientPool::Process(smf_msg_t* msg) {
	auto cmd = (POOL)msg->cmd - _msg;
	switch (cmd) {
	case POOL::Alloc:return (Result)msgAlloc(msg);
	case POOL::Free: return (Result)msgFree(msg);
	default:return Result::Other;
	}
}

bool MsgClientPool::msgAlloc(smf_msg_t* msg_) {
	if (Flags().CheckAll(IS_ASync)) {
		dbgTestPXL("[async]>0x%03x,%x%x/%02x", msg_->cmd, msg_->cpudst, msg_->cpusrc, msg_->svcid);
	}
	auto msg = MsgGetBody<Alloc_t>(msg_);
	auto size = (msg->size + 63) & ~63;
	auto data = _named.Alloc((const char*)&msg->name, (void*)msg->buff, size, msg->align);
	// returnIfErrC(false, !data);
	returnIfErrCS(false, !data, "[%s],%d", (char*)&msg->name,size);
	msg->buff = (uint64_t)addr_remap_to_dev(data);
	msg->size = size;
	return Response(true, msg_);
}

bool MsgClientPool::msgFree(smf_msg_t* msg_) {
	//if (Flags().CheckAll(IS_ASync)) {
		//dbgTestPXL("[async]>0x%03x,%x%x/%02x", msg_->cmd, msg_->cpudst, msg_->cpusrc, msg_->svcid);
	//}
	auto msg = MsgGetBody<Free_t>(msg_);
	return _named.Free(msg->name ? ((const char*)&msg->name) : (const char*)0, addr_remap_to_cpu((void*)msg->buff));
}
