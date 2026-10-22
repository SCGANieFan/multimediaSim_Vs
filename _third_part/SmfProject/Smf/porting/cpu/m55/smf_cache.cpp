#include "SmfPorting.h"
#include "smf_debug.h"
#include "smf_cache_def.h"
extern "C"{
#include "hal_cache.h"
}
namespace smf {
	void cache_invalid(void* buff, int size) {
		CACHE_LINE_CHECK(buff, size, 64)
			hal_cache_invalidate(HAL_CACHE_ID_D_CACHE, (uint32_t)buff, size);
	}
	void cache_writeback(void* buff, int size) {
		CACHE_LINE_CHECK(buff, size, 64)
			hal_cache_sync(HAL_CACHE_ID_D_CACHE, (uint32_t)buff, size);
	}
	void cache_flush(void* buff, int size) {
		CACHE_LINE_CHECK(buff, size, 64)
			hal_cache_sync(HAL_CACHE_ID_D_CACHE, (uint32_t)buff, size);
	}
	void icache_invalid(void* buff, int size) {
		// if(((uint32_t)buff >= PSRAM_BASE) && ((uint32_t)buff < (PSRAM_BASE + PSRAM_SIZE))) {
		CACHE_LINE_CHECK(buff, size, 64)
			hal_cache_invalidate(HAL_CACHE_ID_I_CACHE, (uint32_t)buff, size);
		// }
	}
	int cache_line_length() {
		return 64;
	}
}