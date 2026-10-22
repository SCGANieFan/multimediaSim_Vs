#include "MediaADPcm.h"
#include "SmfFCC.h"
using namespace smf;
MediaADPcm::MediaADPcm() { 
	Codec(fcc64("adpcm"));
	_size = sizeof(MediaADPcm);
}
