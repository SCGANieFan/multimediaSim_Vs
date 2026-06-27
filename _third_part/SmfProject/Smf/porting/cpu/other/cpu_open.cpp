#include <stdio.h>
#include "SmfPorting.h"
#include "smf_debug.h"
//EXTERNC int dsp_media_service_sync(int timeout);
namespace smf {
	bool cpu_open(uint8_t cpuid) {
		return true;
	}
	bool cpu_close(uint8_t cpuid) {
		return true;
	}
	bool cpu_opt(uint8_t cpuid, uint8_t open) {
		return open ? cpu_open(cpuid) : cpu_close(cpuid);
	}
}
