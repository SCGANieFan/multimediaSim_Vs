#include"MTF.h"
using namespace MTFApi_ns;

#define PATH "../../source/audio/wav/"

//#define FILE_NAME "chirp_sin_16k1ch.wav"
#define FILE_NAME "nf_16k1ch.pcm"
#define RATE 16000
#define CHANNEL 1
#define WIDTH 2
#define FRAME_MS 20

using namespace MTFApi_ns;

void WavTest()
{
	MTFApi::Init();

	MTF_REGISTER(wav_demuxer);
	MTF_REGISTER(pcm_demuxer);
	MTF_REGISTER(wav_muxer);

	void* param[] = {
		(void*)(PATH FILE_NAME),
		(void*)(PATH FILE_NAME ".wavtest.wav"),
		(void*)(RATE * FRAME_MS / 1000),
		(void*)(RATE),
		(void*)(CHANNEL),
		(void*)(WIDTH),
	};
#if 0
	const char* str = {
	"|wav_demuxer,url=$0,fSamples=$2|-->"
	"|wav_muxer,url=$1|"
	};
#else
	const char* str = {
	"|pcm_demuxer,url=$0,fSamples=$2,rate=$3,ch=$4,width=$5|-->"
	"|wav_muxer,url=$1|"
	};
#endif
	MTFApi::Api(str, param);
}
