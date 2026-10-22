
#include "SmfPorting.h"
#include "smf_debug.h"
#include "smf_cache_def.h"
#include "ca/core_ca.h"
#include <nuttx/cache.h>

namespace smf {

	void cache_invalid(void* buff, int size) {
		// CACHE_LINE_CHECK(buff, size, 64)
		// 	L1C_CleanInvalidateCacheRange(0, (size_t)buff, (size_t)buff + size);
		up_invalidate_dcache(buff, buff + size);
	}
	void cache_writeback(void* buff, int size) {
		// CACHE_LINE_CHECK(buff, size, 64)
		// 	L1C_CleanInvalidateCacheRange(1, (size_t)buff, (size_t)buff + size);
		up_clean_dcache(buff, buff+size);
	}
	void cache_flush(void* buff, int size) {
		// CACHE_LINE_CHECK(buff, size, 64)
		// 	L1C_CleanInvalidateCacheRange(2, (size_t)buff, (size_t)buff + size);
		up_clean_dcache(buff, buff+size);
	}
	int cache_line_length() {
		return 64;
	}

}
