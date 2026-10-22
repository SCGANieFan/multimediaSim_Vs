#include "MediaVorbis.h"
#include "SmfFCC.h"
using namespace smf;

MediaVorbis::MediaVorbis() {
	Codec(fcc64("vorbis"));
	_size = sizeof(MediaVorbis);
	_complexity = 0;
	_encmode = 0;
	_vbr = 0;
}
