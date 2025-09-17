#include"MTF.h"
#include <stdio.h>
using namespace MTFApi_ns;
#define PATH "../../source/audio/ape/"

#if 0
#define FILE_NAME "Flower_Dance_44p1k2ch.wav.apeOri"
#define RATE 44100
#define CHANNEL 2
#define WIDTH 2
#define FRAME_MS 20
#endif

#if 0
#define FILE_NAME "Inuyasha_48k2ch.wav.apeOri"
#define RATE 48000
#define CHANNEL 2
#define WIDTH 2
#define FRAME_MS 20
#endif


char filenames[][50] = {
#if 0
	"demo1_48k2ch.ape",
	"demo2_44p1k2ch.ape",
	"demo3_96K2ch.ape",
	"demo4_48k2ch.ape",
	"demo5_48k1ch.ape",
	"demo6_48k1ch8b.ape",
	"demo2_44p1k2ch.ape",
	"Flower_Dance_44p1k2ch.ape",
	"Inuyasha_48k2ch.ape",
#else
	//"demo6_48k1ch8b.ape",
	"Flower_Dance_44p1k2ch.ape",
	//"demo5_48k1ch.ape",
#endif
};


void ApeTest()
{
	MTFApi::Init();

	MTF_REGISTER(ape_demux);
	MTF_REGISTER(ape_dec);
	MTF_REGISTER(wav_muxer);

	char fNameIn[256];
	char fNameOut[256];
	for (int i = 0; i < sizeof(filenames) / sizeof(filenames[0]);i++)
	{
		sprintf(fNameIn, "%s%s", PATH, filenames[i]);
		sprintf(fNameOut, "%s.wav", fNameIn);

		void* param[] = {
			(void*)(fNameIn),
			(void*)(fNameOut),
		};

		const char* str = {
		"|ape_demux,url=$0|-->"
		"|ape_dec|-->"
		"|wav_muxer,url=$1|"
		};
		MTFApi::Api(str, param);
	}
}
