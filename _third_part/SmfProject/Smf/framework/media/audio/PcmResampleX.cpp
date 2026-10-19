#include "PcmResampleX.h"
#include "smf_debug.h"

using namespace smf::audio;

PcmResampleX::PcmResampleX() {

}
PcmResampleX::~PcmResampleX() {

}
bool PcmResampleX::Valid()const {
	return _cbResample;
}
PcmResampleX::operator bool()const {
	return Valid();
}
bool PcmResampleX::Reset() {
	_cbResample = 0;
	return true;
}
bool PcmResampleX::Reset(int rate, int channels,int sample_bits, int target_rate){
	target_rate = (target_rate) ? target_rate : rate;
	_irate = rate;
	_bits = sample_bits;
	_channels = channels;
	_orate = target_rate;
	_oadjust = 0;
	_ipos = 0;
	_opos = 0;
	memset(_vs, 0, sizeof(_vs));
	returnIfErrC(false, !_irate);
	returnIfErrC(false, !_orate);
	returnIfErrC(false, !_channels);
	returnIfErrC(false, !_bits);
	int ibits_idx = (_bits >> 3) - 1;
	returnIfErrCS(false, (ibits_idx < 0) || (ibits_idx > 3), "%d", _bits);
	//returnIfErrCS(false, (_channels != 1) && (_channels != 2), "%d", _channels);
	//returnIfErrCS(false, (_channels == 0) && (_channels > 8), "%d", _channels);
	returnIfErrC(false, _channels > 16);
	auto chn = _channels < 3 ? _channels : 3;
	_cbResample = _cbResamples[chn - 1][ibits_idx];
	RateUpdate(_irate, _orate);
	return true;
}
PcmResampleX::CbResample PcmResampleX::GetResample() const {
	return _cbResample;
}
bool PcmResampleX::Run(void* ibuf, int& isize, void* obuf, int& osize) {
	returnIfErrC(false, !_cbResample);
	_cbResample(ibuf, isize, obuf, osize,this);
	return true;
}
bool PcmResampleX::OutputRateAdjust(int adjust) {
	_oadjust = adjust;
	return true;
}

void PcmResampleX::RateUpdate(uint32_t irate, uint32_t orate){
	if (orate > 0x100000 && irate > orate) {
		irate = irate * 0x100000ull / orate;
		orate = 0x100000;
	}
	else if (irate > 0x100000 && orate > irate) {
		orate = orate * 0x100000ull / irate;
		irate = 0x100000;
	}
	_irate = irate;
	_orate = orate;
}

template<class T>
static void res1(void* ibuf, int& isize, void* obuf, int& osize, PcmResampleX*res){
	auto iptr = (T*)ibuf;
	auto optr = (T*)obuf;
	auto iend = (T*)((char*)iptr + isize);
	auto oend = (T*)((char*)optr + osize);
	auto istep = res->_orate + res->_oadjust;
	auto istep32 = (uint64_t)(0x10000000000ull / istep);
	auto ostep = res->_irate;
	auto ipos = res->_ipos;
	auto opos = res->_opos;
	auto v00 = res->_vs[0][0];
	auto v01 = res->_vs[0][1];
	while (iptr < iend && optr < oend) {
		if (ipos > opos) {
			opos += ostep;
			//*optr++ = v00 + (int64_t)(v01 - v00) / istep * (opos - ipos);
			*optr++ = v00 + ((int64_t)(v01 - v00) * istep32 * (opos - ipos) >> 40);
		}
		else if (ipos < opos) {
			ipos += istep;
			v00 = v01;
			v01 = *iptr++;
		}
		else {
			ipos += istep;
			opos += ostep;
			v00 = v01;
			v01 = *iptr++;
			*optr++ = v00;
		}
	}
	if (ipos > opos) {
		ipos -= opos;
		opos = 0;
	}
	else if (ipos < opos) {
		opos -= ipos;
		ipos = 0;
	}
	else {
		ipos = opos = 0;
	}
	res->_ipos = ipos;
	res->_opos = opos;
	res->_vs[0][0] = v00;
	res->_vs[0][1] = v01;
	isize = (char*)iptr - (char*)ibuf;
	osize = (char*)optr - (char*)obuf;
}

