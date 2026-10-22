#include "MediaGif.h"
#include "SmfFCC.h"

using namespace smf;

MediaGif::MediaGif() { 
	_codec = fcc32("gif");
	_size = sizeof(MediaGif);
}
bool MediaGif::setX(uint32_t key, void* val) {
	return MediaVideo::setX(key, val);
}
bool MediaGif::getX(uint32_t key, void* val) const {
	return MediaVideo::getX(key, val);
}
