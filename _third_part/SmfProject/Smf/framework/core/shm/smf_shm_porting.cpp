#include "IMedia.h"
#include "SmfPorting.h"

using namespace smf;

EXTERNC void smf_media_update_vtable(smf_media_def_t* media) { if (media)((IMedia*)media)->UpdateVTable();}
EXTERNC void cache_flush(void* data, uint32_t size) { smf::cache_flush(data, size); }
EXTERNC void cache_invalid(void* data, uint32_t size) { smf::cache_invalid(data, size); }
EXTERNC void sleep_for(uint32_t ms) { smf::sleep_for(ms); }
