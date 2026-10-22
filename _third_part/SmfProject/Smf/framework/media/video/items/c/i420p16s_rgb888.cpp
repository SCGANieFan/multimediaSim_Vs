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
	uint8_t* _vp = _up + _w * _h / 4;
	for (int i = 0; i < _h / 2; i++)
	{
		for (int j = 0; j < _w / 2; j++)
		{
			int16_t tmp1 = (int16_t)(1.772 * (*_up - 128));
			int16_t tmp2 = (int16_t)(0.34414 * (*_up - 128) + 0.71414 * (*_vp - 128));
			int16_t tmp3 = (int16_t)(1.402 * (*_vp - 128));

			*optr = CLIP1(*_yp + tmp1);
			*(optr + 1) = CLIP1(*_yp - tmp2);
			*(optr + 2) = CLIP1(*_yp + tmp3);

			*(optr + 3) = CLIP1(*(_yp + 1) + tmp1);
			*(optr + 4) = CLIP1(*(_yp + 1) - tmp2);
			*(optr + 5) = CLIP1(*(_yp + 1) + tmp3);

			*(optr + 3 * _w) = CLIP1(*(_yp + _w) + tmp1);
			*(optr + 3 * _w + 1) = CLIP1(*(_yp + _w) - tmp2);
			*(optr + 3 * _w + 2) = CLIP1(*(_yp + _w) + tmp3);

			*(optr + 3 * _w + 3) = CLIP1(*(_yp + _w + 1) + tmp1);
			*(optr + 3 * _w + 4) = CLIP1(*(_yp + _w + 1) - tmp2);
			*(optr + 3 * _w + 5) = CLIP1(*(_yp + _w + 1) + tmp3);

			_up++;
			_vp++;
			_yp += 2;
			optr += 6;
		}
		_yp += _w;
		optr += 3 * _w;
	}
	//ifrm->size -= _w * _h * 3 / 2;
	//ofrm->size = _w * _h * 3;
	return true;
}

EXTERNC void smf_i420p16s_rgb888_register() {
	VideoFormats().Add(vfid32("yuv", i420p16s, "rgb", rgb888), &convert);
}