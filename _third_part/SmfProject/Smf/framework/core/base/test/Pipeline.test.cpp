#include "stdio.h"
#include "stdbool.h"
#include "stdint.h"
#include "smf_api.h"
#include "smf_debug.h"
//using namespace smf;
#if 1
#define SMF_REGISTER(x) void smf_##x##_register();smf_##x##_register()
#define TEST_PATH "D://work/audio/"
#ifdef PLATFORM
#undef TEST_PATH
#define TEST_PATH "fatfs:/"
#endif
#define TEST_FILE "48000x1.aac.m4a"
#define TEST_FILE_SRC "file://" TEST_PATH TEST_FILE
#define TEST_FILE_DST "file://" TEST_PATH "xian/" TEST_FILE
#if 0
static const smf_layout_t _serial_io[] = {
	{1,0,{0,0},"pl",{"pl",0,0,0},{}}
	,{2,1,{0,0},"src",{"src-io",0,0},
		{{"thread",true},{"stackSize",1024 * 8}
		,{"url", (uint32_t)TEST_FILE_SRC}
		}}
	,{3,1,{2,0},"sink",{"sink-io",0,0},{{"url", (uint32_t)TEST_FILE_DST".io"}}}
	,{0}
};
#endif
const char* _serial_io = "pl,pl{"
	"src,src-pl,0,thread=#1,stackSize=#8192,url=[" TEST_FILE_SRC "];"
	"sink,sink-io,src,url=[" TEST_FILE_DST ".io]}"
	;
EXTERNC void smf_test_init_io() {
	///register IO
	SMF_REGISTER(pipeline);
	SMF_REGISTER(io_file);
	SMF_REGISTER(io_source);
	SMF_REGISTER(io_sink);
}
EXTERNC bool smf_test_layout(const char* serial, uint32_t* params);
void test_pipeline_io() {
	smf_init();
	smf_test_init_io();
	smf_test_layout(_serial_io,0);
	smf_uninit();
	return;
}
#endif