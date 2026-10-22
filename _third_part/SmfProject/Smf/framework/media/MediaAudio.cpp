#include "MediaAudio.h"
#include "Object.h"
#include "smf_debug.h"
#include "SmfFCC.h"
#include "SmfHash.h"
using namespace smf;
MediaAudio::MediaAudio() {
	_mflags = IS_Audio;
	_size = sizeof(MediaAudio);
	_rate = 0;
	_channels = 0;
	_rev = 0;
	_frameSamples = 0;
}
uint32_t MediaAudio::FrameDurationUs()const {
	return _rate ? (_frameSamples * 1000000ull) / _rate : 0;
}
bool MediaAudio::setX(uint32_t key, void* val) {
	switch (key) {
	case Hash("rate"):
	case Hash("sampleRate"): _rate = (uint32_t)val;	return true;
	case Hash("ch"):
	case Hash("channels"):	_channels = (uint32_t)val;	return true;
	case Hash("fromEncode"):return FromEncode((smf_media_audio_encode_t*)val);
	}
	return IMedia::setX(key,val);
}
bool MediaAudio::getX(uint32_t key, void* val) const {
	switch (key) {
	case Hash("rate"):
	case Hash("sampleRate"):*(uint32_t*)val = _rate;	return true;
	case Hash("ch"):
	case Hash("channels"):	*(uint32_t*)val = _channels;	return true;
	}
	return IMedia::getX(key,val);
}
bool MediaAudio::FromEncode(smf_media_audio_encode_t*enc) {
	returnIfErrC(false, !IMedia::IsAudio(enc->codec));
	auto offset = sizeof(smf_media_def_t) + sizeof(smf_media_audio_def_t);
	returnIfErrC(false, _size < offset);
	if (enc->bitrate) {
		_bitrate = enc->bitrate;
	}
	if (_size > offset) {
		memcpy((char*)this + offset, (char*)enc->params, _size - offset);
	}
	return true;
}
char* MediaAudio::Print(char* ptr, char* end) const {
	ptr = IMedia::Print(ptr, end);
	return snprintf(ptr, end, "%u/%u"
		, _rate, _channels
	);
}

MediaPcm::MediaPcm() {
	_codec = fcc32("pcm");
	_size = sizeof(MediaPcm);
	_sampleBits = 16;
	_sampleWidth = 2;
	_isNonInterlace = 0;
	_isUnSigned = 0;
	_isBigEndian = 0;
	_isFloat = 0;
}

//bool MediaPcm::FromMedia(MediaPcm* media) {
//	returnIfWarnC(false, !media);
//	FromMedia((MediaAudio*)media);
//	DefineFromMedia(_sampleWidth);
//	DefineFromMedia(_sampleBits);
//	DefineFromMedia(_isNonInterlace);
//	DefineFromMedia(_isUnSigned);
//	DefineFromMedia(_isBigEndian);
//	DefineFromMedia(_isFloat);
//	return true;
//}
//uint32_t MediaPcm::Bitrate()const {
//	return _rate * _channels * _sampleWidth * 8;
//}
smf_fraction_t MediaPcm::BytePerSecond()const {
	return { (int)_rate * _channels * _sampleWidth,1 };
}

uint32_t MediaPcm::SizeAlign()const {
	return _channels * _sampleWidth;
}
bool MediaPcm::setX(uint32_t key, void* val) {
	switch (key) {
	case Hash("w"):
	case Hash("width"):	_sampleWidth = (uint32_t)val;	return true;
	case Hash("bits"):
	case Hash("depth"):	
		_sampleBits = (uint32_t)val;
		if ((_sampleWidth << 3) < _sampleBits)_sampleWidth = _sampleBits >> 3;
		return true;
	case Hash("float"):_isFloat = !!val; return true;
	case Hash("interlace"):_isNonInterlace = !val; return true;
	case Hash("signed"):_isUnSigned = !val; return true;
	case Hash("littleEndian"):_isBigEndian = !val; return true;
	case Hash("bigEndian"):_isBigEndian = !!val; return true;
	}
	return MediaAudio::setX(key, val);
}
bool MediaPcm::getX(uint32_t key, void* val) const {
	switch (key) {
	case Hash("w"):
	case Hash("width"):	*(uint32_t*)val = _sampleWidth;	return true;
	case Hash("bits"):
	case Hash("depth"):	*(uint32_t*)val = _sampleBits;	return true;
	case Hash("float"):*(bool*)val = _isFloat; return true;
	case Hash("interlace"):*(bool*)val = !_isNonInterlace; return true;
	case Hash("signed"):*(bool*)val = !_isUnSigned; return true;
	case Hash("littleEndian"):*(bool*)val = !_isBigEndian; return true;
	case Hash("bigEndian"):*(bool*)val = _isBigEndian; return true;
	}
	return MediaAudio::getX(key, val);
}
bool MediaPcm::SetExtraData(void* data, uint32_t size) {
	IMedia::SetExtraData(data, size);
	return true;
}
bool MediaPcm::GetExtraData(void*& data, uint32_t& size) {
	if (IMedia::GetExtraData(data, size))return true;
	return false;
}
char* MediaPcm::Print(char* ptr, char* end) const {
	ptr = MediaAudio::Print(ptr, end);
	return snprintf(ptr, end, ",%u/%u", _sampleBits, _sampleWidth);
}
