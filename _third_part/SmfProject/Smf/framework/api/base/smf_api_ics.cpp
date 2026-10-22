#include "smf_debug.h"
#include "SmfPorting.h"
#include "SmfDef.h"
#include "SmfICS.h"
using namespace smf;

EXTERNC void* smf_ics_get() {
	return ICSBlock();
}

EXTERNC void smf_register_ics(void* ics, int size, bool master) {
	ics::Register(ics, size, master);
}

EXTERNC bool smf_ics_is_poweron(uint8_t cpuid) {
	return ics::IsPowerOn(cpuid);
}

EXTERNC void smf_ics_poweron(uint8_t cpuid) {
	ics::SetPower(true, cpuid);
}

EXTERNC void smf_ics_poweroff(uint8_t cpuid) {
	ics::SetPower(false,cpuid);
}

EXTERNC bool smf_ics_sync_ready(uint8_t cpuid, int timeout, int skip) {
	return ics::IsReady(cpuid, timeout, skip);
}

EXTERNC void smf_ics_ready() {
	ics::SetReady();
}

EXTERNC bool smf_ics_check(void* ics, int size) {
	return ics::CheckIcsBuffer(ics, size);
}

EXTERNC void smf_ics_set_ready(uint8_t cpuid) {
	ics::SetReady(cpuid);
}

EXTERNC smf_display_t* smf_get_display(uint8_t index) {
	return (smf_display_t*)ics::GetDisplay(index);
}

EXTERNC bool smf_register_script(void* addr, int size) {
	return ics::SetScript(addr, size);
}

EXTERNC uint64_t smf_cpu_name64(uint8_t cpuid) {
	return ics::GetCpuName64(cpuid);
}

EXTERNC const char* smf_cpu_name(uint8_t cpuid) {
	return ics::GetCpuName(cpuid);
}
