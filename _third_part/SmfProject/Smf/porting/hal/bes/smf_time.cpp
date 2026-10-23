/*
 * hifi4.c
 *
 *  Created on: 2021/10/29
 *      Author: wangjg
 */
#include "SmfPorting.h"
#include "smf_debug.h"
extern "C" {
#include <hal_timer.h>
#include "cmsis.h"
}

#ifndef SLIM_TICKS_TO_MS
#define SLIM_TICKS_TO_MS __SLIM_TICKS_TO_MS
#endif

#ifndef FAST_TICKS_TO_US64
#define FAST_TICKS_TO_US64 FAST_TICKS_TO_US
#define hal_fast_sys_timer64_get() hal_fast_sys_timer_get()
#endif

namespace smf {
	static int64_t _offset_us = 0;
	void timer_sync() {
		uint32_t lockid = int_lock();
		auto fast_tick = hal_fast_sys_timer64_get();
		auto sys_tick = hal_sys_timer_get();
		int_unlock(lockid);
		auto fact_us = FAST_TICKS_TO_US64((uint64_t)fast_tick);
		auto sys_us = SLIM_TICKS_TO_MS((uint64_t)sys_tick) * 1000;
		_offset_us = sys_us - fact_us;
		dbgTestPXL("%d=%u-%u", (int)_offset_us, (uint32_t)sys_us, (uint32_t)fact_us);
	}
	uint32_t get_ms() {
		return (uint32_t)(get64_us() / 1000);
	}
	uint32_t get_us() {
		return (uint32_t)get64_us();
	}
	uint64_t get64_ms() {
		return get64_us() / 1000;
	}
	uint64_t get64_us() {
		auto ticks = hal_fast_sys_timer64_get();
		return FAST_TICKS_TO_US64((uint64_t)ticks) + _offset_us;
	}

	int64_t get_local_us() { 
		return get64_us(); 
	}
	int64_t get_local_ms() {
		return get64_ms(); 
	}

	uint64_t get64_ticks() {
		return hal_fast_sys_timer64_get();
	}
	uint32_t get_ticks() {
		return hal_sys_timer_get();
	}
	uint32_t get_timestamp() {
		auto ticks = hal_sys_timer_get();
		return SLIM_TICKS_TO_MS(ticks);
	}

	void set64_us(uint64_t us) {
		uint64_t ticks = US_TO_FAST_TICKS64(us);
		dbgTestPXL("%llu,llu", us, ticks);
		dbgTestPXL("%llu", get64_ms());
		hal_fast_sys_timer64_set(ticks);
		dbgTestPXL("%llu", get64_ms());
	}

	void set64_ticks(uint64_t ticks) {
		dbgTestPXL("%llu", get64_ms());
		hal_fast_sys_timer64_set(ticks);
		dbgTestPXL("%llu", get64_ms());
	}
}