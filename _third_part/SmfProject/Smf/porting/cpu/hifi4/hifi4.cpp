/*
 * hifi4.c
 *
 *  Created on: 2021/10/29
 *      Author: wangjg
 */
#include "SmfPorting.h"
#include "smf_debug.h"
#include "smf_msg.h"
#include <hal_timer.h>
#include <hal_sram.h>
#include <plat_addr_map.h>
#include <xtensa/hal.h>
#include <xtensa/xtruntime.h>
#include "cmsis.h"
#include "cmsis_os.h"
//#include "cmsis_os2.h"                  // ::CMSIS:RTOS2

namespace smf {
	uint32_t get_cpu_cycle() {
		return xthal_get_ccount();
	}

	uint64_t get64_cpu_cycle() {
		static uint32_t _cycle = 0;
		static uint32_t _cycleCnt = 0;
		auto cycle = get_cpu_cycle();
		if (cycle <= _cycle) {
			_cycleCnt++;
		}
		_cycle = cycle;
		return 0x100000000ull * _cycleCnt + cycle;
	}
	unsigned get_cpu_freq() {
		static uint32_t _ticks = 0;
		static uint32_t _us = 0;
		static uint32_t _freq = 0;

		auto x = int_lock();		
		uint32_t us1 = get_us();
		uint32_t tick0 = _ticks;
		uint32_t tick1 = get_cpu_cycle();
		uint32_t us0 = _us;
		uint32_t us = us1 - us0;
		if (us > 10000) {			
			if (us0 < us1 && tick0 < tick1 && tick0 && us0) {
				_freq = ((tick1 - tick0) * 1000000ull / us);
			}
			_ticks = tick1;
			_us = us1;
		}		
		int_unlock(x);

		return _freq;
	}

	//void* get_flash_addr(void) {
	//	return hal_cmu_get_flash_addr();
	//}
}

