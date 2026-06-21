#include "Source.h"
#include "smf_debug.h"
using namespace smf;
Source::Source(){
	_flags.Set(IS_Source);
	memset(&_sinfo, 0, sizeof(_sinfo));
	_sinfo.forward.audio.volume = 32768;
	_volume.mute = false;
	_volume.volume = 32768;
}
//Source::~Source() {
//}
bool Source::open(void* para) {
	auto media = oMedia();
	uint32_t ext32 = 0;
	if (media) {
		_sinfo.forward.is_audio = media->IsAudio();
		_sinfo.forward.is_video = media->IsVideo();
		if (_sinfo.forward.is_audio) {
			ext32 = 0x80000000 | (_sinfo.forward.audio.mute << 16) | _sinfo.forward.audio.volume;
		}
	}
	SendMessage("media", smf_direction_upward, (uint32_t)oMedia(), (uint32_t)&_sinfo, (uint32_t)0, (uint32_t)ext32);
	return Element::open(para);
}

bool Source::Schedule() {
	CleanError();
	Frame* frm = _oport->InitFrame();
	if (!processError(0, frm)) {
		return false;
	}
	//
	{
		auto rst = true;
		{
			MeasureCheck mc(this);
			rst = generateFrame(frm);
		}
		if (!rst && !processError(0, frm)) {
			return false;
		}
	}
	if (!_oport->Push(frm)) {
		if (!processError(0,frm)) {
			return false;
		}
	}
	return true;
}

bool Source::run(void*frm) {
	return Push((Frame*)frm);
}

bool Source::set(uint32_t key, void* val) {
	switch (key) {
		//shared info
	case Hash("vol"):
	case Hash("volume"): _volume.volume = _sinfo.forward.audio.volume = (uint32_t)val; dbgExtPXL(1,"%s,volume=%u", Name(), _sinfo.forward.audio.volume); return true;
	case Hash("mute"): _volume.mute = _sinfo.forward.audio.mute = (bool)val; dbgExtPXL(1, "%s,mute=%u", Name(), _sinfo.forward.audio.mute); return true;
	case Hash("muteForce"): _sinfo.forward.audio.mute = val ? true : _volume.mute; dbgExtPXL(1, "%s,muteForce=%u", Name(), _sinfo.forward.audio.mute); return true;
	case Hash("volumeReduce"): _sinfo.forward.audio.volume = val ? (_volume.volume * 2 / 10) : _volume.volume; dbgExtPXL(1, "%s,volumeReduce=%u", Name(), _sinfo.forward.audio.volume); return true;
	//case Hash("volStep"):
	//case Hash("volumeStep"):_sinfo.forward.audio.volumeStep = (uint32_t)val; return true;
	//case Hash("volumeAndStep"):
	//	_sinfo.forward.audio.volume = (uint32_t)val & 0xffff; dbgTestPDL(_sinfo.forward.audio.volume);
	//	_sinfo.forward.audio.volumeStep = (uint32_t)val >> 16;
	//	return true;
	//case Hash("volumeWeakenOthers"):_sinfo.forward.audio.volumeWeakenOthers = (uint32_t)val < 100 ? (uint32_t)val : 100; return true;
	case Hash("prompt"): _sinfo.forward.audio.prompt = !!val; return true;
	case Hash("fade"): _sinfo.forward.audio.fade = !!val; return true;
	}
	return Element::set(key, val);
}

bool Source::get(uint32_t key, void* val) const {
	switch (key) {
	case Hash("vol"):
	case Hash("volume"): *(uint32_t*)val = _sinfo.forward.audio.volume; return true;
	}
	return Element::get(key, val);
}

Source::SharedInfo_t* Source::SharedInfo()const {
	return &_sinfo;
}
