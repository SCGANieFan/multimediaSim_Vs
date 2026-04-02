#include"MTF.h"
using namespace MTFApi_ns;

#define PATH "../../source/audio/opus/"
//#define FILE_NAME "mbz_48k2h.wav"
//#define FILE_NAME "stSection_48k1ch.wav"
//#define FILE_NAME "chirp_sin_48k1ch.wav"
//#define FILE_NAME "mbz_48k1h_40s.wav"
#define FILE_NAME "chirp_sin_48k1ch.wav"
#define FRAME_MS 5
#define BIT_RATE 179200
#define COMPLEXITY 0
#define VBR 0

static void OpusMtfTest()
{
	MTFApi::Init();
	MTF_REGISTER(wav_demuxer);
	MTF_REGISTER(opus_enc);
	MTF_REGISTER(opus_dec);
	MTF_REGISTER(wav_muxer);
	MTF_REGISTER(pcm_muxer);

	void* param[] = {
		(void*)(PATH FILE_NAME),
		(void*)(PATH FILE_NAME ".opusx"),
		(void*)(FRAME_MS),
		(void*)(BIT_RATE),
		(void*)(COMPLEXITY),
		(void*)(VBR),
		(void*)(PATH FILE_NAME ".opusx.wav"),
	};
#if 1
	const char* str = {
	"|wav_demuxer,url=$0,fMs=$2|-->"
	"|opus_enc,bitrate=$3,cpmplexity=$4,vbr=$5|-->"
	"|opus_dec|-->"
	"|wav_muxer,url=$6|"
	};
#endif
#if 0
	const char* str = {
	"|wav_demuxer,url=$0,fMs=$2|-->"
	"|opus_enc,bitrate=$3,cpmplexity=$4,vbr=$5|-->"
	"|pcm_muxer,url=$1|"
	};
#endif
#if 0
	const char* str = {
	"|opus_demuxer,url=$0,fMs=$2|-->"
	"|opus_enc,bitrate=$3,cpmplexity=$4,vbr=$5|-->"
	"|pcm_muxer,url=$1|"
	};
#endif
	MTFApi::Api(str, param);
}

#ifndef EXTERNC
#ifdef __cplusplus
#define EXTERNC extern "C" 
#else
#define EXTERNC
#endif
#endif

EXTERNC void OpusCodecTest();
static void OpusDemoTest()
{
	OpusCodecTest();
}


void OpusTest()
{
	OpusMtfTest();
	//OpusDemoTest();
}