#include "VideoFormatList.h"

using namespace smf;
using namespace smf::video;

#define CLIP1(z) (((z) < 0) ? 0 : (((z) > 255) ? 255 : (z)))
static bool convert(VideoFormat* vf, void* ibuff, void*& obuff){
	int w = vf->_iw;
	int h = vf->_ih;
	int height = h;
	int width = w;
	int fsize = height * width;
	short* y = (short*)ibuff;
	short* u = (short*)y + fsize;
	short* v = (short*)y + fsize + (fsize / 4);
	unsigned char* vyuy = (unsigned char*)obuff;
	for (int m = 0; m < height; m++) {
		for (int n = 0; n < width; n += 2) {
			int index = (m / 2) * (width / 2) + (n / 2);
			unsigned char y0 = CLIP1(y[m * width + n] + 128);
			unsigned char y1 = CLIP1(y[m * width + n + 1] + 128);
			unsigned char v0 = CLIP1(v[index] + 128);
			unsigned char u0 = CLIP1(u[index] + 128);
			*vyuy++ = v0;
			*vyuy++ = y0;
			*vyuy++ = u0;
			*vyuy++ = y1;
		}
	}
	return true;
}

EXTERNC void smf_i420p16s_yuyv_register() {
	VideoFormats().Add(vfid32("yuv", i420p16s, "yuv", yuyv), &convert);
}