template<class T>
static void res2(void* ibuf, int& isize, void* obuf, int& osize, PcmResampleX* res) {
	auto iptr = (T*)ibuf;
	auto optr = (T*)obuf;
	auto iend = (T*)((char*)iptr + isize);
	auto oend = (T*)((char*)optr + osize);
	auto istep = res->_orate + res->_oadjust;
	auto istep32 = (int64_t)(0x10000000000ull / istep);
	auto ostep = res->_irate;
	auto ipos = res->_ipos;
	auto opos = res->_opos;
	auto v00 = res->_vs[0][0];
	auto v01 = res->_vs[0][1];
	auto v10 = res->_vs[1][0];
	auto v11 = res->_vs[1][1];
	while (iptr < iend && optr < oend) {
		if (ipos > opos) {
			opos += ostep;
			//*optr++ = v10 + (int64_t)(v11 - v10) / istep * (opos - ipos);
			auto x = (opos - ipos) * istep32;
			*optr++ = v00 + ((int64_t)(v01 - v00) * x >> 40);
			*optr++ = v10 + ((int64_t)(v11 - v10) * x >> 40);
		}
		else if (ipos < opos) {
			ipos += istep;
			v00 = v01;
			v01 = *iptr++;
			v10 = v11;
			v11 = *iptr++;
		}
		else {
			ipos += istep;
			opos += ostep;
			v10 = v11;
			v11 = *iptr++;
			*optr++ = v10;
			v00 = v01;
			v01 = *iptr++;
			*optr++ = v00;
		}
	}
	if (ipos > opos) {
		ipos -= opos;
		opos = 0;
	}
	else if (ipos < opos) {
		opos -= ipos;
		ipos = 0;
	}
	else {
		ipos = opos = 0;
	}
	res->_ipos = ipos;
	res->_opos = opos;
	res->_vs[0][0] = v00;
	res->_vs[0][1] = v01;
	res->_vs[1][0] = v10;
	res->_vs[1][1] = v11;
	isize = (char*)iptr - (char*)ibuf;
	osize = (char*)optr - (char*)obuf;
}

template<class T>
static void resn(void* ibuf, int& isize, void* obuf, int& osize, PcmResampleX* res) {
	auto iptr = (T*)ibuf;
	auto optr = (T*)obuf;
	auto iend = (T*)((char*)iptr + isize);
	auto oend = (T*)((char*)optr + osize);
	auto istep = res->_orate + res->_oadjust;
	auto istep32 = (int64_t)(0x10000000000ull / istep);
	auto ostep = res->_irate;
	auto ipos = res->_ipos;
	auto opos = res->_opos;
	auto vs = res->_vs;
	auto n = res->_channels;
	while (iptr < iend && optr < oend) {
		if (ipos > opos) {
			opos += ostep;
			//*optr++ = v10 + (int64_t)(v11 - v10) / istep * (opos - ipos);
			auto x = (opos - ipos) * istep32;
			for (int i = 0; i < n; i++) {
				*optr++ = vs[i][0] + ((int64_t)(vs[i][1] - vs[i][0]) * x >> 40);
			}
		}
		else if (ipos < opos) {
			ipos += istep;
			for (int i = 0; i < n; i++) {
				vs[i][0] = vs[i][1];
				vs[i][1] = *iptr++;
			}
		}
		else {
			ipos += istep;
			opos += ostep;
			for (int i = 0; i < n; i++) {
				*optr++ = vs[i][0] = vs[i][1];
				vs[i][1] = *iptr++;
			}
		}
	}
	if (ipos > opos) {
		ipos -= opos;
		opos = 0;
	}
	else if (ipos < opos) {
		opos -= ipos;
		ipos = 0;
	}
	else {
		ipos = opos = 0;
	}
	res->_ipos = ipos;
	res->_opos = opos;
	isize = (char*)iptr - (char*)ibuf;
	osize = (char*)optr - (char*)obuf;
}

#define int24_t int32_t
const PcmResampleX::CbResample PcmResampleX::_cbResamples[3][4] = {
	{&res1<int8_t>, &res1<int16_t>, &res1<int24_t>, &res1<int32_t>},
	{&res2<int8_t>, &res2<int16_t>, &res2<int24_t>, &res2<int32_t>},
	{&resn<int8_t>, &resn<int16_t>, &resn<int24_t>, &resn<int32_t>}
};
