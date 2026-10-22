#include "MediaAlac.h"
#include "SmfFCC.h"
using namespace smf;

MediaAlac::MediaAlac() {
	_codec = fcc32("alac");
	_size = sizeof(MediaAlac);
	_maxSamplePerFrame = 0;
	_maxCodedFrameSize = 0;
	_sampleBits = 16;
}
