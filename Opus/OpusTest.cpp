#include"MTF.h"
#define PATH "../../source/audio/opus/"
#define FILE_NAME "mbz_48k2h.wav"
#define FRAME_MS 20
#define BIT_RATE 20000
#define COMPLEXITY 0
#define VBR 0

static void OpusMtfTest()
{
	MultiemdiaTestInit();

	MTF_REGISTER(wav_demuxer);
	MTF_REGISTER(opus_enc);
	MTF_REGISTER(opus_dec);
	MTF_REGISTER(wav_muxer);

	void* param[] = {
		(void*)(PATH FILE_NAME),
		(void*)(PATH FILE_NAME ".opusx.wav"),
		(void*)(FRAME_MS),
		(void*)(BIT_RATE),
		(void*)(COMPLEXITY),
		(void*)(VBR),
	};

	const char* str = {
	"|wav_demuxer,url=$0,fMs=$2|-->"
	"|opus_enc,bitrate=$3,cpmplexity=$4,vbr=$5|-->"
	"|opus_dec|-->"
	"|wav_muxer,url=$1|"
	};

	MultiemdiaApi(str, param);
}

static void OpusDemoTest()
{
	extern void OpusCodecTest(); OpusCodecTest();
}


void OpusTest()
{
	//OpusMtfTest();
	OpusDemoTest();
}