#include "MediaMp3.h"
#include "Object.h"
#include "smf_debug.h"
#include "SmfFCC.h"
#include "SmfHash.h"
using namespace smf;
#define dbgCheckEquel(s,a,b)
#if 0
#define dbgCheckEquel(s,a,b) \
if((a)!=(b)){\
	dbgErrPXL("[fail]"#a"("s")""<>"#b"("s")",a,b);\
}\
else{\
	dbgTestPXL("[ok]"#a"=="#b"("s")", a);\
}\

#endif
void smfTestMedia() {
	MediaMp3 mp3;
	mp3._rate = 48000;
	mp3._channels = 2;
	//mp3._bandWidth = 22000;
	auto mp30 = &mp3;
	auto mp31 = (smf_media_mp3_t*)mp3.ToMedia();
	dbgTestDump(mp30, sizeof(*mp30));
	dbgTestDump(mp31, sizeof(*mp31));
	//dbgCheckEquel("%p", mp30, mp31); 
	//dbgTestPXL("%p,%p", mp30, mp31);
	//dbgTestPXL("codec:%p/%p,%d/%d", &mp30->_codec, &mp31->head._codec, mp30->_codec, mp31->head._codec);
	//dbgTestPXL("rate:%p/%p,%d/%d", &mp30->_rate, &mp31->audio._rate, mp30->_rate, mp31->audio._rate);
	//dbgTestPXL("ch:%p/%p,%d/%d", &mp30->_channels, &mp31->audio._channels, mp30->_channels, mp31->audio._channels);
	//dbgTestPXL("bandWidth:%p/%p,%d/%d", &mp30->_bandWidth, &mp31->mp3._bandWidth, mp30->_bandWidth, mp31->mp3._bandWidth);

	dbgCheckEquel("%d", sizeof(IMedia), sizeof(smf_media_def_t));
	dbgCheckEquel("%d", sizeof(MediaAudio), sizeof(smf_media_def_t)+sizeof(smf_media_audio_def_t));
	dbgCheckEquel("%d", sizeof(MediaMp3), sizeof(smf_media_def_t) + sizeof(smf_media_audio_def_t)+ sizeof(smf_media_mp3_def_t));
	dbgCheckEquel("%d", sizeof(MediaMp3), sizeof(smf_media_mp3_t));

	dbgCheckEquel("%p", (void*)&mp30->_codec, (void*)&mp31->head._codec);
	dbgCheckEquel("%p", &mp30->_rate, &mp31->audio._rate);	
	//dbgCheckEquel("%p", &mp30->_bandWidth, &mp31->mp3._bandWidth);

#if 0
	dbgTestPDL(mp30->_rate);
	dbgTestPDL(mp31->audio._rate);
	dbgTestPDL(mp30->_channels);
	dbgTestPDL(mp31->audio._channels);
	dbgTestPDL(mp30->_bandWidth);
	dbgTestPDL(mp31->audio.mp3.bandWidth);
	dbgTestPSL(&mp30->_codec);
	dbgTestPSL(&mp31->_codec);
	dbgTestPPL(mp30);
	dbgTestPPL(mp31);
	dbgTestPPL(&mp30->_codec);
	dbgTestPPL(&mp31->_codec);
	dbgTestPPL(&mp30->_mflags);
	dbgTestPPL(&mp31->_mflags);
	dbgTestPPL(&mp30->_ext);
	dbgTestPPL(&mp31->_ext);
	dbgTestPPL(&mp30->_extraSize);
	dbgTestPPL(&mp31->_extraSize);
	dbgTestPPL(&mp30->_extraData);
	dbgTestPPL(&mp31->_extraData);
	dbgTestPPL(&mp30->_frameSamples);
	dbgTestPPL(&mp31->audio._frameSamples);
	dbgTestPPL(&mp30->_bandWidth);
	dbgTestPPL(&mp31->audio.mp3.bandWidth);
	dbgTestPPL(&mp30->_bitrate);
	dbgTestPPL(&mp31->audio.mp3.bitrate);
	dbgTestPDL(sizeof(IMedia));
	dbgTestPDL(sizeof(smf_media_t));
	dbgTestPDL(sizeof(mp30));
	dbgTestPDL(sizeof(mp31));

	dbgTestDump(mp30,32);
	dbgTestDump(mp31,32);
#endif
}
