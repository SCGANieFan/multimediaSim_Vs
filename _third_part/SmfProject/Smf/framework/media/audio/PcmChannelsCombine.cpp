#include "PcmChannelsCombine.h"
#include "PcmFormat.def.h"
#include "smf_debug.h"
using namespace smf;
using namespace smf::audio;

bool PcmChannelsCombine::Open(uint8_t* chns, uint8_t n, uint8_t width) {
	returnIfErrC(false, Valid());
	returnIfErrC(false, !width);
	int ochn = 0;
	for (int i = 0; i < n; i++) {
		auto chn = chns[i];
		_ichs[i] = chn;
		_issizes[i] = width * chn;
		ochn += chn;
	}
	_ochn = ochn;
	_ossize = width * ochn;
	_n = n;
	_cbCombine = _cbCombines[width  - 1];
	return true;
}
bool PcmChannelsCombine::Close() {
	_cbCombine = 0;
	return true;
}

bool PcmChannelsCombine::Combine(void** idatas, void* odata, int samples) {
	returnIfErrC(false, !_cbCombine);
	_cbCombine(idatas, _ichs, _n, odata, samples);
	return true;
}

template<class T>static inline 
void combine(void** ipcms, uint8_t* icnts, uint8_t n, void* opcm, int samples) {
	auto idatas = (T**)ipcms;
	auto odata = (T*)opcm;
	for (int i = 0; i < samples; i++) {
		for (int j = 0; j < n; j++) {
			auto& idata = idatas[j];
			for (int k = 0; k < icnts[j]; k++) {
				*odata++ = *idata++;
			}
		}
	}
}

const PcmChannelsCombine::CbCombine PcmChannelsCombine::_cbCombines[4] = {
	combine<int08_t>,
	combine<int16_t>,
	combine<int24_t>,
	combine<int32_t>
};

bool PcmChannelsCombine::Open(IMedia** imedias, uint8_t n, IMedia* omedia) {
	returnIfErrC(false, Valid());
	auto opcm = (MediaPcm*)omedia;
	auto width = 0;
	auto rate  = 0;
	for (int i = 0; i < n; i++) {
		auto pcm = (MediaPcm*)imedias[i];
		if (width) {
			returnIfErrC(false, width != pcm->_sampleWidth);
		}
		else {
			width = pcm->_sampleWidth;
		}
		if (!rate) rate = pcm->_rate;
		else returnIfErrC(false, rate != pcm->_rate);
		auto chn = pcm->_channels;
		_ichs[i] = chn;
	}
	returnIfErrC(false, !Open(_ichs, n, width));
	if(!opcm->_rate) opcm->_rate = rate;
	if(!opcm->_channels) opcm->_channels = _ochn;
	if(!opcm->_sampleWidth) opcm->_sampleWidth = _width;
	dbgTestPXL("%d/%d/%d",opcm->_rate,opcm->_channels,opcm->_sampleWidth);
	return true;
}

bool PcmChannelsCombine::Combine(Frame** ifrms, Frame* ofrm) {
	returnIfErrC(false, !_cbCombine);
	void* idatas[8];
	void* odata = (void*)ofrm->LeftData();
	int samples = ofrm->Left() / _ossize;
	for (int i = 0; i < _n; i++) {
		idatas[i] = ifrms[i]->Data();
		auto samp = ifrms[i]->size / _issizes[i];
		if (samples > samp)samples = samp;
	}
	//
	_cbCombine(idatas, _ichs, _n, odata, samples);
	//
	for (int i = 0; i < _n; i++) {
		auto isize = _issizes[i] * samples;
		ifrms[i]->size -= isize;
		ifrms[i]->offset += isize;
	}
	int osize = _ossize * samples;
	ofrm->size += osize;
	//ofrm->offset += osize;
	return true;
}