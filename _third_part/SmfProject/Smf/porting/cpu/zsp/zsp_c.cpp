/*
 * zsp.c
 *
 *  Created on: 2023/07/26
 *      Author: wangjg
 */
#include <zsp_util.h>
#include  "SmfPorting.h"
#include "smf_msg.h"
#include "hal_timer.h"

namespace smf {
	uint64_t get64_cpu_cycle() {
		return 0;
	}
	uint32_t get_cpu_cycle() {
		return get64_cpu_cycle();
	}

	unsigned get_cpu_freq() {
		return hal_sys_timer_calc_cpu_freq(1, 0);
	}
}