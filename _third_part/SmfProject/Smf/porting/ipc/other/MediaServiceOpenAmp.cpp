#include "IMsvc.h"
#include "smf_debug.h"
#include "SmfPorting.h"

namespace smf {

	bool smf_message_init(bool master) {
		return true;
	}

	bool smf_ipc_open(uint8_t cpuid) {
		return true;
	}

	bool smf_ipc_close(uint8_t cpuid) {
		return true;
	}
}