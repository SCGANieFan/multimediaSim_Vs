#include "IMsgClient.h"
#include "SmfPorting.h"
using namespace smf;
using namespace smf::msg;

///
MsgClientBase::MsgClientBase(Object* obj):_this(obj) {
}

IMsgService* MsgClientBase::svc() const { return (IMsgService*)(_this->Parent()); }

bool MsgClientBase::set(uint32_t keys, void* vals) {
	switch (keys) {
	case Hash("cpuid"):
	case Hash("TCPU"):
	case Hash("targetCPU"): _targetCpu = (uint8_t)(uint32_t)vals; return true;
	case Hash("async"): _this->Flags().Set(IS_ASync, vals); return true;
	default: return false;// Service::set(keys, vals);
	}
}

bool MsgClientBase::get(uint32_t keys, void* vals) const {
	switch (keys) {
	case Hash("cpuid"):
	case Hash("TCPU"):
	case Hash("targetCPU"):
	case Hash("TID"):
	case Hash("targetID"):*(uint8_t*)vals = _targetCpu; return true;
	default:return false;// Service::get(keys, vals);
	}
}

bool MsgClientBase::DefaultProcess(smf_msg_t* msg) {
	switch ((EXT)msg->ext){
	case EXT::callback:
		if ((msg->size >= sizeof(MsgExtCallback)) && MsgIsResponse(msg)) {
			auto ext = (MsgExtCallback*)msg;
			auto callback = (CbMsg)ext->callback;
			if (callback) {
				return callback(ext);
			}
		}
		return false;
	case EXT::wait:
		if ((msg->size >= sizeof(MsgExtWait)) && MsgIsResponse(msg)) {
			auto ext = (MsgExtWait*)((MsgExtWait*)msg)->msg;
			char32_t val0 = 0;
			char32_t val1 = 0;
			//cp msg to response
			if (ext && ext->wait == 0x5a5a5a5a5a5a5a5aull) {
				memcpy(ext, msg, msg->size);
				ext->wait = 0x7a7a7a7a7a7a7a7aull;
			}
			return true;
		}
		return false;
	default:
		return true;
	}
	return true;
}

