#include "MediaCodec2.h"
#include "SmfFCC.h"
using namespace smf;

MediaCodec2::MediaCodec2() {
	_codec = fcc32("codec2");
	_size = sizeof(MediaCodec2);
}
