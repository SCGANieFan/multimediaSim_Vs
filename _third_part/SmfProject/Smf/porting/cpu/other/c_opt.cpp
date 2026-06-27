#include "SmfPorting.h"
#include <string.h>
namespace smf {
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
		memcpy(dst0, src0, size);
		return (char*)dst0 + size;
	}

	void* memset64(void* dst0, int ch, int size) {
		memset(dst0, ch, size);
		return (char*)dst0 + size;
	}

	void* memset64_0(void* dst0, int size) {
		memset(dst0, 0, size);
		return (char*)dst0 + size;
	}
#if 0
	__attribute__((optimize("-O3")))
		int memcpy64(uint8_t* dst0, uint8_t* src0, uint32_t size) {
		ae_int64* __restrict dst = (ae_int64*)dst0;
		ae_int64* __restrict src = (ae_int64*)src0;
		int len = size >> 3;
		auto end = src + len;
		while (src < end)
			*dst++ = *src++;
		return len << 3;
	}
#endif
#if 0
	__attribute__((optimize("-O3")))
		void testCacheFill(void* dst, int size) {
		for (int i = 0; i < size; i++) {
			dst[i] = i % 256;
		}
	}
#endif
	void memcpyByCache(void* dst, void* src, int size) {
		memcpy(dst, src, size);
	}
	void memcpyByCache_2(void* dst, void* src, int size) {
		memcpy(dst, src, size);
	}
}