bool MsgClientBase::Send(SmfMsg_t* msg) const {
	if (!msg->cpudst && _targetCpu)
		msg->cpudst = _targetCpu;
	return svc() ? svc()->Send(msg) : false;
}
bool MsgClientBase::Send(uint16_t cmd) const {
	SmfMsg_t msg;
	//memset(&msg, 0, sizeof(msg));
	msg.head = 0;
	msg.cmd = cmd;
	msg.size = sizeof(msg);
	return Send(&msg);
}
bool MsgClientBase::Send(void* msg) const {
	return Send((SmfMsg_t*)msg);
}
bool MsgClientBase::Send(uint16_t cmd, void* data, int size, uint8_t flags, uint8_t cpuid) const {
	auto total = sizeof(SmfMsg_t) + size;
	uint32_t buff[248 >> 2];
	// dbgTestPXL("%03x,%d,%d", cmd, total, sizeof(buff));
	returnIfErrC(false, total > sizeof(buff));
	auto msg = (SmfMsg_t*)buff;
	//memset(buff, 0, total);
	msg->head = 0;
	msg->cmd = cmd;
	msg->size = total;
	msg->cpudst = cpuid;
	msg->flags = flags;
	memcpy(msg + 1, data, size);
	return Send((SmfMsg_t*)msg);
}
bool MsgClientBase::SendWait(SmfMsg_t* msg, uint32_t timeout) const {
	returnIfErrC(false, msg->size < sizeof(MsgExtWait));
	auto ext = (MsgExtWait*)msg;
	ext->ext = (uint8_t)EXT::wait;
	ext->wait = 0x5a5a5a5a5a5a5a5aull;
	ext->msg = (uint64_t)ext;
	returnIfErrC(false, !Send(msg));
	//
	auto tp0 = get_ms();
	while (ext->wait != 0x7a7a7a7a7a7a7a7aull) {
		auto tp = get_ms();
		if (tp - tp0 > timeout) {
			ext->wait = 0;
			dbgWarnPXL("timeout:%u", timeout);
			sleep_for(10);
			break;
		}
		sleep_for(2);
	}
	returnIfWarnCS(false, !ext->result, "%03x", msg->cmd);
	return true;
}
bool MsgClientBase::SendWait(void* msg, uint32_t timeout) const {
	return SendWait((SmfMsg_t*)msg, timeout);
}
bool MsgClientBase::SendWait(void* msg_, int size, uint32_t timeout) const {
	auto msg = (MsgExtWait*)msg_;
	msg->size = size;
	return SendWait((SmfMsg_t*)msg, timeout);
}
bool MsgClientBase::SendWait(uint16_t cmd, uint8_t cpuid, void* data, int size, uint8_t* flags, uint32_t timeout) const {
	auto total = sizeof(MsgExtWait) + size;
	uint32_t buff[248 >> 2];
	returnIfErrC(false, total > sizeof(buff));
	auto msg = (MsgExtWait*)buff;
	//memset(msg, 0, total);
	msg->head = 0;
	msg->cmd = cmd;
	msg->size = total;
	msg->flags = flags ? *flags : 0;
	msg->cpudst = cpuid;
	memcpy(msg + 1, data, size);
	auto rst = SendWait((SmfMsg_t*)msg, timeout);
	if (flags)*flags = msg->flags;
	memcpy(data, msg + 1, size);
	return rst;
}
bool MsgClientBase::SendCallback(SmfMsg_t* msg, CbMsg cb, void* priv) const {
	returnIfErrC(false, msg->size < sizeof(MsgExtCallback));
	auto ext = (MsgExtCallback*)msg;
	ext->ext = (uint8_t)EXT::callback;
	ext->callback = (uint64_t)cb;
	ext->priv = (uint64_t)priv;
	returnIfErrC(false, !Send(msg));
	return true;
}
bool MsgClientBase::SendCallback(void* msg_, int size, CbMsg cb, void* priv) const {
	auto msg = (SmfMsg_t*)msg_;
	msg->size = size;
	return SendCallback((SmfMsg_t*)msg, cb, priv);
}
bool MsgClientBase::SendCallback(uint16_t cmd, uint8_t cpuid, void* data, int size, uint32_t flags, CbMsg cb, void* priv) const {
	auto total = sizeof(MsgExtCallback) + size;
	uint32_t buff[248 >> 2];
	returnIfErrC(false, total > sizeof(buff));
	auto msg = (MsgExtCallback*)buff;
	//memset(msg, 0, total);
	msg->head = 0;
	msg->cmd = cmd;
	msg->size = total;
	msg->flags = flags;
	msg->cpudst = cpuid;
	memcpy(msg + 1, data, size);
	auto rst = SendCallback((SmfMsg_t*)msg, cb, priv);
	return rst;
}
bool MsgClientBase::Response(bool rst, void* msg) const {
	return Response(rst, (SmfMsg_t*)msg);
}
bool MsgClientBase::Response(bool rst, SmfMsg_t* msg) const {
	MsgToResponse(msg);
	auto id = msg->cpudst;
	msg->cpudst = msg->cpusrc;
	msg->cpusrc = id;
	msg->result = rst;
	return Send(msg);
}
bool MsgClientBase::Response(bool rst, uint16_t cmd, uint16_t cpuid) const {
	//returnIfErrC(false, !(cmd & 1));
	cmd |= 1;
	dbgTestPXL("%d, %d", rst, cmd);
	SmfMsgResponse_t res;
	res.head = 0;
	res.cmd = cmd;
	res.size = sizeof(res);
	res.cpudst = cpuid;
	//res.svcid = id;
	res.result = rst;
	//res.error = error;
	return Send(&res);
}
bool MsgClientBase::Response(bool rst, uint16_t cmd, void* para, int size) const {
	cmd |= 1;
	dbgTestPXL("%d,%d", rst, cmd);
	uint32_t buff[120 >> 2];
	auto res = (SmfMsgResponse_t*)buff;
	res->head = 0;
	res->cmd = cmd;
	res->size = sizeof(SmfMsgResponse_t) + size;
	res->svcid = 0;
	res->result = rst;
	//res->error = 0;
	memcpy(res + 1, para, size);
	return Send(res);
}
