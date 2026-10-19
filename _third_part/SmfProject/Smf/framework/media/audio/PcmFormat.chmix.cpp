#include "PcmFormat.h"
#include "PcmFormat.def.h"
#include "smf_debug.h"
using namespace smf;
using namespace smf::audio;

#ifndef __max
#define __max(x,y) (((x)>(y))?(x):(y))
#endif
#ifndef __min
#define __min(x,y) (((x)<(y))?(x):(y))
#endif

template<int __shift_type, class Ti, class To>static inline
void fmt_lrmix_(const void* src_, void* dst_, int samples, int ochs, int shf) {
	auto src = (Ti*)src_;
	auto dst = (To*)dst_;
	using Shift = shift_t<__shift_type, To, int>;
	if (ochs == 1) {
		for (int i = 0; i < samples; i++) {
			*dst++ = Shift::shift((src[0] >> 1) + (src[1] >> 1), shf); src += 2;
		}
	}
	else {
		for (int i = 0; i < samples; i++) {
			auto v = Shift::shift((src[0] >> 1) + (src[1] >> 1), shf); src += 2;
			for (int x = 0; x < ochs; x++)
				*dst++ = v;
		}
	}
}

template<class Ti, class To>static inline
void fmt_lrmix_(const void* src_, void* dst_, int samples, int ochs, float gain) {
	auto src = (Ti*)src_;
	auto dst = (To*)dst_;
	if (ochs == 1) {
		for (int i = 0; i < samples; i++) {
			*dst++ = (To)((src[0] + src[1]) * gain); src += 2;
		}
	}
	else {
		for (int i = 0; i < samples; i++) {
			auto v = (src[0] + src[1]) * gain; src += 2;
			for (int x = 0; x < ochs; x++)
				*dst++ = (To)(v);
		}
	}
}

template<class Ti, class To>static inline
void fmt_lrmix(const void* src, void* dst, int samples, PcmFormat* fmt) {
	auto ochs = fmt->_o_channels;
	auto shf = fmt->_shift;
	if (shf > 0)
		fmt_lrmix_< 1, Ti, To>(src, dst, samples, ochs, shf);
	else if (shf < 0)
		fmt_lrmix_<-1, Ti, To>(src, dst, samples, ochs, shf);
	else
		fmt_lrmix_< 0, Ti, To>(src, dst, samples, ochs, shf);
}

template<class Ti, class To>static inline
void fmt_lrmif(const void* src, void* dst, int samples, PcmFormat* fmt) {
	auto ochs = fmt->_o_channels;
	auto gain = fmt->_gain;
	fmt_lrmix_<Ti, To>(src, dst, samples, ochs, gain);
}

const PcmFormat::CbFormatX PcmFormat::_cbFormatsMix[5][5]{
	{&fmt_lrmix<int08_t,int08_t>,&fmt_lrmix<int16_t,int08_t>,&fmt_lrmix<int24_t,int08_t>,&fmt_lrmix<int32_t,int08_t>,&fmt_lrmif<float  ,int08_t>},
	{&fmt_lrmix<int08_t,int16_t>,&fmt_lrmix<int16_t,int16_t>,&fmt_lrmix<int24_t,int16_t>,&fmt_lrmix<int32_t,int16_t>,&fmt_lrmif<float  ,int16_t>},
	{&fmt_lrmix<int08_t,int24_t>,&fmt_lrmix<int16_t,int24_t>,&fmt_lrmix<int24_t,int24_t>,&fmt_lrmix<int32_t,int24_t>,&fmt_lrmif<float  ,int24_t>},
	{&fmt_lrmix<int08_t,int32_t>,&fmt_lrmix<int16_t,int32_t>,&fmt_lrmix<int24_t,int32_t>,&fmt_lrmix<int32_t,int32_t>,&fmt_lrmif<float  ,int32_t>},
	{&fmt_lrmif<int08_t,float  >,&fmt_lrmif<int16_t,float  >,&fmt_lrmif<int24_t,float  >,&fmt_lrmif<int32_t,float  >,&fmt_lrmif<float  ,float  >}
};
