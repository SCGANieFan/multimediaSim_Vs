#include "MediaSsc.h"
#include "SmfFCC.h"
using namespace smf;

MediaSsc::MediaSsc() {
	_codec = fcc32("ssc"); 
	_size = sizeof(MediaSsc);
	_sbm_on_off = false;
}
