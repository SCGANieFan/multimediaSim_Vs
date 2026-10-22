/*
 * hifi4.c
 *
 *  Created on: 2021/10/29
 *      Author: wangjg
 */
#include "SmfPorting.h"
#include "smf_debug.h"
#include "smf_cache_def.h"
#include <hal_timer.h>
#include <hal_sram.h>
#include <plat_addr_map.h>
#include <xtensa/hal.h>
#include <xtensa/xtruntime.h>

// #ifndef PSRAM_BASE
// #define PSRAM_BASE 0x18000000
// #endif
// #ifndef PSRAM_SIZE
// #define PSRAM_SIZE 0x4000000
// #endif

bool is_cached(void* addr32);
namespace smf{
	void cache_invalid(void* buff, int size) {
		// if(((uint32_t)buff >= PSRAM_BASE) && ((uint32_t)buff < (PSRAM_BASE + PSRAM_SIZE))) {
			CACHE_LINE_CHECK(buff, size, 64)
			xthal_dcache_region_invalidate(buff, size);
		// }
	}
	void cache_writeback(void* buff, int size) {
		// if(((uint32_t)buff >= PSRAM_BASE) && ((uint32_t)buff < (PSRAM_BASE + PSRAM_SIZE))) {
			CACHE_LINE_CHECK(buff, size, 64)
			xthal_dcache_region_writeback(buff, size);
		// }
	}
	void cache_flush(void* buff, int size) {
		// if(((uint32_t)buff >= PSRAM_BASE) && ((uint32_t)buff < (PSRAM_BASE + PSRAM_SIZE))) {
			CACHE_LINE_CHECK(buff, size, 64)
			xthal_dcache_region_writeback_inv(buff, size);
		// }
	}

	void icache_invalid(void* buff, int size) {
		// if(((uint32_t)buff >= PSRAM_BASE) && ((uint32_t)buff < (PSRAM_BASE + PSRAM_SIZE))) {
			CACHE_LINE_CHECK(buff, size, 64)
			xthal_icache_region_invalidate(buff,size);
		// }
	}

	int cache_line_length() {
		return 64;
	}

	void cache_region_writeback_inv(void* dst, uint32_t size) {
		CACHE_LINE_CHECK(dst, size, 64)
		xthal_dcache_region_writeback_inv(dst, size);
	}
	void cache_flush_all(void) {
		xthal_dcache_all_writeback_inv();
	}
	void cache_invalid_all(void) {
		xthal_dcache_all_invalidate();

	}
}




