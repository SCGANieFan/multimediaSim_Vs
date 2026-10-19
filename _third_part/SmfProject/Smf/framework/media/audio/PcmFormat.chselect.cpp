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
void fmt_proc_(const void* src_, void* dst_, int samples, int offset, int step, int8_t shf) {
	auto src = (Ti*)src_ + offset;
	auto dst = (To*)dst_;
	using Shift = shift_t<__shift_type, To, int>;
	for (int i = 0; i < samples; i++) {
		*dst++ = Shift::shift(*src, shf); src += step;
	}
}

template<class Ti, class To>static inline
void fmt_proc_(const void* src_, void* dst_, int samples, int offset, int step, float gain) {
	auto src = (Ti*)src_ + offset;
	auto dst = (To*)dst_;
	for (int i = 0; i < samples; i++) {
		*dst++ = (To)(*src * gain); src += step;
	}
}

template<class Ti, class To>static inline
void fmt_proc(const void* src, void* dst, int samples, PcmFormat* fmt) {
	auto ichs = fmt->_i_channels;
	auto shf = fmt->_shift;
	auto _chNO = fmt->_chNO;

	if (shf > 0)
		fmt_proc_< 1, Ti, To>(src, dst, samples, _chNO, ichs, shf);
	else if (shf < 0)
		fmt_proc_<-1, Ti, To>(src, dst, samples, _chNO, ichs, -shf);
	else
		fmt_proc_< 0, Ti, To>(src, dst, samples, _chNO, ichs, 0);
}

template<class Ti, class To>static inline
void fmt_prof(const void* src, void* dst, int samples, PcmFormat* fmt) {
	auto ichs = fmt->_i_channels;
	auto gain = fmt->_gain;
	auto _chNO = fmt->_chNO;

	fmt_proc_<Ti, To>(src, dst, samples, _chNO, ichs, gain);
}

const PcmFormat::CbFormatX PcmFormat::_cbFormatsSelect[5][5]{
	{&fmt_proc<int08_t,int08_t>,&fmt_proc<int16_t,int08_t>,&fmt_proc<int24_t,int08_t>,&fmt_proc<int32_t,int08_t>,&fmt_prof<float  ,int08_t>},
	{&fmt_proc<int08_t,int16_t>,&fmt_proc<int16_t,int16_t>,&fmt_proc<int24_t,int16_t>,&fmt_proc<int32_t,int16_t>,&fmt_prof<float  ,int16_t>},
	{&fmt_proc<int08_t,int24_t>,&fmt_proc<int16_t,int24_t>,&fmt_proc<int24_t,int24_t>,&fmt_proc<int32_t,int24_t>,&fmt_prof<float  ,int24_t>},
	{&fmt_proc<int08_t,int32_t>,&fmt_proc<int16_t,int32_t>,&fmt_proc<int24_t,int32_t>,&fmt_proc<int32_t,int32_t>,&fmt_prof<float  ,int32_t>},
	{&fmt_prof<int08_t,float  >,&fmt_prof<int16_t,float  >,&fmt_prof<int24_t,float  >,&fmt_prof<int32_t,float  >,&fmt_prof<float  ,float  >}
};


