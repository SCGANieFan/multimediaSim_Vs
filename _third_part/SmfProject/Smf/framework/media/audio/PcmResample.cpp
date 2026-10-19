#include "PcmResample.h"
#include "PcmFormat.def.h"
#include "smf_debug.h"

using namespace smf::audio;
template<class T, class Tx, int BITS>
static inline bool res_proc_0(void* ibuf, int& isize, void* obuf, int& osize, PcmResample& res);
template<class T, class Tx, int BITS>
static inline bool res_proc_1(void* ibuf, int& isize, void* obuf, int& osize, PcmResample& res);
template<class T, class Tx, int BITS>
static inline bool res_proc_2(void* ibuf, int& isize, void* obuf, int& osize, PcmResample& res);

//static inline bool res_prof(void* ibuf, int& isize, void* obuf, int& osize, PcmResample& res);

//PcmResample::PcmResample() {
//
//}
//PcmResample::~PcmResample() {
//
//}
bool PcmResample::Valid()const {
	return _cbResample;
}
PcmResample::operator bool()const {
	return Valid();
}
bool PcmResample::Reset() {
	_cbResample = 0;
	return true;
}
bool PcmResample::Reset(uint32_t rate, uint32_t target_rate, uint8_t channels, uint8_t sample_bits, uint8_t sample_width, int8_t left_shift, bool is_float){
	target_rate = (target_rate) ? target_rate : rate;
	if ((sample_width << 3) < sample_bits)sample_width = (sample_bits + 7) >> 3;
	_rate = rate;
	_bits = sample_bits;
	_width = sample_width;
	_channels = channels;
	_target_rate = target_rate;
	_shift = left_shift;
	returnIfErrC(false, !target_rate);
	int ibits_idx = sample_width - 1;
	returnIfErrCS(false, (ibits_idx < 0) || (ibits_idx > 3), "unsupport sampleWidth:%d/%d", sample_width, sample_bits);
	returnIfErrC(false, (channels <=0)||(channels > 8));
	_cbResample = 0;
	if (!(_rate % _target_rate)) {
		dbgTestPXL("[1]%u->%u,%u", _rate,_target_rate, is_float);
		if (is_float) {
			_cbResample = &res_proc_1<float,float,0>;
		}
		else {
			switch (_width) {
			case 1:_cbResample = &res_proc_1<int08_t, int64_t, 8>; break;
			case 2:_cbResample = &res_proc_1<int16_t, int64_t, 16>; break;
			case 3:_cbResample = &res_proc_1<int24_t, int64_t, 24>; break;
			case 4:	 if (_bits == 32)_cbResample = &res_proc_1<int32_t, int64_t, 32>;
				  else if (_bits == 24)_cbResample = &res_proc_1<int32_t, int64_t, 24>;
				  else return false;
				break;
			default: return false;
			}
		}
	}
	else if (!(_target_rate % _rate)) {
		dbgTestPXL("[2]%u->%u,%u", _rate, _target_rate, is_float);
		if (is_float) {
			_cbResample = &res_proc_2<float, float, 0>;
		}
		else {
			switch (_width) {
			case 1:_cbResample = &res_proc_2<int08_t, int64_t, 8>; break;
			case 2:_cbResample = &res_proc_2<int16_t, int64_t, 16>; break;
			case 3:_cbResample = &res_proc_2<int24_t, int64_t, 24>; break;
			case 4:	 if (_bits == 32)_cbResample = &res_proc_2<int32_t, int64_t, 32>;
				  else if (_bits == 24)_cbResample = &res_proc_2<int32_t, int64_t, 24>;
				  else return false;
				break;
			default: return false;
			}
		}
	}
	else {
		dbgTestPXL("[0]%u->%u,%u", _rate, _target_rate, is_float);
		if (is_float) {
			_cbResample = &res_proc_0<float, float, 0>;
		}
		else {
			switch (_width) {
			case 1:_cbResample = &res_proc_0<int08_t, int64_t, 8>; break;
			case 2:_cbResample = &res_proc_0<int16_t, int64_t, 16>; break;
			case 3:_cbResample = &res_proc_0<int24_t, int64_t, 24>; break;
			case 4:	 if (_bits == 32)_cbResample = &res_proc_0<int32_t, int64_t, 32>;
				  else if (_bits == 24)_cbResample = &res_proc_0<int32_t, int64_t, 24>;
				  else return false;
				break;
			default: return false;
			}
		}
	}
	memset(_refs, 0, sizeof(_refs));
	_iidx = 0;
	_oidx = 0;
	return true;
}

bool PcmResample::Run(void* ibuf, int& isize, void* obuf, int& osize) {
	returnIfErrC(false, !_cbResample);
	return _cbResample(ibuf, isize, obuf, osize, *this);
}

