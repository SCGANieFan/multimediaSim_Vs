#include "MediaSbc.h"
#include "smf_debug.h"
#include "SmfFCC.h"
#include "SmfHash.h"
using namespace smf;
MediaSbc::MediaSbc() {
	_codec = fcc32("sbc");
	_size = sizeof(MediaSbc);
	_syncWord = 0;
	_channelMode = 0;
	_numSubBands = 0;
	_numBlocks = 0;
	_allocMethod = 0;
	_bitPool = 0;
	_sbcMode = 0;
}
bool MediaSbc::setX(uint32_t key, void* val) {
	switch (key) {
	case Hash("chMode"): _channelMode = (uint32_t)val; return true;
	case Hash("bands"): _numSubBands = (uint32_t)val; return true;
	case Hash("blks"): _numBlocks = (uint32_t)val; return true;
	case Hash("allocM"): _allocMethod = (uint32_t)val; return true;
	case Hash("bitPool"): _bitPool = (uint32_t)val; return true;
	}
	return MediaAudio::setX(key, val);
}

MediaMsbc::MediaMsbc() {
	_codec = fcc32("msbc");
	_rate = 16000;
	_channels = 1;
	_syncWord = SMF_SBC_SYNCWORDS_MSBC;
	_channelMode = SMF_SBC_CHNL_MODE_MONO;
	_numSubBands = 8;
	_numBlocks = 15;
	_allocMethod = SMF_SBC_ALLOC_METHOD_LOUDNESS;
	_bitPool = 26;
	_sbcMode = SMF_SBC_MODE_MSBC;
}
