#include "MediaFFmpeg.h"
#include "SmfFCC.h"
#include "SmfHash.h"
#include "smf_debug.h"
using namespace smf;
MediaFFmpeg::MediaFFmpeg() { 
	_codec = fcc32("fpeg");
	_size = sizeof(MediaFFmpeg);
	_vbr = 0;
	_profile = 0;
	_gop = 0;
}
bool MediaFFmpeg::setX(uint32_t key, void* val) {
	switch (key) {
	case Hash("vbr"): _vbr = (uint32_t)val;	return true;
	case Hash("profile"): _profile = (uint32_t)val;	return true;
	case Hash("format"): _format = (uint32_t)val;	return true;
	case Hash("gop"): _gop = (uint32_t)val;	return true;
	}
	return MediaVideo::setX(key, val);
}
bool MediaFFmpeg::getX(uint32_t key, void* val) const {
	switch (key) {
	case Hash("vbr"): *(uint32_t*)val = _vbr;	return true;
	case Hash("profile"): *(uint32_t*)val = _profile;	return true;
	case Hash("format"): *(uint32_t*)val = _format;	return true;
	case Hash("gop"): *(uint32_t*)val = _gop;	return true;
	}
	return MediaVideo::getX(key, val);
}
