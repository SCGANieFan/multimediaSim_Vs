#pragma once
#if 1
#include "plc_audio_cal_com.h"

#if 1	//shift
template<class Ti, class To, class Tx, int N, int NN, char shiftType = (N > 0 ? 1 : (N < 0 ? -1 : 0))>
struct RShift_t {
	static inline To Run(Ti x) {
		return (Tx)x;
	}
};

template<class Ti, class To, class Tx, int N, int NN>
struct RShift_t<Ti, To, Tx, N, NN, 1> {
	static inline To Run(Ti x) {
		return  (To)(((Tx)x) >> N);
	}
};

template<class Ti, class To, class Tx, int N, int NN>
struct RShift_t<Ti, To, Tx, N, NN, -1> {
	static inline To Run(Ti x) {
		return  (To)(((Tx)x) << NN);
	}
};


class RShift_c {
public:
	RShift_c() {}
	~RShift_c() {}
public:
	template<int N, class Ti, class To = Ti, class Tx = Ti>
	static inline To Run(Ti x) {
		return RShift_t<Ti, To, Tx, N, -N>::Run(x);
	}
}; 
#endif


#endif
#include "plc_base.h"
#include "plc_audio_cal_product_c.h"
#include "plc_audio_cal_basic_c.h"

template<class Ti, class To, class Tf, class Tx, i8 _rShift, i8 dir, i8 type = ((i8)(_rShift == 0 ? 0 : 1) * 10 + (i8)(dir == -1 ? 0 : 1))>
struct Product_t {
STATIC void RunAllCh(void* dst, void* src, void* fac, const i32 productSample, i8 channels) {
return;
}
STATIC void RunCh(void* dst, void* src, void* fac, const i32 productSample, i8 channels) {
return;
}
};
#if 1
//_rShift=0,dir=-1
template<class Ti, class To, class Tf, class Tx, i8 _rShift, i8 dir>
struct Product_t<Ti, To, Tf, Tx, _rShift, dir, 0> {
STATIC void RunAllCh(void* dst, void* src, void* fac, const i32 productSample, i8 channels) {
To* p_dst = (To*)dst;
Ti* p_src = (Ti*)src;
Tf* pFac = (Tf*)fac;
for (i32 s = 0; s < productSample; s++) {
	for (i8 ch = 0; ch < channels; ch++) {
		Tx tmp = static_cast<Tx>(*p_src) * *pFac;
		*p_dst = static_cast<To>(tmp);
		p_dst++;
		p_src++;
	}
	pFac--;
}
}
STATIC void RunCh(void* dst, void* src, void* fac, const i32 productSample, i8 channels) {
To* p_dst = (To*)dst;
Ti* p_src = (Ti*)src;
Tf* pFac = (Tf*)fac;
for (i32 s = 0; s < productSample; s++) {
	Tx tmp = static_cast<Tx>(*p_src) * *pFac;
	*p_dst = static_cast<To>(tmp);
	p_dst += channels;
	p_src += channels;
	pFac--;
}
}
};

//_rShift=0,dir=1
template<class Ti, class To, class Tf, class Tx, i8 _rShift, i8 dir>
struct Product_t<Ti, To, Tf, Tx, _rShift, dir, 1> {
STATIC void RunAllCh(void* dst, void* src, void* fac, const i32 productSample, i8 channels) {
To* p_dst = (To*)dst;
Ti* p_src = (Ti*)src;
Tf* pFac = (Tf*)fac;
for (i32 s = 0; s < productSample; s++) {
	for (i8 ch = 0; ch < channels; ch++) {
		Tx tmp = static_cast<Tx>(*p_src) * *pFac;
		*p_dst = static_cast<To>(tmp);
		p_dst++;
		p_src++;
	}
	pFac++;
}
}
STATIC void RunCh(void* dst, void* src, void* fac, const i32 productSample, i8 channels) {
To* p_dst = (To*)dst;
Ti* p_src = (Ti*)src;
Tf* pFac = (Tf*)fac;
for (i32 s = 0; s < productSample; s++) {
	Tx tmp = static_cast<Tx>(*p_src) * *pFac;
	*p_dst = static_cast<To>(tmp);
	p_dst += channels;
	p_src += channels;
	pFac++;
}
}
};

