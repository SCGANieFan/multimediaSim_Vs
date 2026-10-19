#include "PcmFormat.h"
#include "PcmFormat.def.h"
#include <stdlib.h>
#include <string.h>
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
void fmt_proc_(const void* src_, void* dst_, int samples, int8_t cnt, int8_t scnt, int8_t dcnt, bool copy, int8_t shf) {
	auto src = (Ti*)src_;
	auto dst = (To*)dst_;
	using Shift = shift_t<__shift_type, To, int>;
	if (scnt) {
		for (int i = 0; i < samples; i++) {
			for (int x = 0; x < cnt; x++) {
				*dst++ = Shift::shift(*src++, shf);
			}
			src += scnt;
		}
	}
	else if (dcnt) {
		if (copy) {
			for (int i = 0; i < samples; i++) {
				auto ptr = dst;
				for (int x = 0; x < cnt; x++) {
					*dst++ = Shift::shift(*src++, shf);
				}
				for (int x = 0; x < dcnt; x++) {
					*dst++ = *ptr++;
				}
			}
		}
		else {
			for (int i = 0; i < samples; i++) {
				for (int x = 0; x < cnt; x++) {
					*dst++ = Shift::shift(*src++, shf);
				}
				for (int x = 0; x < dcnt; x++) {
					*dst++ = 0;
				}
			}
		}
	}
	else {
		auto n = samples * cnt;
		for (int i = 0; i < n; i++) {
			*dst++ = Shift::shift(*src++, shf);
		}
	}
}

template<class Ti, class To>static inline
void fmt_proc_(const void* src_, void* dst_, int samples, int8_t cnt, int8_t scnt, int8_t dcnt, bool copy, float gain) {
	auto src = (Ti*)src_;
	auto dst = (To*)dst_;
	if (scnt) {
		for (int i = 0; i < samples; i++) {
			for (int x = 0; x < cnt; x++) {
				*dst++ = (To)(*src++ * gain);
			}
			src += scnt;
		}
	}
	else if (dcnt) {
		if (copy) {
			for (int i = 0; i < samples; i++) {
				auto ptr = dst;
				for (int x = 0; x < cnt; x++) {
					*dst++ = (To)(*src++ * gain);
				}
				for (int x = 0; x < dcnt; x++) {
					*dst++ = *ptr++;
				}
			}
		}
		else {
			for (int i = 0; i < samples; i++) {
				for (int x = 0; x < cnt; x++) {
					*dst++ = (To)(*src++ * gain);
				}
				for (int x = 0; x < dcnt; x++) {
					*dst++ = 0;
				}
			}
		}
	}
	else {
		auto n = samples * cnt;
		for (int i = 0; i < n; i++) {
			*dst++ = (To)(*src++ * gain);
		}
	}
}

template<class Ti, class To>static inline
void fmt_proc_(const void* src_, void* dst_, int samples, int8_t cnt, int8_t scnt, int8_t dcnt, bool copy) {
	auto src = (Ti*)src_;
	auto dst = (To*)dst_;
	if (scnt) {
		for (int i = 0; i < samples; i++) {
			for (int x = 0; x < cnt; x++) {
				*dst++ = *src++;
			}
			src += scnt;
		}
	}
	else if (dcnt) {
		if (copy) {
			for (int i = 0; i < samples; i++) {
				auto ptr = dst;
				for (int x = 0; x < cnt; x++) {
					*dst++ = *src++;
				}
				for (int x = 0; x < dcnt; x++) {
					*dst++ = *ptr++;
				}
			}
		}
		else {
			for (int i = 0; i < samples; i++) {
				for (int x = 0; x < cnt; x++) {
					*dst++ = *src++;
				}
				for (int x = 0; x < dcnt; x++) {
					*dst++ = 0;
				}
			}
		}
	}
	else {
		auto n = samples * cnt;
		for (int i = 0; i < n; i++) {
			*dst++ = *src++;
		}
	}
}

template<class Ti, class To>static inline
void fmt_proc(const void* src, void* dst, int samples, PcmFormat* fmt) {
	auto shf = fmt->_shift;
	auto scnt = fmt->_i_channels;
	auto dcnt = fmt->_o_channels;
	auto copy = fmt->_chcopy;
	auto cnt = scnt > dcnt ? dcnt : scnt;
	scnt -= cnt;
	dcnt -= cnt;

	if (shf > 0)
		fmt_proc_< 1, Ti, To>(src, dst, samples, cnt, scnt, dcnt, copy, shf);
	else if (shf < 0) 
		fmt_proc_<-1, Ti, To>(src, dst, samples, cnt, scnt, dcnt, copy, -shf);
	else 
		fmt_proc_< 0, Ti, To>(src, dst, samples, cnt, scnt, dcnt, copy, 0);
}

template<class Ti, class To>static inline
void fmt_copy(const void* src, void* dst, int samples, PcmFormat* fmt) {
	auto scnt = fmt->_i_channels;
	auto dcnt = fmt->_o_channels;
	auto copy = fmt->_chcopy;
	auto cnt = scnt > dcnt ? dcnt : scnt;
	scnt -= cnt;
	dcnt -= cnt;
	fmt_proc_<Ti, To>(src, dst, samples, cnt, scnt, dcnt, copy);
}

template<class Ti, class To>static inline
void fmt_prof(const void* src, void* dst, int samples, PcmFormat* fmt) {
	auto gain = fmt->_gain;
	auto scnt = fmt->_i_channels;
	auto dcnt = fmt->_o_channels;
	auto copy = fmt->_chcopy;
	auto cnt = scnt > dcnt ? dcnt : scnt;
	scnt -= cnt;
	dcnt -= cnt;
	fmt_proc_<Ti, To>(src, dst, samples, cnt, scnt, dcnt, copy, gain);
}

const PcmFormat::CbFormatX PcmFormat::_cbFormats[5][5]{
	{&fmt_proc<int08_t,int08_t>,&fmt_proc<int16_t,int08_t>,&fmt_proc<int24_t,int08_t>,&fmt_proc<int32_t,int08_t>,&fmt_prof<float  ,int08_t>},
	{&fmt_proc<int08_t,int16_t>,&fmt_proc<int16_t,int16_t>,&fmt_proc<int24_t,int16_t>,&fmt_proc<int32_t,int16_t>,&fmt_prof<float  ,int16_t>},
	{&fmt_proc<int08_t,int24_t>,&fmt_proc<int16_t,int24_t>,&fmt_proc<int24_t,int24_t>,&fmt_proc<int32_t,int24_t>,&fmt_prof<float  ,int24_t>},
	{&fmt_proc<int08_t,int32_t>,&fmt_proc<int16_t,int32_t>,&fmt_proc<int24_t,int32_t>,&fmt_proc<int32_t,int32_t>,&fmt_prof<float  ,int32_t>},
	{&fmt_prof<int08_t,float  >,&fmt_prof<int16_t,float  >,&fmt_prof<int24_t,float  >,&fmt_prof<int32_t,float  >,&fmt_copy<float  ,float  >}
};
