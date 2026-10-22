#include "hal_timer.h"
#include "SmfPorting.h"
#include "smf_debug.h"
#include "smf_msg.h"
#include <string.h>
#include "hal_sysfreq.h"

#ifdef CHIP_ROLE_CP
#define SMF_CURRENT_CPU SMF_CPU_CP
#else
#define SMF_CURRENT_CPU SMF_CPU_AP
#endif

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