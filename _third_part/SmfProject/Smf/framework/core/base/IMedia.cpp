#include "IMedia.h"
#include "smf_debug.h"
#include "SmfFCC.h"
#include "SmfHash.h"
#include "Element.h"
#include "MediaAudio.h"
#include "MediaVideo.h"
using namespace smf;
IMedia::IMedia() {
	_size = sizeof(IMedia);
	_codec = fcc32("user");//not to use uint64_t
	_codec_high = 0;
	_mflags = 0;
	_extraSize = 0;
	_extraData = 0;
	_bitrate = 0;
}
IMedia::~IMedia() {
	
}
bool IMedia::Is(const char* codec)const { return Is(fcc64(codec)); }
bool IMedia::Is(uint64_t codec)const { return *(uint64_t*)&_codec == codec; }
bool IMedia::IsAudio()const{return _mflags &IS_Audio;}
bool IMedia::IsVideo()const{return _mflags &IS_Video;}
bool IMedia::IsOther()const{return _mflags &IS_Other;}
bool IMedia::IsError()const { return _mflags &IS_Error; }
void IMedia::SetError() { _mflags |= IS_Error; }
smf_media_def_t* IMedia::ToMedia() const { return (smf_media_def_t*)this; }
uint64_t IMedia::Codec()const { return *(uint64_t*)&_codec; }
uint64_t IMedia::CodecType() const { return CodecType(Codec()); }
uint32_t IMedia::CodecTypeHash() const { return CodecTypeHash(Codec()); }
const char* IMedia::CodecString()const { return (const char*)&_codec; }
void IMedia::Codec(uint64_t codec) { *(uint64_t*)&_codec = codec; }
void IMedia::Codec(const char* codec) { *(uint64_t*)&_codec = fcc64(codec); }
void IMedia::InitMedia(Element* ele) { ele->oPort()->Set(this); }
bool IMedia::FromMedia(IMedia*media, bool check) {
	return FromMedia((smf_media_def_t*)media, check);
}
bool IMedia::FromMedia(smf_media_def_t* media, bool check) {
	if (check) {
		returnIfErrC(false, _codec != media->_codec);
		returnIfErrCS(false, _size != media->_size, "%d,%d", _size, media->_size);
	}
	auto offset = 16;
	auto size = _size ? _size : media->_size;
	memcpy((char*)this + offset, (char*)media + offset, size - offset);
	dbgTestPXL("update success");
	return true;
}
IMedia* IMedia::Clone() const {
	auto media = Create(Codec());
	returnIfErrC(0, !media);
	memcpy((char*)media + 16, (char*)this + 16, media->_size - 16);
	return media;
}
bool IMedia::UpdateVTable() {
	auto media = Create(Codec());
	returnIfErrC(false, !media);
	//dbgTestPXL("%p",this);
	//dbgTestPPL((void*)this);
	//dbgTestPPL((void*)media);
	memcpy((void*)this, (void*)media, 4);
	delete media;
	return true;
}
bool IMedia::SetX(const char* keys, void* vals) { return setX(Hash(keys), vals); }
bool IMedia::SetX(uint32_t keys, void* vals) { 
	auto rst = setX(keys, vals);
	if (rst) {
		MediaChange(true);
	}
	return rst;
}
bool IMedia::GetX(const char* keys, void* vals) const { return getX(Hash(keys), vals); }
bool IMedia::GetX(uint32_t keys, void* vals) const { return getX((keys), vals);}
bool IMedia::setX(uint32_t key, void* val) {
	switch (key) {
	case Hash("codec"):Codec((const char*)val); return true;
	case Hash("extraSize"):_extraSize = ((uint32_t)val); return true;
	case Hash("extraData"):_extraData = ((uint8_t*)val); return true;
	case Hash("br"):
	case Hash("bitrate"): _bitrate = (uint32_t)val;	return true;
	case Hash("fromMedia"): FromMedia((smf_media_def_t*)val, true);	return true;
	}
	return false;
}
bool IMedia::getX(uint32_t key, void* val) const {
	switch (key) {
	case Hash("br"):
	case Hash("bitrate"): *(uint32_t*)val = _bitrate;	return true;
	}
	return false;
}
uint64_t IMedia::convertByteToUs(uint32_t size)const{
	auto bps = BytePerSecond();
	//dbgTestPXL("%s,%d,%d",CodecString(),bps.num, bps.den);
	if (bps.num) {
		return size * 1000000ull * bps.den / bps.num;
	}
	else {
		// dbgTestPXL("%s,%d,%d", CodecString(), bps.num, bps.den);
	}
	return 0;
}

uint32_t IMedia::convertUsToByte(uint64_t us)const {
	auto bps = BytePerSecond();
	auto den = bps.den ? bps.den : 1;
	auto size = (us * bps.num) / (den * 1000000ull);
	auto align = SizeAlign();
	if (align) {
		size = size / align * align;
	}
	return size;
}
uint32_t IMedia::Bitrate()const {
	auto bps = BytePerSecond();
	return (bps.num << 3) / bps.den;
}
void IMedia::MediaChange(bool val) { val ? (_mflags |= IS_Changed) : (_mflags &= ~IS_Changed); }
bool IMedia::MediaChanged()const { return _mflags & IS_Changed; }

char* IMedia::Print(char* ptr, char* end) const {
	return snprintf(ptr,end, "media@%s,%u,%08x", CodecString(), _size, _mflags);
}
bool IMedia::SetExtraData(void* data, uint32_t size) {
	_extraData = (uint8_t*)data;
	_extraSize = size;
	return true;
}
bool IMedia::GetExtraData(void*& data, uint32_t& size) {
	if (_extraData && _extraSize) {
		data = _extraData;
		size = _extraSize;
		return true;
	}
	return false;
}
