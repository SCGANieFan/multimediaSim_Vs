#include "VideoFormatList.h"
#include <xtensa/tie/xt_hifi2.h>
using namespace smf;
using namespace smf::video;

static bool convert(VideoFormat* vf, void* idata, void*& odata) {
	unsigned height = vf->_ih;
	unsigned width = vf->_iw;
    unsigned tsize = height * width * 3 / 2;

    ae_int16x4 *ibuff = (ae_int16x4 *)idata;
    ae_int16x4 *obuff = (ae_int16x4 *)odata;

    ae_int16x4 t0, t1;
    for(int i = 0; i < tsize; i += 8) {
        AE_L16X4_IP(t0, ibuff, 8);
        AE_L16X4_IP(t1, ibuff, 8);

        ae_int16x4 x0 = AE_SEL16_7531(t0, t1);
        ae_int16x4 x1 = AE_SEL16_6420(t0, t1);
        ae_int64 x2 = AE_MOVINT64_FROMF16X4(x0);
        ae_int64 x3 = AE_MOVINT64_FROMF16X4(x1);
        ae_int64 x4 = AE_SLLI64(x3, 8);
        ae_int64 x5 = x2 | x4;
        ae_int16x4 x6 = AE_MOVINT16X4_FROMINT64(x5);
        *obuff++ = x6;
    }

	return true;
}

EXTERNC void smf_i420p16_i420_register() {
	VideoFormats().Add(vfid32("yuv", i420p16, "yuv", i420), &convert);
}
