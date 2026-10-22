#include "MediaSilk.h"
#include "SmfFCC.h"
using namespace smf;

MediaSilk::MediaSilk() {
	_codec = fcc32("silk"); 
	_size = sizeof(MediaSilk);
	_complexity = 0;
	_packetLossPercentage = 0;
	_packetSize = 0;
	_useDTX = 0;
	_useInBandFEC = 0;
	_API_sampleRate = 0;
	_maxInternalSampleRate = 0;
}
