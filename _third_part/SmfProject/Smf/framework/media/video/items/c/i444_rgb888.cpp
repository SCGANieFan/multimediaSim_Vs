#include "VideoFormatList.h"

using namespace smf;
using namespace smf::video;

#define CLIP1(z) (((z) < 0) ? 0 : (((z) > 255) ? 255 : (z)))
static bool convert(VideoFormat* vf, void* idata, void*& odata) {
	uint8_t* optr = (uint8_t*)odata;
	auto _w = vf->_iw;
	auto _h = vf->_ih;
	uint8_t* _yp = (uint8_t*)idata;
	uint8_t* _up = _yp + _w * _h;
	uint8_t* _vp = _up + _w * _h;
	for (int i = 0; i < _w * _h; i++)
	{
		uint8_t tmp1 = (uint8_t)(1.772 * (*_up - 128));
		uint8_t tmp2 = (uint8_t)(0.34414 * (*_up - 128) + 0.71414 * (*_vp - 128));
		uint8_t tmp3 = (uint8_t)(1.402 * (*_vp - 128));

		*optr = *_yp + tmp1;
		*optr = *_yp - tmp2;
		*optr = *_yp + tmp3;
		*optr += 3;
	}
	//ifrm->size -= _w * _h * 3;
	//ofrm->size = _w * _h * 3;
	return true;
}

EXTERNC void smf_i444_rgb888_register() {
	VideoFormats().Add(vfid32("yuv", i444, "rgb", rgb888), &convert);
}

