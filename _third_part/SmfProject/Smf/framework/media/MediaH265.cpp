#include "MediaH265.h"
#include "SmfFCC.h"
// #include "SmfHash.h"
// #include "smf_debug.h"

using namespace smf;

MediaH265::MediaH265() { 
	_codec = fcc32("h265");
	_size = sizeof(MediaH265);
}
bool MediaH265::setX(uint32_t key, void* val) {
	return MediaVideo::setX(key, val);
}
bool MediaH265::getX(uint32_t key, void* val) const {
	return MediaVideo::getX(key, val);
}
