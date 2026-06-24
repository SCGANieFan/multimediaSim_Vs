#include "SmfCmd.h"
#include "SmfHash.h"
#include "SmfFCC.h"
#include "SmfPorting.h"
#include "smf_debug.h"
#include "smf_api.h"

using namespace smf;

EXTERNC void smf_cmd_power_register() {
	smf_cmd_register("power", [](int argc, char** argv) {
		returnIfErrC0(argc < 3);
		auto cpuid = strtoul(argv[1],0,0);
		switch (Hash(argv[2])) {
		case Hash("on"):
			PowerOn(cpuid);
			break;
		case Hash("off"):
			PowerOff(cpuid);
			break;
		}
		});
	smf_cmd_register("freq", [](int argc, char** argv) {
		returnIfErrC0(argc < 2);
		auto freq = strtoul(argv[1], 0, 0);
		SysFreqRequest(freq * 1000000);
		});
}