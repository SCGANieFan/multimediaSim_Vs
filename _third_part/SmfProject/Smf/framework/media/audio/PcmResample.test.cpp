#include "smf_api.h"
#include "SmfPorting.h"
#include "smf_debug.h"
#if 0
#if 0
EXTERNC void test_resample() {
	SMF_REGISTER(std_pool);
	smf_designate_globle_pool("gbl");
	SMF_REGISTER(fs);
	SMF_REGISTER(bg);
	SMF_REGISTER(pcm_format);
	SMF_REGISTER(wav_demuxer);
	SMF_REGISTER(wav_muxer);
	SMF_REGISTER(pipeline);
#define FNAME "E://work/audio/sin/sin400hz_48000_2_16_5s.wav"
	auto script =
		"pl,pl{"
		"src,dem-wav,0,thread=#1,url=[" FNAME "];"
		"fmt,fmt-pcm,src,rate=#8000,ch=#1,bits=#32;"
		"sink,mux-wav,fmt,url=[" FNAME "8_1_32.wav];"
		"},sts=#2,autoDestroyAfterEOS=#1";
	auto x = smf_create_object_from_string(script, 0);
	dbgTestPPL(x);
	sleep_for(80000);
}
#endif
#if 1
static int _trigCnt = 0;
static int _errCnt = 0;
static int _eosCnt = 0;
static void test_fmt(const char* fpath,const char* fname, uint32_t rate, uint8_t chn, uint8_t bits, uint8_t width) {
	char ifname[128];
	sprintf(ifname, "%s/%s", fpath, fname);
	char ofname[128];
	sprintf(ofname, "%s/2/%d-%d-%d-%d.wav", fpath, rate, chn, bits, width);
	dbgTestPXL("%s,%s",ifname,ofname);
	//static bool _loop;
	//_loop = true;
	auto cb = [](smf_message_t* msg) {
		switch (msg->id) {
		case SMF_MESSAGE_ERROR:_errCnt = false; break;
		case SMF_MESSAGE_EOS:_eosCnt++; break;
		default:break;
		}
		dbgTestPXL("%d,%d,%d", _trigCnt,_eosCnt,_errCnt);
		return false;
	};
	uint32_t params[] = { (uint32_t)ifname,(uint32_t)ofname,rate,chn,bits,width,(uint32_t)(cb_message)cb };
	auto script =
		"_,pl{"
			"src,dem-wav,0,thread=#1,url=$0,omax=#1920;"
			"fmt,fmt-pcm,src,omax=#8192,rate=$2,ch=$3,bits=$4,width=$5;"
			"sink,mux-wav,fmt,url=$1;"
		"},cbMsg=$6,autoDestroyAfterEOS=#1,sts=#2";
		//"},autoDestroyAfterEOS=#1,sts=#2";
	auto x = smf_create_object_with_params(script, 0, params);
	returnIfErrC0(!x);
	_trigCnt++;
	//while (_loop)
	//	sleep_for(1);
	dbgTestPXL("\n\n\n\n");
}

EXTERNC void test_resample() {
	SMF_REGISTER(std_pool);
	smf_designate_globle_pool("gbl");
	SMF_REGISTER(fs);
	SMF_REGISTER(background);
	SMF_REGISTER(pcm_format);
	SMF_REGISTER(wav_demuxer);
	SMF_REGISTER(wav_muxer);
	SMF_REGISTER(pipeline);
#define FPATH "E://work/audio/testfmt/"
#define FNAME "sin400hz_48000_2_16_5s.wav"
	uint32_t rates[]{ 8000,16000,32000,44100,48000,96000,192000 };
	//uint32_t rates[]{ 48000 };
	uint32_t chns[]{ 1,2,4,8 };
	//uint32_t chns[]{ 1 };
	uint32_t bitss[]{ 8,16,24,32 };
	//uint32_t bitss[]{32 };
	uint32_t widths[]{ 0,1,2,3,4 };
	//uint32_t widths[]{ 0 };
	//test_fmt(FPATH, FNAME, 48000, 8, 24, 1);
	for (auto rate : rates) {
		for (auto chn : chns) {
			for (auto bits : bitss) {
				for (auto w : widths) {
					test_fmt(FPATH,FNAME, rate, chn, bits, w);
					sleep_for(100);
				}
			}
		}
	}
	//test_fmt(FNAME, 48000, 1, 8, 0);

	sleep_for(10000);
	dbgTestPXL("test end: %d,%d,%d", _trigCnt, _eosCnt, _errCnt);
}
#endif
#endif