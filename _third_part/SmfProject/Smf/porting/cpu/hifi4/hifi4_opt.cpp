#include "SmfPorting.h"
#include <xtensa/hal.h>
#include <xtensa/xtruntime.h>
#include <hifi2/basic_op_xtensa.h>
#include <hifi2/oper_32b_xtensa.h>
namespace smf {
	__attribute__((optimize("-O0")))
		void* memcpy32(void* dst0, void* src0, int size) {
		uint32_t* __restrict dst = (uint32_t*)dst0;
		uint32_t* __restrict src = (uint32_t*)src0;
		int len = (size + 3) >> 2;
		auto end = src + len;
		while (src < end)
			*dst++ = *src++;
		return dst0;
	}

	__attribute__((optimize("-O3")))
		void* memcpy64(void* dst0, void* src0, int size) {
		ae_int64* __restrict dst = (ae_int64*)dst0;
		ae_int64* __restrict src = (ae_int64*)src0;
		int len = (size + 7) >> 3;
		auto end = src + len;
		while (src < end)
			*dst++ = *src++;
		return dst0;
	}

	__attribute__((optimize("-O3")))
		void* memset64(void* dst0, int ch, int size) {
		ch |= ch << 8;
		ae_int16x4 v = AE_MOVDA16(ch);
		ae_int16x4* __restrict dst = (ae_int16x4*)dst0;
		int len = (size + 7) >> 3;
		auto end = dst + len;
		while (dst < end)
			*dst++ = v;
		return dst0;
	}

	__attribute__((optimize("-O3")))
		void* memset64_0(void* dst0, int size) {
		ae_int64* __restrict dst = (ae_int64*)dst0;
		int len = (size + 7) >> 3;
		auto end = dst + len;
		auto v = AE_ZERO64();
		while (dst < end)
			*dst++ = v;
		return dst0;
	}
#if 0
	__attribute__((optimize("-O3")))
		int memcpy64(uint8_t* dst0, uint8_t* src0, uint32_t size) {
		ae_int64* __restrict dst = (ae_int64*)dst0;
		ae_int64* __restrict src = (ae_int64*)src0;
		int len = (size + 7) >> 3;
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
	__attribute__((optimize("-O3")))
		void memcpyByCache(void* dst, void* src, int size) {
		int prefetch_state = xthal_set_cache_prefetch(XTHAL_PREFETCH_BLOCKS(8) |
			XTHAL_DCACHE_PREFETCH_HIGH |
			XTHAL_ICACHE_PREFETCH_HIGH |
			XTHAL_DCACHE_PREFETCH_L1);
		//
	//	printf("PREFETCH default : %08x\n",prefetch_state);
	//	printf("PREFETCH new : %08x\n",xthal_get_cache_prefetch());
		//
		xthal_dcache_block_prefetch_modify_grp((void*)dst, size);
		xthal_dcache_block_prefetch_for_read((void*)src, size);
		memcpy64(dst, src, size);
		xthal_set_cache_prefetch(prefetch_state);
		//xthal_dcache_all_writeback_inv();
	}
	__attribute__((optimize("-O3")))
		void memcpyByCache_2(void* dst, void* src, int size) {
		auto dstPtr = (uint8_t*)dst;
		auto srcPtr = (uint8_t*)src;
		while (size != 0) {
			if (size > 32 * 1024) {
				memcpyByCache(dstPtr, srcPtr, 32 * 1024);
				size -= 32 * 1024;
				dstPtr += 32 * 1024;
				srcPtr += 32 * 1024;

			}
			else {
				memcpyByCache(dstPtr, srcPtr, size);
				xthal_dcache_region_writeback_inv(dstPtr, 32 * 1024);
				return;
			}
		}
	}
}