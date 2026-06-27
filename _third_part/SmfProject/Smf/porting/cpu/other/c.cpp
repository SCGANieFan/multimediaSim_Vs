/*
 * hifi4.c
 *
 *  Created on: 2021/10/29
 *      Author: wangjg
 */

#include "SmfPorting.h"
#include "smf_debug.h"
#include "smf_msg.h"
namespace smf {
	uint32_t get_cpu_cycle() {
		return 0;
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
		static uint32_t _tp = 0;
		auto tick0 = _ticks;
		auto tick1 = _ticks = get_cpu_cycle();
		auto tp0 = _tp;
		auto tp1 = _tp = get_ms();
		auto khz = (tp0 != tp1) ? ((uint32_t)(tick1 - tick0) / (tp1 - tp0) / 1000) : 0;
		//dbgTestPXL("[%d]tp:%d/%d,tick:%d/%d", mhz, tp0,tp1,tick0,tick1);
		return khz;
	}

	//void* get_flash_addr(void) {
	//	return hal_cmu_get_flash_addr();
	//}
}
