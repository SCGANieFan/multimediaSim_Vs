#include "StreamClient.h"
#include "SmfFCC.h"
#include "SmfDef.h"
#include "SmfICS.h"
#include "SmfMsg.h"
#include "smf_debug_ext.h"
#include "SmfLayout.h"

using namespace smf;
using namespace smf::msg;
using namespace smf::ssvc;

bool StreamClient::PowerOn(uint8_t cpuid) {
	returnIfErrC(false, !smf::PowerOn(cpuid));
	returnIfErrC(false, !ics::IsReady(cpuid, 1000, 2));
	return true;
}

bool StreamClient::PowerOff(uint8_t cpuid) {
	returnIfErrC(false, !smf::PowerOff(cpuid));
	return true;
}
