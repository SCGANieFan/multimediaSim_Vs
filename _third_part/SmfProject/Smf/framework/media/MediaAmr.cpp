#include "MediaAmr.h"
#include "SmfFCC.h"
using namespace smf;

MediaAmr::MediaAmr() {
	_codec = fcc32("amr"); 
	_size = sizeof(MediaAmr);
}
MediaAmr::MediaAmr(uint64_t codec) {
	Codec(codec); 
	_size = sizeof(MediaAmr);
}
