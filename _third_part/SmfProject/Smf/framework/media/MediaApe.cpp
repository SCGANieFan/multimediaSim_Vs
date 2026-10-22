#include "MediaApe.h"
#include "SmfFCC.h"
using namespace smf;

MediaApe::MediaApe() {
	_codec = fcc32("ape"); 
	_size = sizeof(MediaApe);
	_sampleBits = 16;
	_sampleWidth = 2;
}
