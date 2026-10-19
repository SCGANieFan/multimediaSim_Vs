#include "PcmFormat.h"
#include "PcmFormat.def.h"
#include "smf_debug.h"
using namespace smf;
using namespace smf::audio;

template<int __shift_type, class Ti, class To>static inline
void fmt_proc_(const void* src_, void* dst_, int samples, int8_t cnt, uint32_t chmap, int8_t shf) {
	auto src = (Ti*)src_;
	auto dst = (To*)dst_;
	using Shift = shift_t<__shift_type, To, int>;
	for (int i = 0; i < samples; i++) {
		auto map = chmap;
		for (int x = 0; x < cnt; x++) {
			if (map & 1) {
				*dst++ = Shift::shift(*src++, shf);
			}
			else {
				src++;
			}
			map >>= 1;
		}
	}
}
template<class Ti, class To>static inline
void fmt_proc_(const void* src_, void* dst_, int samples, int8_t cnt, uint32_t chmap, float gain) {
	auto src = (Ti*)src_;
	auto dst = (To*)dst_;
	for (int i = 0; i < samples; i++) {
		auto map = chmap;
		for (int x = 0; x < cnt; x++) {
			if (map & 1) {
				*dst++ = static_cast<To>(*src++ * gain);
			}
			else {
				src++;
			}
			map >>= 1;
		}
	}
}

template<class Ti, class To>static inline
void fmt_proc(const void* src, void* dst, int samples, PcmFormat* fmt) {
	auto shf = fmt->_shift;
	auto cnt = fmt->_i_channels;
	auto chmap = fmt->_chMap;

	if (shf > 0)
		fmt_proc_< 1, Ti, To>(src, dst, samples, cnt, chmap, shf);
	else if (shf < 0)
		fmt_proc_<-1, Ti, To>(src, dst, samples, cnt, chmap, -shf);
	else
		fmt_proc_< 0, Ti, To>(src, dst, samples, cnt, chmap, 0);
}

template<class Ti, class To>static inline
void fmt_prof(const void* src, void* dst, int samples, PcmFormat* fmt) {
	auto gain = fmt->_gain;
	auto cnt = fmt->_i_channels;
	auto chmap = fmt->_chMap;
	fmt_proc_<Ti, To>(src, dst, samples, cnt, chmap, gain);
}

const PcmFormat::CbFormatX PcmFormat::_cbFormatsMap[5][5]{
	{&fmt_proc<int08_t,int08_t>,&fmt_proc<int16_t,int08_t>,&fmt_proc<int24_t,int08_t>,&fmt_proc<int32_t,int08_t>,&fmt_prof<float  ,int08_t>},
	{&fmt_proc<int08_t,int16_t>,&fmt_proc<int16_t,int16_t>,&fmt_proc<int24_t,int16_t>,&fmt_proc<int32_t,int16_t>,&fmt_prof<float  ,int16_t>},
	{&fmt_proc<int08_t,int24_t>,&fmt_proc<int16_t,int24_t>,&fmt_proc<int24_t,int24_t>,&fmt_proc<int32_t,int24_t>,&fmt_prof<float  ,int24_t>},
	{&fmt_proc<int08_t,int32_t>,&fmt_proc<int16_t,int32_t>,&fmt_proc<int24_t,int32_t>,&fmt_proc<int32_t,int32_t>,&fmt_prof<float  ,int32_t>},
	{&fmt_prof<int08_t,float  >,&fmt_prof<int16_t,float  >,&fmt_prof<int24_t,float  >,&fmt_prof<int32_t,float  >,&fmt_prof<float  ,float  >}
};

int PcmFormat::getChsFromMap()const {
	auto map = _chMap;
	int c = 0;
	while (map) {
		if (map & 1)c++;
		map >>= 1;
	}
	return c;
}
