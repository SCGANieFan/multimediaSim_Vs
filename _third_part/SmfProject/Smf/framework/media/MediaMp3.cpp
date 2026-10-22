#include "MediaMp3.h"
#include "smf_debug.h"
#include "SmfFCC.h"
#include "SmfHash.h"
using namespace smf;

MediaMp3::MediaMp3(){ 
	_codec = fcc32("mp3");
	_size = sizeof(MediaMp3);
	_layer = 0;
}

//uint32_t MediaMp3::FrameMax() const {
//	// NOTE: buffer size is too large.
//	return _rate * _channels;
//}
