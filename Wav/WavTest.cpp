#include"MTF.h"
using namespace MTFApi_ns;
#define PATH "../../source/audio/wav/"

#define FILE_NAME "chirp_sin_16k1ch.wav"
#define RATE 16000
#define CHANNEL 1
#define WIDTH 2
#define FRAME_MS 20



void WavTest()
{
	MTFApi::Init();
	//MultiemdiaTestInit();

	MTF_REGISTER(wav_demuxer);
	MTF_REGISTER(wav_muxer);

	void* param[] = {
		(void*)(PATH FILE_NAME),
		(void*)(PATH FILE_NAME ".wavtest.wav"),
		(void*)(RATE * FRAME_MS / 1000),
	};

	const char* str = {
	"|wav_demuxer,url=$0,fSamples=$2|-->"
	"|wav_muxer,url=$1|"
	};

	//MultiemdiaApi(str, param);
	MTFApi::Api(str, param);
}
