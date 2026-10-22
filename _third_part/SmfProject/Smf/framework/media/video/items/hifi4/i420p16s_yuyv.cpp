#include "VideoFormatList.h"
#include <xtensa/tie/xt_hifi2.h>
using namespace smf;
using namespace smf::video;

static bool convert(VideoFormat* vf, void* idata, void*& odata) {
	unsigned height = vf->_ih;
	unsigned width = vf->_iw;
    unsigned ysize = height * width;
    unsigned uvsize = ysize >> 2;
    unsigned width_4 = width >> 2;

    int16_t *idata0 = (int16_t *)idata;
    ae_int16x4 *yp0 = (ae_int16x4 *)idata0;
    ae_int16x4 *yp1 = yp0 + width_4;

    int16_t *up = idata0 + ysize;
    int16_t *vp = up + uvsize;

    ae_int16x4 *o0 = (ae_int16x4 *)odata; 
    ae_int16x4 *o1 = o0 + width_4;
    int yi = 0;
    int yuyvi = 0;
    int uvi = 0;
    ae_int16x4 y0, y1, u0, v0;
    ae_int16x4 xx = 0x8000;
    for (int i = 0; i < height; i += 2) {
        for (int j = 0; j < width; j += 4) {
            AE_L16X4_IP(y0, yp0, 8);
            AE_L16X4_IP(y1, yp1, 8);

            int16_t u0 = up[uvi];
            int16_t u1 = up[uvi + 1];
            int16_t v0 = vp[uvi];
            int16_t v1 = vp[uvi + 1];

            ae_int16x4 uv = {u0, v0, u1, v1};

            ae_int16x4 x0 = AE_SLAA16S(y0, 8);
            ae_int16x4 x1 = AE_SLAA16S(y1, 8);
            ae_int16x4 x2 = AE_SLAA16S(uv, 8);
            ae_int16x4 x3 = AE_ADD16(x0, xx);
            ae_int16x4 x4 = AE_ADD16(x1, xx);
            ae_int16x4 x5 = AE_ADD16(x2, xx);

            ae_int64 t00 = AE_MOVINT64_FROMF16X4(x3);
            ae_int64 t01 = AE_MOVINT64_FROMF16X4(x5);
            ae_int64 t10 = AE_MOVINT64_FROMF16X4(x4);
            ae_int64 t11 = AE_MOVINT64_FROMF16X4(x5);
            ae_int64 t02 = AE_SRLI64(t00, 8);
            ae_int64 t12 = AE_SRLI64(t10, 8);
            ae_int64 t03 = t01 | t02;
            ae_int64 t13 = t11 | t12;
            ae_int16x4 r0 = AE_MOVINT16X4_FROMF64(t03);
            ae_int16x4 r1 = AE_MOVINT16X4_FROMF64(t13);
            AE_S16X4_IP(r0, o0, 8);
            AE_S16X4_IP(r1, o1, 8);

            uvi += 2;
        }
        yp0 += width_4;
        yp1 += width_4;
        o0 += width_4;
        o1 += width_4;
    }
    return true;
}


EXTERNC void smf_i420p16s_yuyv_register() {
	VideoFormats().Add(vfid32("yuv", i420p16s, "yuv", yuyv), &convert);
}
