#include "VideoFormatList.h"

using namespace smf;
using namespace smf::video;
#define CLIP1(z) (((z) < 0) ? 0 : (((z) > 255) ? 255 : (z)))
static bool convert(VideoFormat* vf, void* idata, void*& odata) {
	int16_t* iptr = (int16_t*)idata;
	uint8_t* optr = (uint8_t*)odata;
	auto n = vf->_ow * vf->_oh * 3 / 2;
	for (int i = 0; i < n; i++) {
		auto temp = *iptr++;
		*optr++ = CLIP1(temp + 128);
	}
	return true;
}

EXTERNC void smf_i420p16s_i420_register() {
	VideoFormats().Add(vfid32("yuv", i420p16s, "yuv", i420), &convert);
}