#include "PcmFormatAdv.h"
#include <stdlib.h>
#include <string.h>
#include "smf_debug.h"
using namespace smf::audio;
bool PcmFormatAdv::Reset() {
	if (_fmt)_fmt.Reset();
	if (_res)_res.Reset();
	if (_buff) {Free(_buff); _buff = 0;}
	_funcProc = 0;
	_buffSizeRatio = 0;
	_buffSizeRatioDenum = 0;
	return true;
}
bool PcmFormatAdv::Reset(uint32_t i_rate, uint32_t o_rate
	, uint8_t i_channels, uint8_t o_channels
	, uint8_t i_bits, uint8_t o_bits
	, uint8_t i_width, uint8_t o_width
	, uint8_t i_float, uint8_t o_float
	, uint8_t i_interlace, uint8_t o_interlace
	, int8_t left_shift
	, bool lrmix
	, uint32_t chmap
	, int8_t chNO
) {
	if (!o_rate)o_rate = i_rate;
	if (!o_channels)o_channels = i_channels;
	if (!o_bits)o_bits = i_bits;
	if (i_width < ((i_bits + 7) >> 3))i_width = (i_bits + 7) >> 3;
	if (o_width < ((o_bits + 7) >> 3))o_width = (o_bits + 7) >> 3;

	auto shift = left_shift;
	auto fmten = i_channels != o_channels || i_bits != o_bits || i_width != o_width || lrmix;
	auto resen = i_rate != o_rate;
	bool fmtPrev = false;
	if (resen) {
		int8_t chnl = 0;
		int8_t bits = 0;
		int8_t width = 0;
		bool isfloat = false;
		if ((i_width != 3 && o_width == 3) || (i_rate > o_rate)) {
			fmtPrev = false;
			chnl = i_channels;
			bits = i_bits;
			width = i_width;
			isfloat = i_float;
			_buffSizeRatio = i_channels * i_width;
			_buffSizeRatioDenum = o_channels * o_width;
		}
		else if ((i_width == 3 && o_width != 3) || (i_rate < o_rate)){
			fmtPrev = true;
			chnl = o_channels;
			bits = o_bits;
			width = o_width;
			isfloat = o_float;
			_buffSizeRatio = i_rate;
			_buffSizeRatioDenum = o_rate;
		}
		else {
			dbgErrPL();
			return false;
		}
		//
		if (!_res.Reset(i_rate, o_rate, chnl, bits, width, (fmten ? 0 : shift), isfloat)) {
			dbgErrPL();
			return false;
		}
	}
	if (fmten) {
		auto rate = fmtPrev ? i_rate : o_rate;
		returnIfErrC(false, !_fmt.Reset(
			rate, rate
			, i_channels, o_channels
			, i_bits, o_bits
			, i_width, o_width
			, i_float, o_float
			, i_interlace, o_interlace
			, shift
			, lrmix
			, chmap
			, chNO
		));
	}
	//
	if (_fmt && !_res)_funcProc = &PcmFormatAdv::processFormat;
	else if (!_fmt && _res)_funcProc = &PcmFormatAdv::processResample;
	else if (_fmt && _res && fmtPrev)_funcProc = &PcmFormatAdv::processFormatResample;
	else if (_fmt && _res && !fmtPrev)_funcProc = &PcmFormatAdv::processResampleFormat;
	else _funcProc = &PcmFormatAdv::processCopy;
	//
	//dbgTestPXL("%s,%u/%u,%u/%u,%u/%u,%u/%u",Name(), rate,o_rate,channels,o_channels,sample_bits,o_bits,_sampleWidth,o_sampleWidth);
	return true;
}
bool PcmFormatAdv::Format(void* input, int& insize, void* output, int& outsize, void* temp) {
	if (_buffSizeRatioDenum && !temp) {
		if (!_buff) {
			int size = outsize * _buffSizeRatio / _buffSizeRatioDenum;
			_buff = Alloc(size);
			returnIfErrC(false, !_buff);
		}
		temp = _buff;
	}
	return _funcProc ? (this->*_funcProc)(input, insize, output, outsize, temp) : false;
}
bool PcmFormatAdv::processFormat(void* idata, int& isize, void* odata, int& osize, void* temp) {
	return _fmt.Format(idata, isize, odata, osize);
}
bool PcmFormatAdv::processResample(void* idata, int& isize, void* odata, int& osize, void* temp) {
	return _res.Run(idata, isize, odata, osize);
}
bool PcmFormatAdv::processFormatResample(void* idata, int& isize, void* odata, int& osize, void* temp) {
	int size = osize * _buffSizeRatio / _buffSizeRatioDenum;
	returnIfErrC(false, !_fmt.Format(idata, isize, temp, size));
	returnIfErrC(false, !_res.Run(temp, size, odata, osize));
	return true;
}
bool PcmFormatAdv::processResampleFormat(void* idata, int& isize, void* odata, int& osize, void* temp) {
	int size = osize* _buffSizeRatio / _buffSizeRatioDenum;
	returnIfErrC(false, !_res.Run(idata, isize, temp, size));
	returnIfErrC(false, !_fmt.Format(temp, size, odata, osize));
	return true;
}
bool PcmFormatAdv::processCopy(void* idata, int& isize, void* odata, int& osize, void* temp) {
	int size = isize < osize ? isize : osize;
	memcpy(odata, idata, size);
	isize = size;
	osize = size;
	return true;
}
bool PcmFormatAdv::IsCopyOnly()const {
	return _funcProc == &PcmFormatAdv::processCopy;
}
