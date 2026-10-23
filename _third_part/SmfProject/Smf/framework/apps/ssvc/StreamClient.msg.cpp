#include "StreamClient.h"
#include "HookDefine.h"
#include "SmfFCC.h"
#include "SmfMsg.h"
#include "smf_debug_ext.h"
#include "SmfLayout.h"

using namespace smf;
using namespace smf::msg;
using namespace smf::ssvc;

bool StreamClient::init(IMsgService* svc, bool enable) {
	if (enable) {
		returnIfErrC(false, !_cfg);
		if (_flagsExt.Check((uint32_t)eFlagsExt::job)) {
			MsgRouteRegister(eStream::job);
		}
	}
	return true;
}
Result StreamClient::Process(smf_msg_t* msg) {
	auto svc = (eStream)(msg->cmd & 0xff0);
	auto cmd = (eStream)(msg->cmd & 0x00f);
	switch (svc) {
	case eStream::audioPlayer:
	case eStream::audioRecord:
	case eStream::streamServer:
		switch (cmd) {
		case eStream::addResponse:return (Result)AddResponse(msg);
		case eStream::removeResponse:return (Result)RemoveResponse(msg);
		case eStream::setResponse:return Result::True;
		case eStream::getResponse:return (Result)GetResponse(msg);
		default: return Result::Other;
		}
		break;
	case eStream::streamCommand:
		switch ((eStream)msg->cmd) {
		case eStream::job:return (Result)Job(msg);
		case eStream::device:return (Result)Device(msg);
		case eStream::notify:return (Result)Notify(msg);
		default: return Result::Other;
		}
		break;
	default:
		switch ((eHook)msg->cmd) {
		case eHook::hookResponse:
		case eHook::hookServerResponse:return Result::True;
		default:return Result::Other;
		}
	}
	return Result::Other;
}

bool StreamClient::Notify(smf_msg_t* msg) {
	auto& val = *MsgGetBody<MsgNotify_t>(msg);
	auto stream = msg->svcid;
	auto msgid = msg->flags;
	auto seqno = val.seqNO;
	dbgSsvcPXL("[%02x,%02x,%02x]%u", stream, msgid, seqno, val.data_size);
	switch ((eNotify)msgid) {
	case eNotify::eos:
		dbgSsvcPXL("eos");
		//StopWithSeqNO(stream, seqno);
		break;
	case eNotify::error:
		dbgSsvcPXL("error");
		//StopWithSeqNO(stream, seqno);
		break;
	case eNotify::done:
		dbgSsvcPXL("done");
		//StopWithSeqNO(stream, seqno);
		break;
	default:
		break;
	}
	return Notify(msgid, stream, seqno, val.data, val.data_size);
}
bool StreamClient::Notify(uint8_t msgid, uint8_t stream, uint8_t seqno, void* data, int size) {
	bool have = false;
	for (int i = 0; i < SMF_STREAM_MESSAGE_CALLBACK_end; i++) {
		have = have || (uint32_t)_notifys[i].func;
	}
	if (have) {
		if (_flagsExt.Check(eFlagsExt8::NotifyInvoke)) {
			auto buf = Alloc(sizeof(smf_stream_message_t) + size);
			returnIfErrC(false, !buf);
			auto msg = new(buf) smf_stream_message_t{ this, msgid, stream, seqno, (uint8_t)size };
			if (size && data) {
				memcpy(msg->data, data, size);
			}
			return Invoke([](void* priv) {
				auto para = (smf_stream_message_t*)priv;
				auto this_ = (StreamClient*)para->priv;
				this_->NotifyCallback(*para);
				Free(para);
				return true;
				}, msg);
		}
		else {
			uint64_t buff[512/8];
			auto& msg = *(smf_stream_message_t*)buff;
			msg.priv = 0;
			msg.msgid = msgid;
			msg.stream = stream;
			msg.seqno = seqno;
			msg.data_size = size;
			//smf_stream_message_t msg{ 0, msgid, stream, (uint16_t)size };
			returnIfErrCS(false, size > sizeof(buff), "size=%d",size);
			if (size && data) {
				memcpy(msg.data, data, size);
			}
			return NotifyCallback(msg);
		}
	}
	return true;
}
bool StreamClient::NotifyCallback(smf_stream_message_t& msg) {
	auto ms0 = get_ms();
	for (int i = 0; i < SMF_STREAM_MESSAGE_CALLBACK_end; i++) {
		auto callback = _notifys[i];
		if (callback.func) {
			msg.priv = callback.priv;
			callback.func((smf_stream_message_t*)&msg);
			auto ms1 = get_ms();
			if ((ms1 > ms0) && (ms1 - ms0 >= 10)) {
				dbgWarnPXL("[%u]timeout(%u)[%02x,%02x]%u", i, ms1 - ms0, msg.stream, msg.msgid, msg.data_size);
			}
		}
	}
	return true;
}
bool StreamClient::GetResponse(smf_msg_t* msg_) {
	auto msg = (TMsg<MsgGet_t>*)msg_;	
	if (msg->callback) {
		auto cb = (smf_cb_data)msg->callback;
		cb((void*)msg->callback_priv, msg->flags, msg->data, msg->data_size);
		return true;
	}
	returnIfErrC(true, get_timestamp() > msg->timepoint);
	if (msg->buff && msg->data_size) {
		memcpy((void*)msg->buff, msg->data, msg->data_size);
	}
	if (msg->sema && *(semaphore*)msg->sema) {
		((semaphore*)msg->sema)->post();
	}
	return true;
}
bool StreamClient::AddResponse(smf_msg_t* msg_) {
	auto stream = msg_->svcid;
	auto seqno = msg_->flags;
	auto seqnoX = _cfg.GetSeqNO(stream);
	auto& rst = _streams[stream];
	dbgExtPXL(1,"[%02x,%x,%x]%u/%u,%u", stream,seqno, seqnoX, rst.idx, rst.cnt, rst.result);
	returnIfErrC(false, seqno != seqnoX);
	rst.result = rst.result && msg_->result;
	rst.idx++;
	if (rst.idx >= rst.cnt) {
		auto evt = rst.result ? eNotify::start_sucess : eNotify::start_fail;
		if (!rst.result) {
			Stop(stream);
		}
		return Notify((uint8_t)evt, stream, seqno);
	}
	return true;
}
bool StreamClient::RemoveResponse(smf_msg_t* msg_) {
	auto stream = msg_->svcid;
	auto seqno = msg_->flags;
	auto seqnoX = _cfg.GetSeqNO(stream);
	auto& rst = _streams[stream];
	dbgExtPXL(1,"[%02x,%x,%x]%u/%u,%u", stream, seqno, seqnoX, rst.idx, rst.cnt, rst.result);
	returnIfErrC(false, seqno != seqnoX);
	rst.result = rst.result && msg_->result;
	rst.idx++;
	if (rst.idx >= rst.cnt) {
		auto evt = rst.result ? eNotify::stop_sucess : eNotify::stop_fail;
		return Notify((uint8_t)evt, stream, seqno);
	}
	return true;
}

bool StreamClient::StopWithSeqNO(uint8_t stream, uint8_t seqno) {
	unique_lock<mutex> lck(_mtx);
	//auto& item = _streams[stream];
	auto seqnoX = _cfg.GetSeqNO(stream);
	dbgExtPXL(1,"[%02x]%02x,%02x", stream, seqno, seqnoX);
	if (seqno == seqnoX) {
		return Stop(stream, true);
	}
	return true;
}
