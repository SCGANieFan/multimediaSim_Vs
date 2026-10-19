#include "PcmFormat.h"
#include "PcmFormat.def.h"
#include <stdlib.h>
#include <string.h>
#include "smf_debug.h"
using namespace smf;
using namespace smf::audio;

#if 0
#define K_BITS 15
#define K_MAX (1u<<K_BITS)
#define K_MASK (K_MAX-1)
template<class Ti, class To, class Tx>
static inline bool res_nch(void* ibuf, int& isize, void* obuf, int& osize, PcmFormat& res) {
	auto iptr = (Ti*)ibuf;
	auto optr = (To*)obuf;
	auto ichs = res._i_channels;
	auto ochs = res._o_channels;
	auto irate = res._i_rate;
	auto orate = res._o_rate;
	auto iend = isize / (ichs * sizeof(Ti));
	auto oend = osize / (ochs * sizeof(To));
	auto oidx0 = res._oidx;
	auto iidx0 = res._iidx;
	auto oidx = 0;
	auto iidx = 0;
	auto shf = K_BITS - res._shift;
	uint32_t k = ((uint64_t)irate << K_BITS) / orate;
	auto refs = (T*)res._refs;
	auto ipos = oidx0 * k;
	for (oidx = 0; oidx < oend; oidx++, ipos += k) {
		iidx = ((ipos + K_MASK) >> K_BITS) - iidx0;
		if (iidx >= iend) {
			iidx = iend;
			break;
		}
		int32_t k0 = ipos & K_MASK;
		int32_t k1 = K_MAX - k0;
		auto src2 = iptr + iidx * chs;
		auto src1 = iidx > 0 ? (src2 - chs) : refs;
		if (!k0 || !k1) {
			auto srcx = !k0 ? src1 : src2;
			for (int i = 0; i < chs; i++) {
				Tx v = ((Tx)srcx[i] << K_BITS) >> shf;
				*optr++ = sat_t<sizeof(To)>::sat(v);
			}
		}
		else {
			for (int i = 0; i < chs; i++) {
				Tx v = ((Tx)src1[i] * k1 + (Tx)src2[i] * k0) >> shf;
				*optr++ = sat_t<sizeof(To)>::sat(v);
			}
		}
	}
	//save one sample
	memcpy((void*)refs, iptr + (iidx - 1) * chs, chs * 1 * sizeof(T));
	//save status
	res._oidx = oidx0 + oidx;
	res._iidx = iidx0 + iidx;
	//output size
	isize = iidx * chs * sizeof(T);
	osize = oidx * chs * sizeof(T);
	return true;
}
#endif