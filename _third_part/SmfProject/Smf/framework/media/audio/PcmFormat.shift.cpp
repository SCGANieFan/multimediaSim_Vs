#include "PcmFormat.h"
#include "PcmFormat.def.h"
#include <stdlib.h>
#include <string.h>
#include "smf_debug.h"
using namespace smf;
using namespace smf::audio;

template<int __shift_type, class T>static inline
void fmt_shift_(T* data, int samples, int8_t shf) {
	for (int i = 0; i < samples; i++) {
		data[i] = shift_t<__shift_type,T,int>::shift(data[i], shf);
	}
}
template<class T>static inline
void fmt_shift(T* data, int samples, int8_t shf) {
	if (shf > 0) fmt_shift_<1>(data, samples, shf);
	else fmt_shift_<-1>(data, samples, shf);
}
bool PcmFormat::LeftShift(void* data, int size, int8_t bits, int8_t shf) {
	auto n = size / (bits >> 3);
	switch (bits) {
	case  8: fmt_shift((int8_t *)data, n, shf); return true;
	case 16: fmt_shift((int16_t*)data, n, shf); return true;
	case 24: fmt_shift((int24_t*)data, n, shf); return true;
	case 32: fmt_shift((int32_t*)data, n, shf); return true;
	default:return false;
	}
}
