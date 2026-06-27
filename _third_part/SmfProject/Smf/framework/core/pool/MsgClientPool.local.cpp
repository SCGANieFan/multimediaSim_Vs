#include "MsgClientPool.local.h"
#include "MsgClientPool.msg.h"
#include "SmfFCC.h"
#include "SmfPorting.h"

using namespace smf;
using namespace smf::msg;
using namespace smf::msg::pool;
using namespace smf::msg::pool::local;

EXTERNC void smf_msg_client_pool_register() {
	MsgClientPool::Register<MsgClientPool>("mclient-lpool0",(void*)POOL::Pool0);
}
EXTERNC void smf_msg_client_pool0_register() {
	MsgClientPool::Register<MsgClientPool>("mclient-lpool0", (void*)POOL::Pool0);
}
EXTERNC void smf_msg_client_pool1_register() {
	MsgClientPool::Register<MsgClientPool>("mclient-lpool1", (void*)POOL::Pool1);
}
EXTERNC void smf_msg_client_pool2_register() {
	MsgClientPool::Register<MsgClientPool>("mclient-lpool2", (void*)POOL::Pool2);
}
EXTERNC void smf_msg_client_pool3_register() {
	MsgClientPool::Register<MsgClientPool>("mclient-lpool3", (void*)POOL::Pool3);
}

void MsgClientPool::initialize(Reflection::item_t*item) {
	IMsgClient::initialize(item);
	_msg = (POOL)(uint32_t)item->param;
	Init((_msg == POOL::Pool0) ? 0 : 1);
}

bool MsgClientPool::init(IMsgService* svc, bool en) {
	return true;
}

Result MsgClientPool::Process(smf_msg_t* msg) {
	auto cmd = (POOL)msg->cmd - _msg;
	switch (cmd) {
	case POOL::AllocResponse: 
	case POOL::FreeResponse: return (Result)DefaultProcess(msg);
	default:return Result::Other;
	}
}

void* MsgClientPool::alloc(const char* name, void* buff, unsigned& size, unsigned align) {
	Alloc_t para{ fcc64low(name) ,size, align, (uint64_t)buff };
	returnIfErrC(0, !svc()->SendWait((uint16_t)(_msg + POOL::Alloc), 0, para));
	//dbgTestPXL("%p,%u", para.buff, para.size);
	size = para.size;
	return addr_remap_to_cpu((void*)para.buff);
}
bool MsgClientPool::free(const char* name, void* buff) {
	Free_t para{ fcc64low(name) ,(uint64_t)addr_remap_to_dev(buff)};
	return svc()->Send((uint16_t)(_msg + POOL::Free), para);
}
