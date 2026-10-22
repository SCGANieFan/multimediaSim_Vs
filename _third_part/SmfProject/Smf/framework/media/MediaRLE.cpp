#include "MediaRLE.h"
#include "SmfFCC.h"

using namespace smf;

MediaRLE::MediaRLE() { 
	_codec = fcc32("rle");
	_size = sizeof(MediaRLE);
}
bool MediaRLE::setX(uint32_t key, void* val) {
	return MediaVideo::setX(key, val);
}
bool MediaRLE::getX(uint32_t key, void* val) const {
	return MediaVideo::getX(key, val);
}
