#include "VideoFormat.h"
#include "VideoFormatList.h"
#include "MediaVideo.h"
#include "SmfHash.h"
#include "SmfDef.h"

using namespace smf;
using namespace smf::video;

VideoFormat::~VideoFormat() {
	if (_priv) {
		Free(_priv);
	}
}

bool VideoFormat::Reset() {
	_func = 0;
	return true;
}

bool VideoFormat::Reset(
	uint64_t icodec, uint8_t ifmt, uint16_t iw, uint16_t ih, uint16_t ix, uint16_t iy, uint16_t ixsize, uint16_t iysize
	, uint64_t ocodec, uint8_t ofmt, uint16_t ow, uint16_t oh, uint16_t ox, uint16_t oy, uint16_t oxsize, uint16_t oysize
	, bool scale, bool crop
) {
	_iw = iw;
	_ih = ih;
	_ix = ix;
	_iy = iy;
	_ixsize = ixsize;
	_iysize = iysize;
	_icodec = icodec;
	_ifmt = ifmt;
	
	_ow = ow;
	_oh = oh;
	_ox = ox;
	_oy = oy;
	_oxsize = oxsize;
	_oysize = oysize;
	_ocodec = ocodec;
	_ofmt = ofmt;

	_scale = scale;
	_crop = crop;

	_isize = MediaVideoRaw::FrameSize(_iw, _ih, _ifmt);
	_osize = MediaVideoRaw::FrameSize(_ow, _oh, _ofmt);
	returnIfErrC(false, !_osize);
	if (_scale) {
		adaptiveDispResolution();
	}

	dbgTestPXL("(%s%u:%u*%u)->(%s%u:%u*%u),%u,%u", (char*)&icodec, ifmt, iw, ih, (char*)&ocodec, ofmt, ow, oh, scale, crop);
	_func = VideoFormats().Find(this);
	returnIfErrC(false, !_func);
	return true;
}
bool VideoFormat::Format(void* idata, void*& odata) {
	returnIfErrC(false, !_func);
	return _func(this, idata, odata);
}

bool VideoFormat::adaptiveDispResolution() {
	returnIfErrC(false, !_iw);
	returnIfErrC(false, !_ih);
	returnIfErrC(false, !_ow);
	returnIfErrC(false, !_oh);
	returnIf(false, _ix && _iy);
	double rw = (double)_ow / (double)_iw;
	double rh = (double)_oh / (double)_ih;
	double min_r = 0;
	if (rw < rh) {
		_ix = _ow;
		_iy = (int)((double)_ih * rw);
		min_r = rw;
	} else {
		_iy = _oh;
		_ix = (int)((double)_iw * rh);
		min_r = rh;
	}
	dbgTestPXL("Disp w = %d, h = %d, r=%.2f", _ix, _iy, min_r);

	return true;
}