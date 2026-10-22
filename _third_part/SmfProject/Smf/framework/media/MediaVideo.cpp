#include "MediaVideo.h"
#include "Object.h"
#include "smf_debug.h"
#include "SmfFCC.h"
#include "SmfHash.h"
using namespace smf;
MediaVideo::MediaVideo() {
	_mflags |= IS_Video;
	_width = 0;
	_height = 0;
	_frameRate = 0;
	_frameRateDenum = 1;
	_roiX = _roiY = _roiW = _roiH = 0;
}
bool MediaVideo::setX(uint32_t key, void* val) {
	switch (key) {
	case Hash("w"):
	case Hash("width"): _width = (uint32_t)val;	return true;
	case Hash("h"):
	case Hash("height"):	_height = (uint32_t)val;	return true;
	case Hash("fr"):
	case Hash("frameRate"): _frameRate = (uint32_t)val;	return true;
	case Hash("frDenum"):
	case Hash("frameRateDenum"): _frameRateDenum = (uint32_t)val;	return true;
	case Hash("roiX"): _roiX = (uint32_t)val;	return true;
	case Hash("roiY"): _roiY = (uint32_t)val;	return true;
	case Hash("roiW"): _roiW = (uint32_t)val;	return true;
	case Hash("roiH"): _roiH = (uint32_t)val;	return true;
	}
	return IMedia::setX(key,val);
}
bool MediaVideo::getX(uint32_t key, void* val) const {
	switch (key) {
	case Hash("w"):
	case Hash("width"):*(uint32_t*)val = _width;	return true;
	case Hash("h"):
	case Hash("height"):	*(uint32_t*)val = _height;	return true;
	case Hash("fr"):
	case Hash("frameRate"): *(uint32_t*)val = _frameRate;	return true;
	case Hash("frDenum"):
	case Hash("frameRateDenum"): *(uint32_t*)val = _frameRateDenum;	return true;
	case Hash("roiX"): *(uint32_t*)val = _roiX;	return true;
	case Hash("roiY"): *(uint32_t*)val = _roiY;	return true;
	case Hash("roiW"): *(uint32_t*)val = _roiW;	return true;
	case Hash("roiH"): *(uint32_t*)val = _roiH;	return true;
	}
	return IMedia::getX(key,val);
}
char* MediaVideo::Print(char* ptr, char* end) const {
	ptr = IMedia::Print(ptr, end);
	return snprintf(ptr, end, "(%u,%u)%u/%u(%u,%u,%u,%u)"
		, _width, _height
		, _frameRate, _frameRateDenum
		, _roiX, _roiY, _roiW, _roiH
	);
}

MediaVideoRaw::MediaVideoRaw() {
	_codec = fcc32("raw");
	_size = sizeof(MediaVideoRaw);
	_format = 0;
	_pixelBits = 0;
}
bool MediaVideoRaw::setX(uint32_t key, void* val) {
	switch (key) {
	case Hash("fmt"):
	case Hash("format"): _format = (uint32_t)val;	return true;
	case Hash("pixel"):
	case Hash("pixelBits"):	_pixelBits = (uint32_t)val;	return true;
	}
	return MediaVideo::setX(key, val);
}
bool MediaVideoRaw::getX(uint32_t key, void* val) const {
	switch (key) {
	case Hash("fmt"):
	case Hash("format"):*(uint32_t*)val = _format;	return true;
	case Hash("pixel"):
	case Hash("pixelBits"):	*(uint32_t*)val = _pixelBits;	return true;
	}
	return MediaVideo::getX(key, val);
}

uint32_t MediaVideoRaw::FrameSize()const {
	return FrameSize(_width, _height, _format);
}
uint32_t MediaVideoRaw::FrameSize(uint16_t w, uint16_t h, uint8_t fmt) {
	switch (fmt) {
	case i420p16:
	case i420p16s:return w * h * 3;
	case i420:
	case yv12:
	case nv12:
	case nv21:	return w * h * 3 / 2;
	case i422p16:
	case i422p16s:return w * h * 2 * 2;
	case i422:
	case yv16:
	case nv16:
	case nv61:	return w * h * 2;
	case i444p16:
	case i444p16s:return w * h * 3 * 2;
	case i444:
	case yv24:
	case nv24:
	case nv42:	return w * h * 3;
	case vyuy:
	case uyvy:
	case yuyv:
	case yvyu:	return w * h * 2;
	case pal8:	return w * h + 1024;
	case rgb565:return w * h * 2;
	case rgb888:return w * h * 3;
	case rgba8888:return w * h * 4;
	default: return 0;
	}
}
char* MediaVideoRaw::Print(char* ptr, char* end) const {
	ptr = MediaVideo::Print(ptr, end);
	return snprintf(ptr, end, "(%u,%u)"
		, _format, _pixelBits
	);
}
MediaYuv::MediaYuv() {
	_codec = fcc32("yuv");
	_size = sizeof(MediaYuv);
}
bool MediaYuv::setX(uint32_t key, void* val) {
	//switch (key) {
	//}
	return MediaVideoRaw::setX(key, val);
}
bool MediaYuv::getX(uint32_t key, void* val) const {
	//switch (key) {
	//}
	return MediaVideoRaw::getX(key, val);
}

MediaRgb::MediaRgb() {
	_codec = fcc32("rgb");
	_size = sizeof(MediaRgb);
	_paletteCount = 0;
	_palette = 0;
}
bool MediaRgb::setX(uint32_t key, void* val) {
	switch (key) {
	case Hash("palette"):	_palette = (rgb888_t*)val;	return true;
	case Hash("paletteCount"):	_paletteCount = (uint32_t)val;	return true;
	}
	return MediaVideoRaw::setX(key, val);
}
bool MediaRgb::getX(uint32_t key, void* val) const {
	switch (key) {
	case Hash("palette"):	*(rgb888_t**)val = _palette;	return true;
	case Hash("paletteCount"):	*(uint32_t*)val = _paletteCount;	return true;
	}
	return MediaVideoRaw::getX(key, val);
}
char* MediaRgb::Print(char* ptr, char* end) const {
	ptr = MediaVideoRaw::Print(ptr, end);
	return snprintf(ptr, end, "(%u,%p)"
		, _paletteCount, _palette
	);
}