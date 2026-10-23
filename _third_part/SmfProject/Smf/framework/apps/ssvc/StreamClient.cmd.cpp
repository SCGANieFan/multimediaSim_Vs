#include "StreamClient.h"
#include "SmfFCC.h"
#include "SmfMsg.h"
#include "smf_debug_ext.h"
#include "SmfPortingDevice.h"
#include "SmfPortingUser.h"

using namespace smf;
using namespace smf::msg;
using namespace smf::ssvc;

bool StreamClient::SetJob(bool enable) {
	TMsg<MsgJob_t> msg;
	memset(&msg, 0, sizeof(msg));
	msg.size = sizeof(msg);
	msg.cmd = (uint16_t)eStream::job;
	msg.svcid = 0;
	msg.flags = enable ? 1 : 0;
	returnIfErrC(false, !Send(&msg));
	return true;
}

bool StreamClient::Job(smf_msg_t* msg) {
	bool enable = msg->flags;
	if (enable) {
		JobHold();
	}
	else {
		JobRelease();
	}
	return true;
}

bool StreamClient::Device(smf_msg_t* msg) {
	auto res = MsgGetBody<MsgDevice_t>(msg);
	auto devid = (MediaDevice_e)msg->svcid;
	auto op = msg->flags;
	auto& cfg = res->cfg;
	dbgExtPXL(1,"[%02x,%02x]%d,%d", devid, op, cfg.master, cfg.chmap);
	switch (op) {
	case 0: {//stop
		smf_device_notify((smf_media_device_e)devid, (smf_media_device_state_e)op);
		auto ret = smf_device_stop(devid);
		returnIfErrC(false, ret != 0);
		smf_device_stop_success_notify((smf_media_device_e)devid);
		return true;
	}
	case 1: {//start
		auto ret = smf_device_start(devid, &cfg);
		returnIfErrC(false, ret != 0);
		smf_device_notify((smf_media_device_e)devid, (smf_media_device_state_e)op);
		return true;
	}
	default:dbgErrPXL("opt unavailable: %d", op);
	}
	return false;
}