//_rShift!=0,dir=-1
template<class Ti, class To, class Tf, class Tx, i8 _rShift, i8 dir>
struct Product_t<Ti, To, Tf, Tx, _rShift, dir, 10> {
STATIC void RunAllCh(void* dst, void* src, void* fac, const i32 productSample, i8 channels) {
To* p_dst = (To*)dst;
Ti* p_src = (Ti*)src;
Tf* pFac = (Tf*)fac;
for (i32 s = 0; s < productSample; s++) {
	for (i8 ch = 0; ch < channels; ch++) {
		Tx tmp = static_cast<Tx>(*p_src)* *pFac;
		*p_dst = RShift_c::Run<_rShift>(tmp);
		p_dst++;
		p_src++;
	}
	pFac--;
}
}
STATIC void RunCh(void* dst, void* src, void* fac, const i32 productSample, i8 channels) {
To* p_dst = (To*)dst;
Ti* p_src = (Ti*)src;
Tf* pFac = (Tf*)fac;
for (i32 s = 0; s < productSample; s++) {
	Tx tmp = static_cast<Tx>(*p_src) * *pFac;
	*p_dst = RShift_c::Run<_rShift>(tmp);
	p_dst += channels;
	p_src += channels;
	pFac--;
}
}
};

//_rShift!=0,dir=1
template<class Ti, class To, class Tf, class Tx, i8 _rShift, i8 dir>
struct Product_t<Ti, To, Tf, Tx, _rShift, dir, 11> {
STATIC void RunAllCh(void* dst, void* src, void* fac, const i32 productSample, i8 channels) {
To* p_dst = (To*)dst;
Ti* p_src = (Ti*)src;
Tf* pFac = (Tf*)fac;
for (i32 s = 0; s < productSample; s++) {
	for (i8 ch = 0; ch < channels; ch++) {
		Tx tmp = static_cast<Tx>(*p_src) * *pFac;
		*p_dst = RShift_c::Run<_rShift>(tmp);
		p_dst++;
		p_src++;
	}
	pFac++;
}
}
STATIC void RunCh(void* dst, void* src, void* fac, const i32 productSample, i8 channels) {
To* p_dst = (To*)dst;
Ti* p_src = (Ti*)src;
Tf* pFac = (Tf*)fac;
for (i32 s = 0; s < productSample; s++) {
	Tx tmp = static_cast<Tx>(*p_src) * *pFac;
	*p_dst = RShift_c::Run<_rShift>(tmp);
	p_dst += channels;
	p_src += channels;
	pFac++;
}
}
};
#endif
#if 1
template<class Ti, class To, class Tf, class Tx, i8 _rShift = 0>
#endif
class Product_c
{
public:
Product_c() {}
~Product_c() {}
public:
STATIC INLINE void RunAllChForWard(void* dst, void* src, void* fac, const i32 productSample, i32 channels) {
Product_t<Ti, Ti, Tf, Tx, _rShift, 1>::RunAllCh(dst, src, fac, productSample, channels);
}
STATIC INLINE void RunAllChBackWard(void* dst, void* src, void* fac, const i32 productSample, i32 channels) {
Product_t<Ti, Ti, Tf, Tx, _rShift, -1>::RunAllCh(dst, src, fac, productSample, channels);
}
STATIC INLINE void RunChForWard(void* dst, void* src, void* fac, const i32 productSample, i32 channels) {
Product_t<Ti, Ti, Tf, Tx, _rShift, 1>::RunCh(dst, src, fac, productSample, channels);
}
STATIC INLINE void RunChBackWard(void* dst, void* src, void* fac, const i32 productSample, i32 channels) {
Product_t<Ti, Ti, Tf, Tx, _rShift, -1>::RunCh(dst, src, fac, productSample, channels);
}
};