#if 1
template<class T,class Tx,int BITS>
static inline bool res_proc_0(void* ibuf, int& isize, void* obuf, int& osize, PcmResample&res) {
	auto iptr = (T*)ibuf;
	auto optr = (T*)obuf;
	auto chs = res._channels;
	auto irate = res._rate;
	auto orate = res._target_rate;
	auto ssize = chs * sizeof(T);
	auto iidx0 = res._iidx;
	auto oidx0 = res._oidx;
	auto iend = iidx0 + isize / ssize;
	auto oend = oidx0 + osize / ssize;
	auto refs = (T*)res._refs;
	auto oidx = oidx0;
	auto opos = (uint64_t)oidx0 * irate;
	auto oposend = (uint64_t)iend * orate;
	while(oidx < oend && opos < oposend) {
		int iidx = opos / orate;
		int k0 = opos % orate;
		int ii = iidx - iidx0;
		int k1 = orate - k0;
		auto src2 = iptr + ii * chs;
		auto src1 = (ii > 0) ? (src2 - chs) : refs;
		for (int i = 0; i < chs; i++) {
			*optr++ = ((Tx)src1[i] * k1 + (Tx)src2[i] * k0) / orate;
		}
		//
		oidx++;
		opos += irate;
	}
	//reset iidx
	auto iidx = opos / orate;
	//
	if (oidx > oend)oidx = oend;
	if (iidx > iend)iidx = iend;
	//save one sample
	if (iidx > iidx0) {
		memcpy((void*)refs, iptr + (iidx - iidx0 - 1) * chs, ssize);
	}
	//output size
	isize = (iidx - iidx0) * ssize;
	osize = (oidx - oidx0) * ssize;
	//save status
	if (oidx >= orate && iidx >= irate) {
		iidx -= irate;
		oidx -= orate;
	}
	res._oidx = oidx;
	res._iidx = iidx;
	return true;
}
template<class T, class Tx, int BITS>
static inline bool res_proc_1(void* ibuf, int& isize, void* obuf, int& osize, PcmResample& res) {
	auto iptr = (T*)ibuf;
	auto iptr0 = (T*)ibuf;
	auto optr = (T*)obuf;
	auto chs = res._channels;
	auto irate = res._rate;
	auto orate = res._target_rate;
	auto ssize = chs * sizeof(T);
	auto iidx0 = res._iidx;
	auto oidx0 = res._oidx;
	int8_t n = irate / orate;
	auto iend = iidx0 + isize / ssize;
	auto oend = oidx0 + osize / ssize;
	auto refs = (T*)res._refs;
	auto iidx = iidx0;
	auto oidx = oidx0;
	while (oidx < oend && iidx + n <= iend) {
		auto src2 = iptr;
		auto src1 = (iidx > iidx0) ? (src2 - chs) : refs;
		for (int i = 0; i < chs; i++) {
			optr[i] = src1[i];
		}
		//
		iptr += chs * n;
		optr += chs;
		iidx += n;
		oidx++;
	}
	iptr = iptr0;
	//
	if (oidx > oend)oidx = oend;
	if (iidx > iend)iidx = iend;
	//save one sample
	memcpy((void*)refs, iptr + (iidx - iidx0 - 1) * chs, ssize);
	//output size
	isize = (iidx - iidx0) * ssize;
	osize = (oidx - oidx0) * ssize;
	//save status
	if (oidx >= orate && iidx >= irate) {
		iidx -= irate;
		oidx -= orate;
	}
	res._oidx = oidx;
	res._iidx = iidx;
	return true;
}
template<class T, class Tx, int BITS>
static inline bool res_proc_2(void* ibuf, int& isize, void* obuf, int& osize, PcmResample& res) {
	auto iptr = (T*)ibuf;
	auto iptr0 = (T*)ibuf;
	auto optr = (T*)obuf;
	auto chs = res._channels;
	auto irate = res._rate;
	auto orate = res._target_rate;
	auto ssize = chs * sizeof(T);
	auto iidx0 = res._iidx;
	auto oidx0 = res._oidx;
	int8_t n = orate / irate;
	auto iend = iidx0 + isize / ssize;
	auto oend = oidx0 + osize / ssize;
	auto refs = (T*)res._refs;
	auto iidx = iidx0;
	auto oidx = oidx0;
	while (oidx + n <= oend && iidx < iend) {
		for (int i = 0; i < chs; i++) {
			auto src2 = iptr;
			auto src1 = (iidx > iidx0) ? (src2 - chs) : refs;
			auto k = (src2[i] - src1[i] + (n >> 1)) / n;
			auto s = src1[i];
			auto dst = optr + i;
			for (int x = 0; x < n; x++) {
				*dst = s;
				s += k;
				dst += chs;
			}
		}
		iptr += chs;
		optr += chs * n;
		iidx++;
		oidx += n;
	}
	iptr = iptr0;
	//
	if (oidx > oend)oidx = oend;
	if (iidx > iend)iidx = iend;
	//save one sample
	memcpy((void*)refs, iptr + (iidx - iidx0 - 1) * chs, ssize);
	//output size
	isize = (iidx - iidx0) * ssize;
	osize = (oidx - oidx0) * ssize;
	//save status
	if (oidx >= orate && iidx >= irate) {
		iidx -= irate;
		oidx -= orate;
	}
	res._oidx = oidx;
	res._iidx = iidx;
	return true;
}
#endif

