#include "VideoFormatList.h"

using namespace smf;
using namespace smf::video;

#define CLIP1(z) (((z) < 0) ? 0 : (((z) > 255) ? 255 : (z)))
static bool convert(VideoFormat* vf, void* idata, void*& odata) {
	uint8_t* optr = (uint8_t*)odata;
	uint8_t* _yp = (uint8_t*)idata;
	auto _w = vf->_iw;
	auto _h = vf->_ih;
	uint8_t* _up = _yp + _w * _h;
	uint8_t* _vp = _up + _w * _h;
	for (int i = 0; i < _w * _h; i++)
	{
		int16_t tmp1 = (int16_t)(1.772 * (*_up - 128));
		int16_t tmp2 = (int16_t)(0.34414 * (*_up - 128) + 0.71414 * (*_vp - 128));
		int16_t tmp3 = (int16_t)(1.402 * (*_vp - 128));

		*optr = CLIP1(*_yp + tmp1);
		*optr = CLIP1(*_yp - tmp2);
		*optr = CLIP1(*_yp + tmp3);
		*optr = 0;
		*optr += 4;
	}
	//ifrm->size -= _w * _h * 3;
	//ofrm->size = _w * _h * 4;
	return true;
}

EXTERNC void smf_i444_rgba8888_register() {
	VideoFormats().Add(vfid32("yuv", i444, "rgb", rgba8888), &convert);
}
