#include "MediaOpus.h"
#include "smf_debug.h"
#include "SmfFCC.h"
#include "SmfHash.h"
using namespace smf;
MediaOpus::MediaOpus(){
	_codec = fcc32("opus");
	_size = sizeof(MediaOpus);
	_frameDMs = 0;
	_application = 0;
	_lsb_depth = 0;
	_complexity = 0;
}
static inline uint32_t opus_appFrom(const char* app) {
	switch (Hash(app)) {
	case Hash("voip"):return 2048;
	case Hash("audio"):return 2049;
	case Hash("lowdelay"):return 2051;
	}
	return 0;
}
bool MediaOpus::setX(uint32_t key, void* val) {
	switch (key) {
	case Hash("app"):
	case Hash("application"): _application = opus_appFrom((const char*)val);	return true;
	case Hash("lsb"):
	case Hash("lsb_depth"): _lsb_depth = (uint32_t)val;	return true;
	case Hash("comp"):
	case Hash("complexity"): _complexity = (uint32_t)val;	return true;
	case Hash("dms"): 
	case Hash("frameDMs"): _frameDMs = (uint32_t)val;	return true;
	}
	return MediaAudio::setX(key, val);
}

//bool MediaOpus::getX(uint32_t key, void* val) const {
//	switch (key) {
	//case Hash("app"): 
	//case Hash("application"): *(uint32_t*)val = _application);	return true;
	//case Hash("lsb"):
	//case Hash("lsb_depth"): *(uint32_t*)val = _lsb_depth;	return true;
	//case Hash("comp"):
	//case Hash("complexity"): *(uint32_t*)val = _complexity;	return true;
//	}
//	return MediaAudio::getX(key, val);
//}