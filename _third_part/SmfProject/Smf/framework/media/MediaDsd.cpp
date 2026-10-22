#include "MediaDsd.h"
#include "smf_debug.h"
#include "SmfFCC.h"
#include "SmfHash.h"
using namespace smf;

MediaDsd::MediaDsd(){ 
	_codec = fcc32("dsd");
	_size = sizeof(MediaDsd);
	_rate = 44100 * 64;
}
