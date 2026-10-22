#include "MediaPng.h"
#include "SmfFCC.h"

using namespace smf;

MediaPng::MediaPng() { 
	_codec = fcc32("png");
	_size = sizeof(MediaPng);
}
bool MediaPng::setX(uint32_t key, void* val) {
	return MediaVideo::setX(key, val);
}
bool MediaPng::getX(uint32_t key, void* val) const {
	return MediaVideo::getX(key, val);
}
