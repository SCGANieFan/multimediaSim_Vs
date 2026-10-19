#include "PcmResampleAdv.h"
#include "PcmFormat.h"
#include "unique_ptr.h"
#include "smf_debug.h"
#include <memory>

using namespace smf;
using namespace smf::audio;

#define max(x,y) (((x)>(y))?(x):(y))
//
PcmResampleAdv::~PcmResampleAdv() {
	Reset();
}
bool PcmResampleAdv::Valid()const {
	return _hd || _res;
}
PcmResampleAdv::operator bool()const {
	return Valid();
}
bool PcmResampleAdv::Reset() {
	if (_hd) {Free(_hd); _hd = 0;}
	if (_res) {delete _res; _res = 0;}
	return true;
}
bool PcmResampleAdv::Reset(uint32_t rate, uint32_t target_rate
	, uint8_t channels, uint8_t sample_bits, uint8_t sample_width
	, int8_t left_shift, bool is_float) {
	if (is_float || !reset(rate, target_rate, channels, sample_bits, sample_width, left_shift)) {
		std::unique_ptr<PcmResample> res(new PcmResample());
		returnIfErrC(false, !res);
		returnIfErrC(false, !res->Reset(rate, target_rate, channels, sample_bits, sample_width, left_shift, is_float));
		_res = res.release();
	}
	return true;
}
bool PcmResampleAdv::reset(int rate, int target_rate, int8_t channels, int8_t sample_bits, int8_t sample_width, int8_t left_shift){
	returnIfWarnC(false, !AudioResamples().get_size);
	auto size = AudioResamples().get_size(max(rate, target_rate));
	returnIfWarnCS(false, !size, "%d to %d", rate, target_rate);
	VoidBuffer ap(size);
	void* hd = (void*)ap;
	returnIfErrCS(false, !hd, "%d to %d", rate, target_rate);
	returnIfWarnC(false, !AudioResamples().open(hd
		, channels, channels
		, rate, target_rate
		, sample_bits, sample_bits
	));
	ap.release();
	_hd = hd;
	_shf = left_shift;
	_bits = sample_bits;
	_width = sample_width;
	//dbgTestPDL(size);
	//dbgTestDump(_hd,16);
	return true;
}
bool PcmResampleAdv::Run(void* ibuf, int& isize, void* obuf, int& osize) {
	if (_hd)
		return AudioResamples().run(_hd, ibuf, &isize, obuf, &osize) && (_shf || PcmFormat::LeftShift(obuf, osize, _bits, _shf));
	else if (_res)
		return _res->Run(ibuf, isize, obuf, osize);
	else
		return false;
}
