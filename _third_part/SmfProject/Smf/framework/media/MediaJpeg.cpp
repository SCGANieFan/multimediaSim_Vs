#include "MediaJpeg.h"
#include "SmfFCC.h"
#include "SmfHash.h"
#include "smf_debug.h"
using namespace smf;
MediaJpeg::MediaJpeg() { 
	_codec = fcc32("jpeg");
	_size = sizeof(MediaJpeg);
	_qp = 0;
}
bool MediaJpeg::setX(uint32_t key, void* val) {
	switch (key) {
	case Hash("qp"): _qp = (uint32_t)val;	return true;
	}
	return MediaVideo::setX(key, val);
}
bool MediaJpeg::getX(uint32_t key, void* val) const {
	switch (key) {
	case Hash("qp"): *(uint32_t*)val = _qp;	return true;
	}
	return MediaVideo::getX(key, val);
}
