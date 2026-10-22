#include "MediaLc3.h"
#include "smf_debug.h"
#include "SmfFCC.h"
#include "SmfHash.h"
using namespace smf;

MediaLc3::MediaLc3() {
	_codec = fcc32("lc3");
	_size = sizeof(MediaLc3);
	_frameDMs = 0;
	_epmode = 0;///lc3_epmode_e
	_epmr = 0;///lc3_epmr_e;
	_hrmode = false;///high resolution mode
	_lfemode = 0;///lfe mode(deactivates LTPF, TNS, NF)
	_other = 0;
	_sampleBits = 0;
	_sampleWidth = 0;
    _isNonInterlace = 0;
	_frameSize = 0;
	_quanPrec = 48;
	_ltpfFlag = 1;
}

bool MediaLc3::setX(uint32_t key, void* val) {
	switch (key) {
	case Hash("dms"):
    case Hash("frame_dms"):
	case Hash("frameDMs"): _frameDMs = (uint32_t)val;	return true;
	case Hash("fsize"):
	case Hash("frame_size"): _frameSize = (uint32_t)val;	return true;
	case Hash("epmode"): _epmode = (uint32_t)val;	return true;
	case Hash("epmr"): _epmr = (uint32_t)val;	return true;
	case Hash("hrmode"): _hrmode = (uint32_t)val;	return true;
	case Hash("lfemode"): _lfemode = (uint32_t)val;	return true;
	case Hash("quanPrec"): _quanPrec = (uint32_t)val; return true;
	case Hash("ltpfFlag"): _ltpfFlag = (uint32_t)val; return true;
	case Hash("other"): _other = (void*)val;	return true;
	case Hash("bits"):
    case Hash("sample_bits"):  _sampleBits = (uint32_t)val; return true;
	case Hash("sample_width"): _sampleWidth = (uint32_t)val; return true;
	case Hash("no_interlace"): _isNonInterlace = (bool)val; return true;
	}
	return MediaAudio::setX(key, val);
}

bool MediaLc3::getX(uint32_t key, void* val) const {
	switch (key) {
	case Hash("dms"):
	case Hash("frameDMs"):*(uint32_t*)val = _frameDMs;	return true;
	case Hash("epmode"): * (uint32_t*)val = _epmode;	return true;
	case Hash("epmr"): * (uint32_t*)val = _epmr;	return true;
	case Hash("hrmode"): * (uint32_t*)val = _hrmode;	return true;
	case Hash("lfemode"): *(uint32_t*)val = _lfemode;	return true;
	case Hash("other"): *(void**)val = _other;	return true;
	}
	return MediaAudio::getX(key, val);
}