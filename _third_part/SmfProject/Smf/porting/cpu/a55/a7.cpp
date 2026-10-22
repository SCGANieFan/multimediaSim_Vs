#include "smf_debug.h"
#include "SmfPorting.h"
#include "smf_msg.h"
#include "hal_timer.h"
namespace smf {
	uint32_t get_cpu_cycle() {
		return 0;
	}

	uint64_t get64_cpu_cycle() {
		return 0;
	}
	unsigned get_cpu_freq() {
		return hal_sys_timer_calc_cpu_freq(5, 0);
	}

	void* memset64_0(void* dst0, int size) {
		memset(dst0, 0, size);
		return dst0;
	}
}

