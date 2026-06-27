/*
 * hifi4.c
 *
 *  Created on: 2021/10/29
 *      Author: wangjg
 */

#include "SmfPorting.h"
#include "smf_debug.h"

namespace smf {
	void cache_invalid(void* buff, int size) {
	}
	void cache_writeback(void* buff, int size) {
	}
	void cache_flush(void* buff, int size) {
	}
	void icache_invalid(void* buff, int size) {
	}
	int cache_line_length() {
		return 64;
	}
}



