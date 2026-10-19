#include "AudioGain.h"
#include "PcmFormat.def.h"
#include "smf_debug.h"
#include "string.h"
#include "math.h"
using namespace smf;
using namespace smf::audio;
PcmGain::~PcmGain(){
	Free(_facs);
}

bool PcmGain::gain(void* dst0, void* src0, int32_t scf, int32_t end) {
	return _cbGain ? _cbGain(this, dst0, src0, scf, end): false;
}
bool PcmGain::gainAdd(void* dst0, void* src0, int32_t scf, int32_t end) {
	return _cbGainAdd ? _cbGainAdd(this, dst0, src0, scf, end) : false;
}

template<class T,class X,class F,int __bits>
static inline bool pcmGain(PcmGain* pcm, void* dst0, void* src0, int32_t scf, int32_t end) {
	auto* __restrict dst = (T*)dst0;
	auto* __restrict src = (T*)src0;
	auto* __restrict _facs = pcm->_facs;
	auto n = pcm->_cnt;
	//
	//if (scf == 0x10000 && end == 0x10000) {
	if (scf == 0x8000 && end == 0x8000) {//q15
		if (dst != src) {
			//memcpy(dst, src, samps * chn * sizeof(T));
			for (int k = 0; k < n; k++) {
				dst[k] = src[k];
			}
		}
	}
	else if (scf == 0 && end == 0) {
		//memset(dst, 0, samps * chn * sizeof(T));
		for (int k = 0; k < n; k++) {
			dst[k] = 0;
		}
	}
	else if (scf == end) {
		for (int k = 0; k < n; k++) {
			auto v = (X)src[k] * scf >> 15; dst[k] = sat_t<__bits>::sat(v);
		}
	}
	else {
		auto w = end - scf;
		for (int k = 0; k < n; k++) {
			auto fac = scf + ((F)w * _facs[k] >> 15);
			auto v = (X)src[k] * fac >> 15; dst[k] = sat_t<__bits>::sat(v);
		}
	}
	//
	return true;
}

template<class T, class X, class F, int __bits>
static inline bool pcmGainAdd(PcmGain* pcm, void* dst0, void* src0, int32_t scf, int32_t end) {
	auto* __restrict dst = (T*)dst0;
	auto* __restrict src = (T*)src0;
	auto* __restrict _facs = pcm->_facs;
	auto n = pcm->_cnt;
	//
	//if (scf == 0x10000 && end == 0x10000) {
	if (scf == 0x8000 && end == 0x8000) {//q15
		if (dst != src) {
			for (int k = 0; k < n; k++) {
				auto v = dst[k] + src[k]; dst[k] = sat_t<__bits>::sat(v);
			}
		}
	}
	else if (scf == 0 && end == 0) {

	}
	else if (scf == end) {
		for (int k = 0; k < n; k++) {
			auto v = dst[k] + ((X)src[k] * scf >> 15); dst[k] = sat_t<__bits>::sat(v);
		}
	}
	else{
		auto w = end - scf;
		for (int k = 0; k < n; k++) {
			auto fac = scf + ((F)w * _facs[k] >> 15);
			auto v = dst[k] + ((X)src[k] * fac >> 15); dst[k] = sat_t<__bits>::sat(v);
		}
	}
	return true;
}

#define PI 3.14159265358979323846f
bool PcmGain::init(int fsize, int8_t chn, int8_t bits, int8_t w, bool superGain) {
	returnIfErrC(false, _facs);
	returnIfErrC(false, !fsize);
	returnIfErrC(false, !chn);
	returnIfErrC(false, !bits);
	returnIfErrC(false, !w);
	auto cnt = fsize / w;
	auto facs = (uint16_t*)Alloc(cnt * w);
	returnIfErrC(false, !facs);
	auto n = cnt / chn;
	auto xstep = PI / (n - 1);
	auto x = xstep / 2;
	auto facs0 = &facs[cnt >> 1];
	auto facs1 = facs0 - 1;
	for (int i = 0; i < (n >> 1); i++) {
		auto v = (uint16_t)(sin(x) * 16384);
		auto v0 = 16384 + v;
		auto v1 = 16384 - v;
		for (int j = 0; j < chn; j++) {
			*facs0++ = v0;
			*facs1-- = v1;
		}
		x += xstep;
	}
	//
	facs0 -= chn;
	facs1 += chn;
	for (int j = 0; j < chn; j++) {
		*facs0++ = 32768;
		*facs1-- = 0;
	}
	//for (int i = 0; i < cnt; i++) {dbgTestPXL("[%d]%u", i, facs[i]);}
	//dbgTestDump(facs, cnt * w);
	_cnt = cnt;
	_facs = facs;
	if (superGain) {
		switch (bits) {
		//case  8: _cbGain = pcmGain<int08_t, int32_t, int64_t,  8>; _cbGainAdd = pcmGainAdd<int08_t, int32_t, int64_t,  8>; break;
		case 16: _cbGain = pcmGain<int16_t, int32_t, int64_t, 16>; _cbGainAdd = pcmGainAdd<int16_t, int32_t, int64_t, 16>; break;
		case 24: _cbGain = pcmGain<int32_t, int64_t, int64_t, 24>; _cbGainAdd = pcmGainAdd<int32_t, int64_t, int64_t, 24>; break;
		case 32: _cbGain = pcmGain<int32_t, int64_t, int64_t, 32>; _cbGainAdd = pcmGainAdd<int32_t, int64_t, int64_t, 32>; break;
		default:return false;
		}
	}
	else {
		switch (bits) {
		//case  8: _cbGain = pcmGain<int08_t, int32_t, int32_t, 0>; _cbGainAdd = pcmGainAdd<int08_t, int32_t, int32_t, 8>; break;
		case 16: _cbGain = pcmGain<int16_t, int32_t, int32_t, 0>; _cbGainAdd = pcmGainAdd<int16_t, int32_t, int32_t, 16>; break;
		case 24: _cbGain = pcmGain<int32_t, int64_t, int32_t, 0>; _cbGainAdd = pcmGainAdd<int32_t, int64_t, int32_t, 24>; break;
		case 32: _cbGain = pcmGain<int32_t, int64_t, int32_t, 0>; _cbGainAdd = pcmGainAdd<int32_t, int64_t, int32_t, 32>; break;
		default:return false;
		}
	}
	return true;
}

void PcmGain::uninit() {
	Free(_facs);
	_facs = 0;
}
