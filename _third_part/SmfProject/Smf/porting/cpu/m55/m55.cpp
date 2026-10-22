#include "hal_timer.h"
#include "SmfPorting.h"
#include "smf_debug.h"
#include "smf_msg.h"
#include <string.h>
#include "hal_sysfreq.h"
namespace smf {
	uint32_t get_cpu_cycle() {
		return 0;
	}

	uint64_t get64_cpu_cycle() {
		return 0;
	}
	unsigned get_cpu_freq() {
		return hal_sys_timer_calc_cpu_freq(1, 0);
	}
}