#if 0
#define K_BITS 14
#define K_MAX (1u<<K_BITS)
#define K_MASK (K_MAX-1)
template<class T,class Tx,int BITS>
static inline bool res_proc(void* ibuf, int& isize, void* obuf, int& osize, PcmResample& res) {
	auto iptr = (T*)ibuf;
	auto optr = (T*)obuf;
	auto chs = res._channels;
	auto ssize = chs * sizeof(T);
	auto irate = res._rate;
	auto orate = res._target_rate;
	auto iend = isize / ssize;
	auto oend = osize / ssize;
	auto oidx0 = res._oidx;
	auto iidx0 = res._iidx;
	auto refs = (T*)res._refs;
	auto shf = K_BITS - res._shift;
	auto k = (uint32_t)(((uint64_t)irate << K_BITS) / orate);
	auto ipos = oidx0 * k;
	uint32_t oidx = 0;
	uint32_t iidx = 0;
	for (oidx = 0; oidx < oend; oidx++, ipos += k) {
		iidx = (ipos >> K_BITS) - iidx0;
		if (iidx >= iend) {
			iidx = iend;
			break;
		}
		int32_t k0 = ipos & K_MASK;
		int32_t k1 = K_MAX - k0;
		auto src2 = iptr + iidx * chs;
		auto src1 = iidx > 0 ? (src2 - chs) : refs;
		for (int i = 0; i < chs; i++) {
			Tx v = ((Tx)src1[i] * k1 + (Tx)src2[i] * k0) >> shf;
			*optr++ = sat_t<BITS>::sat(v);
		}
	}
	//save one sample
	memcpy((void*)refs, iptr + (iidx - 1) * chs, chs * sizeof(T) * 1);
	
	//save status
	res._oidx = oidx0 + oidx;
	res._iidx = iidx0 + oidx * irate / orate;
	if (res._oidx >= orate && res._iidx >= irate) {
		res._iidx -= irate;
		res._oidx -= orate;
	}
	
	//output size
	isize = iidx * ssize;
	osize = oidx * ssize;
	return true;

}

static inline bool res_prof(void* ibuf, int& isize, void* obuf, int& osize, PcmResample& res) {
	auto iptr = (float*)ibuf;
	auto optr = (float*)obuf;
	auto chs = res._channels;
	auto ssize = chs * sizeof(float);
	auto irate = res._rate;
	auto orate = res._target_rate;
	auto iend = isize / ssize;
	auto oend = osize / ssize;
	auto oidx0 = res._oidx;
	auto iidx0 = res._iidx;
	auto refs = (float*)res._refs;
	//auto shf = K_BITS - res._shift;
	auto k = (uint32_t)(((uint64_t)irate << K_BITS) / orate);
	auto ipos = oidx0 * k;
	uint32_t oidx = 0;
	uint32_t iidx = 0;
	for (oidx = 0; oidx < oend; oidx++, ipos += k) {
		iidx = (ipos >> K_BITS) - iidx0;
		if (iidx >= iend) {
			iidx = iend;
			break;
		}
		auto k0 = (ipos & K_MASK) * (1.0f / K_MAX);
		auto k1 = 1.0f - k0;
		auto src2 = iptr + iidx * chs;
		auto src1 = iidx > 0 ? (src2 - chs) : refs;
		for (int i = 0; i < chs; i++) {
			*optr++ = src1[i] * k1 + src2[i] * k0;
		}
	}
	//save one sample
	memcpy((void*)refs, iptr + (iidx - 1) * chs, chs * sizeof(float) * 1);

	//save status
	res._oidx = oidx0 + oidx;
	res._iidx = iidx0 + oidx * irate / orate;
	if (res._oidx >= orate && res._iidx >= irate) {
		res._iidx -= irate;
		res._oidx -= orate;
	}

	//output size
	isize = iidx * ssize;
	osize = oidx * ssize;
	return true;

}
#endif
