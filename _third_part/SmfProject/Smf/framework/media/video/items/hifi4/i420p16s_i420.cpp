#include "VideoFormatList.h"
#include <xtensa/tie/xt_hifi2.h>
using namespace smf;
using namespace smf::video;

static bool convert(VideoFormat* vf,void* ibuff, void*& obuff) {
	int n = vf->_osize;
	ae_int16x4* __restrict src1 = (ae_int16x4*)ibuff;
	ae_int16x4* __restrict src2 = (ae_int16x4*)ibuff + 1;
	ae_int16x4* __restrict dst = (ae_int16x4*)obuff;
	ae_int16x4 s1, s2;
	ae_int16x4 xx = 0x8000;
	ae_int64 mask = 0x00ff00ff00ff00ff;
	for (int i = 0; i < n / 8; i++) {
		AE_L16X4_IP(s1, src1, 16);
		AE_L16X4_IP(s2, src2, 16);
		auto x3 = AE_SLAA16S(s1, 8);
		auto x4 = AE_SLAA16S(s2, 8);
		auto x5 = AE_ADD16(x3, xx);
		auto x6 = AE_ADD16(x4, xx);
		auto x1 = AE_SEL16I(x5, x6, 7);
		auto x2 = AE_SEL16I(x5, x6, 8);
		auto x7 = AE_MOVINT64_FROMINT16X4(x1);
		auto x8 = AE_MOVINT64_FROMINT16X4(x2);
		auto x9 = AE_SRLI64(x7, 8);
		x9 = x9 & mask;
		auto x10 = x8 | x9;
		auto x11 = AE_MOVINT16X4_FROMINT64(x10);
		*dst++ = x11;
	}
	return true;
}

EXTERNC void smf_i420p16s_i420_register() {
	VideoFormats().Add(vfid32("yuv", i420p16s, "yuv", i420), &convert);
}
