#include"MTF.h"
using namespace MTFApi_ns;

#define PATH "../../source/container/ogg/"
//#define FILE_NAME "test.opusx"
#define FILE_NAME "mbz_48k2h_40s.opusx"
//#define FILE_NAME "mbz_48k2h_40s.opusx"
//#define FILE_NAME "test48k2ch.ogg"
//#define FILE_NAME "mbz_48k2h.ogg"

#define RATE 48000
#define CHANNEL 2
#define WIDTH 2

using namespace MTFApi_ns;


static void OggMtfTest() {
	MTFApi::Init();
	MTF_REGISTER(opus_demuxer);
	MTF_REGISTER(opus_muxer);
	MTF_REGISTER(ogg_demuxer);
	MTF_REGISTER(ogg_muxer);
#if 1
	void* param[] = {
		(void*)(PATH FILE_NAME),
		(void*)(PATH FILE_NAME ".ogg"),
		(void*)(2048),
		(void*)(RATE),
		(void*)(CHANNEL),
		(void*)(WIDTH),
	};
	//, url = $0, fSamples = $2
	const char* str = {
	"|opus_demuxer,url=$0,rate=$3,ch=$4,witdh=$5|-->"
	"|ogg_muxer,url=$1,pagebyte=$2|"
	};
#else
	void* param[] = {
	(void*)(PATH FILE_NAME),
	(void*)(PATH FILE_NAME ".ogg"),
	(void*)(2048),
	(void*)(RATE),
	(void*)(CHANNEL),
	(void*)(WIDTH),
	};
	//, url = $0, fSamples = $2
	const char* str = {
	"|ogg_demuxer,url=$0,rate=$3,ch=$4,witdh=$5|-->"
	"|ogg_muxer,url=$1,pagebyte=$2|"
	};
#endif
	MTFApi::Api(str, param);
}
extern "C" void OggDemoMuxTest();
static void OggGadfTest() {
	OggDemoMuxTest();
}

void OggTest()
{
	//OggMtfTest();
	OggGadfTest();
}



