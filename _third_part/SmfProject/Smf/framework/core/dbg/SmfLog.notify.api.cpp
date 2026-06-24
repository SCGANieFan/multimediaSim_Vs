#include "Object.h"
#include "SmfICS.h"
#include "SmfDef.gbl.h"
#include "SmfMsg.h"
#include "smf_debug.h"
using namespace smf;

EXTERNC void smf_notify_log_enable(uint32_t delay) {
	if (delay) {
		const char* serial = "notify,notify,0,delay=$0;";
		uint32_t paras[]{ delay };
		returnIfErrC0(!Deserialize(serial, paras, "bg"));
	}
	else {
		Root().Set("bg/notify/en", false);
	}
}

bool smf::SysFreqRequest(uint32_t freq) {
	// return Root().Set("bg/notify/sysfreqRequest", freq);
	return RemoteSet(SMF_CPU_AP, "bg/notify/sysfreqRequest", freq);
}

bool smf::PowerOn(uint8_t cpuid) {
	returnIf(false, cpuid < 1);
	returnIf(false, cpuid > 7);
	returnIf(true, !GlobalFlagsGet((eGlobalFlags)(1u << cpuid)));
	auto& ref = ics::CpuRefs(cpuid);
	ics::Locker lck(1000);
	//dbgTestPXL("[%u]%u,%u", cpuid, ref, ics::IsPowerOn(cpuid));
	//
	if (!ref) {
		returnIfErrC(false, !RemoteSet(SMF_CPU_AP, "bg/notify/powerOn", cpuid));
		ref = 1;
	}
	else {
		ref++;
	}
	//dbgTestPXL("[%u]%u,%u", cpuid, ref, ics::IsPowerOn(cpuid));
	return true;
}

bool smf::PowerOff(uint8_t cpuid) {
	returnIf(false, cpuid < 1);
	returnIf(false, cpuid > 7);
	returnIf(true, !GlobalFlagsGet((eGlobalFlags)(1u << cpuid)));
	auto& ref = ics::CpuRefs(cpuid);
	ics::Locker lck(1000);
	//dbgTestPXL("[%u]%u,%u", cpuid, ref, ics::IsPowerOn(cpuid));
	//
	if (ref == 0) {
	}
	else if (ref == 1) {
		returnIfErrC(false, !RemoteSet(SMF_CPU_AP, "bg/notify/powerOff", cpuid));
		ref = 0;
	}
	else {
		ref--;
	}
	//dbgTestPXL("[%u]%u,%u", cpuid, ref, ics::IsPowerOn(cpuid));
	return true;
}
