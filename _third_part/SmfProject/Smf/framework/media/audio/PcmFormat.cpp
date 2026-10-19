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
template<class Ti, class To>static inline
void fmt_lrmix(Ti* src, To* dst, int samples, PcmFormat* fmt);

bool PcmFormat::Valid()const {
	return _cbFormatX;
}
PcmFormat::operator bool()const {
	return Valid();
}
bool PcmFormat::Reset() {
	_cbFormatX = nullptr;
	return true;
}
bool PcmFormat::Reset(const PcmSpec& i, const PcmSpec& o
	, int8_t left_shift
	, bool lrmix
	, uint32_t chmap
	, int8_t chNO
){
	return Reset(i.rate, o.rate
	, i.channels, o.channels
	, i.bits, o.bits
	, i.width, o.width
	, i.is_float, o.is_float
	, i.interlace, o.interlace
	, left_shift
	, lrmix
	, chmap
	, chNO
	);
}
bool PcmFormat::Reset(uint32_t i_rate, uint32_t o_rate
	, uint8_t i_channels, uint8_t o_channels
	, uint8_t i_bits, uint8_t o_bits
	, uint8_t i_width, uint8_t o_width
	, uint8_t i_float, uint8_t o_float
	, uint8_t i_interlace, uint8_t o_interlace
	, int8_t left_shift
	, bool lrmix
	, uint32_t chmap
	, int8_t chNO
) 
{
	if (!o_rate)o_rate = i_rate;
	if (!o_channels)o_channels = i_channels;
	if (!o_bits)o_bits = i_bits;

	if (i_width < ((i_bits + 7) >> 3))i_width = (i_bits + 7) >> 3;
	if (o_width < ((o_bits + 7) >> 3))o_width = (o_bits + 7) >> 3;

	_i_rate = i_rate;
	_i_channels = i_channels;
	_i_bits = i_bits;
	_i_width = i_width;
	_i_float = i_float;
	_i_interlace = i_interlace;

	_o_rate = o_rate;
	_o_channels = o_channels;
	_o_bits = o_bits;
	_o_width = o_width;
	_o_float = o_float;
	_o_interlace = o_interlace;
	//
	//_cbFormat = NULL;
	int iIdx = i_width - 1;
	int oIdx = o_width - 1;
	returnIfErrCS(false, iIdx < 0 || iIdx > 3, "unsupport sample width:%d/%d", i_width, i_bits);
		returnIfErrCS(false, oIdx < 0 || oIdx > 3, "unsupport sample width:%d/%d", o_width, o_bits);
	returnIfErrCS(false, i_channels < 1 || i_channels > 8, "unsupport channels:%d", i_channels);
	returnIfErrCS(false, o_channels < 1 || o_channels > 8, "unsupport channels:%d", o_channels);
	//
	//dbgTestPDL(_target_channels);
	//dbgTestPDL(_target_width);
	//dbgTestPDL(_channels);
	_isampleWidth = i_channels * i_width;
	_osampleWidth = o_channels * o_width;
	_shift = o_bits - i_bits + left_shift;
	_chMap = chmap;
	_lrmix = lrmix;
	_chNO = chNO;
	if (i_float)iIdx = 4;
	if (o_float)oIdx = 4;
	if (i_float && !o_float)_gain = 1u << (_o_bits - 1);
	if (!i_float && o_float)_gain = 1.f / (1u << (_i_bits - 1));
	if (i_float && o_float)_gain = 1.f;
	if (chNO >= 0) {
		returnIfErrC(false, o_channels != 1);
		returnIfErrC(false, i_channels < o_channels);
		returnIfErrC(false, chNO+1 > i_channels);
		_cbFormatX = _cbFormatsSelect[oIdx][iIdx];
	}
	else if (chmap) {
		returnIfErrC(false, o_channels != getChsFromMap());
		returnIfErrC(false, i_channels < o_channels);
		_cbFormatX = _cbFormatsMap[oIdx][iIdx];
	}
	else if (lrmix && (i_channels == 2)) {
		_cbFormatX = _cbFormatsMix[oIdx][iIdx];
	}
	else {
		_cbFormatX = _cbFormats[oIdx][iIdx];
	}
	return true;
}

bool PcmFormat::Format(void* input, int& insize, void* output, int& outsize) {
	returnIfErrC(false, !_cbFormatX);
	_cbFormatX(input, output, fmt_size(insize, outsize), this);
	return true;
}

int PcmFormat::fmt_size(int& insize, int& outsize) {
	//dbgTestPXL("%d,%d,0x%x,%d,%d", insize, outsize, _scale, _width, _channels);
	auto isamples = insize / _isampleWidth;
	auto osamples = outsize / _osampleWidth;
	auto samples = __min(isamples, osamples);
	insize = samples * _isampleWidth;
	outsize = samples * _osampleWidth;
	return samples;
}
