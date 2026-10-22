#include "MediaCvsd.h"
#include "SmfFCC.h"
using namespace smf;

MediaCvsd::MediaCvsd() {
	_codec = fcc32("cvsd"); 
	_size = sizeof(MediaCvsd);
	_rate = 8000;
	_channels = 1;
}
