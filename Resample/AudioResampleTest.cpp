#include"MTF.h"
#define PATH "../../source/audio/resample/"

#if 1
#define FILE_NAME "chirp_sin_16k1ch.wav"
#define RATE 16000
#define CHANNEL 1
#define WIDTH 2
#define FRAME_MS 20
#define O_RATE 48000
#endif

#if 0
#define FILE_NAME "sin4ch48k16b.pcm"
#define RATE 48000
#define CHANNEL 4
#define WIDTH 2
#define FRAME_MS 20
#define O_RATE 24999
#endif

#if 0
#define FILE_NAME "sin4ch48k32b.pcm"
#define RATE 48000
#define CHANNEL 4
#define WIDTH 4
#define FRAME_MS 20
#define O_RATE 33000
#endif

//#define O_RATE 32000
//#define O_RATE 8000

void AudioResampleTest()
{
	MultiemdiaTestInit();

	MTF_REGISTER(pcm_demuxer);
	MTF_REGISTER(auio_resample);
	MTF_REGISTER(pcm_muxer);

	void* param[] = {
		(void*)(PATH FILE_NAME),
		(void*)(PATH FILE_NAME ".resample48k.pcm"),
		(void*)RATE,
		(void*)CHANNEL,
		(void*)WIDTH,
		(void*)FRAME_MS,
		(void*)O_RATE,
	};

	const char* str = {
	"|pcm_demuxer,url=$0,rate=$2,ch=$3,width=$4,fMs=$5|-->"
	"|auio_resample,oFs=$6|-->"
	"|pcm_muxer,url=$1|"
	};

	MultiemdiaApi(str, param);
}