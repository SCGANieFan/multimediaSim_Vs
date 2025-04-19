#include<stdio.h>
#include"MTF.h"
using namespace MTFApi_ns;
#define PATH "../../source/audio/speedCtr/"

#if 0
//16k1ch
//#define FILE_NAME "chirp_sin_16k1ch.wav"
#define FILE_NAME "Female1_1st_16k1ch.wav"
//#define FILE_NAME "Female1_1st_16k1ch.wav"
//#define FILE_NAME "stSection_16k1ch.wav"
#define RATE 16000
#define CHANNEL 1
#define WIDTH 2
#endif

#if 0
//16k2ch
#define FILE_NAME "man_track_16k_2ch.wav"
#define RATE 16000
#define CHANNEL 2
#define WIDTH 2
#endif


//48k2ch
//#define FILE_NAME "mbz_48k2h.wav"
#define FILE_NAME "stSection_16k1ch.wav"
//#define FILE_NAME "sin5k48k2ch.wav"

#if 0
#define RATE 48000
#define CHANNEL 2
#define WIDTH 2
#endif


#define FRAME_MS 20
//#define SPEED (2.0f)
//#define SPEED (1.5f)
#define SPEED (0.7f)
#define TO_STR0(name) #name
#define TO_STR(name) TO_STR0(name)


#define FILE_OUT_NAME FILE_NAME ".speed" TO_STR(SPEED) ".wav"

void AudioSpeedCtrTest()
{
	MTFApi::Init();
	MTF_REGISTER(auio_speedCtr);
	//MTF_REGISTER(pcm_demuxer);
	//MTF_REGISTER(pcm_muxer);
	MTF_REGISTER(wav_demuxer);
	MTF_REGISTER(wav_muxer);
	uint32_t speedQ8 = (uint32_t)(SPEED * (1 << 8));
	void* param[] = {
		(void*)(PATH FILE_NAME),
		(void*)(PATH FILE_OUT_NAME),
		(void*)FRAME_MS,
		(void*)speedQ8,
	};

	const char* str = {
	"|wav_demuxer,url=$0,fMs=$2|-->"
	"|auio_speedCtr,speedQ8=$3|-->"
	"|wav_muxer,url=$1|"
	};

	MTFApi::Api(str, param);
}
