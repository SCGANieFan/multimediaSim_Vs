#include"MTF.h"
using namespace MTFApi_ns;

#define PATH "../../source/audio/plc/"
#if 0
#if 0
//#define FILE_NAME "chirp_sin_16k1ch.wav"
#define FILE_NAME "stSection_16k1ch.wav"
#define RATE 16000
#define CHANNEL 1
#define WIDTH 2
#define FRAME_MS 10
#define DECAY_MS 20
#define OVERLAP_MS 5 //OVERLAP_MS < FRANME_MS,
#else
#define FILE_NAME "sin4ch48k16b.wav"
#define RATE 48000
#define CHANNEL 4
#define WIDTH 2
#define FRAME_MS 10
#define DECAY_MS 30
#define GAIN_MS 10
#define OVERLAP_MS 4
#endif

#else
#if 0
#define FILE_NAME "sin1k_48k1ch24b.pcm"
#define RATE 48000
#define CHANNEL 1
#define WIDTH 3
#define FRAME_MS 2
#define DECAY_MS 50
#define OVERLAP_MS 2
#else

//#define FILE_NAME "mbz_48k2h.wav"
//#define FILE_NAME "sin4ch48k16b.wav"
//#define FILE_NAME "sin4ch48k32b.wav"
//#define FILE_NAME "sin1ch48k32b_1.wav"
//#define FILE_NAME "sin1ch48k32b_2.wav"
//#define FILE_NAME "sin1ch48k32b_3.wav"
//#define FILE_NAME "sin1ch48k32b_4.wav"
//#define FILE_NAME "sin4ch48k32bFloat.wav"
//#define FILE_NAME "Walking01_Burn(1).wav"
//#define FILE_NAME "30hz_48k2ch.wav"
//#define FILE_NAME "sin1k48k2ch16b.wav"
//#define FILE_NAME "stSection_48k1ch.wav"
//#define FILE_NAME "Flower Dance_44.1k2chF32.wav"
//#define FILE_NAME "Inuyasha_48k2ch_40sF32.wav"
//#define FILE_NAME "lfe01_48k2ch.wav"
//#define FILE_NAME "lfe02_48k2ch.wav"
//#define FILE_NAME "lfe03_48k2ch.wav"
//#define FILE_NAME "lfe04_48k2ch.wav"
//#define FILE_NAME "sin1050hz16k2ch.wav"
 //#define FILE_NAME "sin1050hz48k2ch32b.wav"
 #define FILE_NAME "plc_test_gx_32k1ch.wav"

#define RATE 32000
//#define RATE 96000
#define FRAME_MS 20
#endif
#endif

#define FRAME_LEN ((FRAME_MS*RATE/1000))
//#define FRAME_LEN (240)

void PlcTest()
{
	MTFApi::Init();

	MTF_REGISTER(wav_demuxer);
	MTF_REGISTER(music_plc);
	MTF_REGISTER(wav_muxer);

	
#if 0
	void* param[] = {
	(void*)(PATH FILE_NAME),
	(void*)(PATH FILE_NAME ".plc.wav"),
	(void*)(FRAME_MS),
	};
	const char* str = {
	"|wav_demuxer,url=$0,fMs=$2|-->"
	"|music_plc|-->"
	"|wav_muxer,url=$1|"
	};

#else
	void* param[] = {
		(void*)(PATH FILE_NAME),
		(void*)(PATH FILE_NAME ".plc.wav"),
		(void*)(FRAME_LEN),
	};
	const char* str = {
	"|wav_demuxer,url=$0,fSamples=$2|-->"
	"|music_plc|-->"
	"|wav_muxer,url=$1|"
	};
#endif
	MTFApi::Api(str, param);
}
