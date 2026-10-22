#include "MediaAac.h"
#include "SmfFCC.h"
#include "SmfHash.h"
#include "smf_debug.h"
#include "BitStream.h"
using namespace smf;
MediaAac::MediaAac() { 
	_codec = fcc32("aac");
	_size = sizeof(MediaAac);
	_package = 0;
	_aot = 0;
	_layout = 0;
	_vbr = 0;
}
bool MediaAac::setX(uint32_t key, void* val) {
	switch (key) {
	case Hash("aot"): _aot = (uint32_t)val;	return true;
	case Hash("pkg"):
	case Hash("package"): _package = (uint32_t)val;	return true;
	}
	return MediaAudio::setX(key, val);
}
bool MediaAac::getX(uint32_t key, void* val) const {
	switch (key) {
	case Hash("aot"): *(uint32_t*)val = _aot;	return true;
	case Hash("pkg"):
	case Hash("package"): *(uint32_t*)val = _package;	return true;
	}
	return MediaAudio::getX(key, val);
}
bool MediaAac::SetExtraData(void* data, uint32_t size) {
	return IMedia::SetExtraData(data, size) && ParseASC();
}
bool MediaAac::GetExtraData(void*& data, uint32_t& size) {
	if (IMedia::GetExtraData(data, size))return true;
	return false;
}
uint8_t MediaAac::RateIndex()const {
	static const uint32_t tbl[]{
		96000,88200,64000
		,48000,44100,32000
		,24000,22050,16000
		,12000,11025,8000
		,7350,0,0
		,0
	};
	for (int i = 0; i < 0xc; i++) {
		if (_rate == tbl[i])
			return i;
	}
	return 0xf;
}

bool MediaAac::ParseASC() {
	returnIf(true, _package != SMF_AAC_PACKAGE_RAW);
	returnIfWarnC(false, !_extraData);
	returnIfWarnC(false, !_extraSize);
	if(_aot == 0) {
		BsReader bs(_extraData, _extraSize);
		// parse AOT
		int tmp = 0;
		tmp = bs.Read(5);
		if(tmp == 0x1f) {
			int tmp2 = bs.Read(6);
			tmp = 32 + tmp2;
		}
		_aot = tmp;
	}
	// dbgTestPDL(_aot);
	return true;
}