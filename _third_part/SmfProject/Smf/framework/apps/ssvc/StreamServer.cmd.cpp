#include "StreamServer.h"
#include "StreamClient.h"
#include "SmfHash.h"
#include "ShmHook.h"
#include "SmfFCC.h"
#include "SmfICS.h"
#include "smf_debug_ext.h"

using namespace smf;
using namespace smf::msg;
using namespace smf::ssvc;

bool StreamServer::Device(uint8_t device, uint8_t op, MediaDeviceCfg_t* cfg) {
	TMsg<MsgDevice_t> msg;
	memset(&msg, 0, sizeof(msg));
	msg.size = sizeof(msg);
	msg.cmd = (uint16_t)eStream::device;
	msg.cpudst = cfg->device_cpuid;
	msg.svcid = (uint8_t)device;
	msg.flags = (uint8_t)op;
	if(cfg) msg.cfg = *cfg;
	//dbgTestPXL("[%02x,%02x]%p", device, op, cfg);
	return Send(&msg);
}
