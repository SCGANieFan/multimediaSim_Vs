/*
 * hifi4.c
 *
 *  Created on: 2021��1��29��
 *      Author: wangjg
 */
#include <stdint.h>
#include <string.h>

namespace smf {
	//__attribute__((optimize("-O3")))
	void* memcpy32(void* dst0, void* src0, int size) {
		uint32_t* __restrict dst = (uint32_t*)dst0;
		uint32_t* __restrict src = (uint32_t*)src0;
		int len = (size + 3) >> 2;
		auto end = src + len;
		while (src < end)
			*dst++ = *src++;
		return dst0;
	}

	void* memcpy64(void* dst0, void* src0, int size) {
		return memcpy(dst0, src0, size);
	}
	void* memset64(void* dst0, int ch, int size) {
		return memset(dst0, ch, (size_t)size);
	}
	void* memset64_0(void* dst0, int size) {
		return memset(dst0, 0, (size_t)size);
	}
}