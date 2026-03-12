#include"MTF.h"
using namespace MTFApi_ns;

#define PATH "../../source/container/ogg/"
//#define FILE_NAME "test.opusx"
#define FILE_NAME "mbz_48k2h_40s.opusx"
//#define FILE_NAME "test48k2ch.ogg"
//#define FILE_NAME "mbz_48k2h.ogg"
//#define FILE_NAME "一杯陈豆浆 - 漫步人生路 [mqms2].ogg"
//#define FILE_NAME "三Z-STUDIO&HOYO-MiX - 绝不空军 [mqms2].ogg"
//#define FILE_NAME "用户提供的异常文件.ogg"

#define RATE 48000
#define CHANNEL 2
#define WIDTH 2

using namespace MTFApi_ns;


static void OggMtfTest() {
	MTFApi::Init();
	MTF_REGISTER(opus_dec);
	MTF_REGISTER(opus_demuxer);
	MTF_REGISTER(opus_muxer);
	MTF_REGISTER(ogg_demuxer);
	MTF_REGISTER(ogg_muxer);
	MTF_REGISTER(pcm_muxer);
#if 1
	void* param[] = {
		(void*)(PATH FILE_NAME),
		(void*)(PATH FILE_NAME ".ogg"),
		(void*)(2048),
	};
	//, url = $0, fSamples = $2
	const char* str = {
	"|opus_demuxer,url=$0|-->"
	"|ogg_muxer,url=$1,pagebyte=$2|"
	};
#endif
#if 0
	void* param[] = {
	(void*)(PATH FILE_NAME),
	(void*)(PATH FILE_NAME ".oggdem"),
	};
	//, url = $0, fSamples = $2
	const char* str = {
	"|ogg_demuxer,url=$0|-->"
	"|pcm_muxer,url=$1|"
	};
#endif
#if 0
	void* param[] = {
	(void*)(PATH FILE_NAME),
	(void*)(PATH FILE_NAME ".oggdem"),
	};
	//, url = $0, fSamples = $2
	const char* str = {
	"|ogg_demuxer,url=$0|-->"
	"|opus_dec|-->"
	"|pcm_muxer,url=$1|"
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
	OggMtfTest();
	//OggGadfTest();
}



