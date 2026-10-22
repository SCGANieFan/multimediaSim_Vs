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
	for (int i = 0; i < n / 16; i++) {
		AE_L16X4_IP(s1, src1, 16);
		AE_L16X4_IP(s2, src2, 16);
		auto x3 = AE_SLAA16S(s1, 8);
		auto x4 = AE_SLAA16S(s2, 8);
		auto x5 = AE_ADD16(x3, xx);
		auto x6 = AE_ADD16(x4, xx);
		*dst++ = x5;
		*dst++ = x6;
	}

	return true;
}

EXTERNC void smf_i420p16s_i420p16_register() {
	VideoFormats().Add(vfid32("yuv", i420p16s, "yuv", i420p16), &convert);
}
