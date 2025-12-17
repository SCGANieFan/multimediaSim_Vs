#include <string.h>
#include <stdint.h>
#include <stdarg.h>
#include "heap_api.h"
#include "hal_trace.h"
#include "plc_api.h"

#if WIN32
//#define LOG(fmt,...) 
#define LOG(fmt,...) TRACE(0, "<%s>(%s)[%d]" fmt "\n", strrchr(__FILE__, '\\') + 1, __func__, __LINE__, ##__VA_ARGS__)
#else
#define LOG(fmt,...) TRACE(0, "<%s>(%s)[%d]" fmt "\n", strrchr(__FILE__, '/') + 1, __func__, __LINE__, ##__VA_ARGS__)
#endif
static bool isRegister = false;
static multi_heap_handle_t plcHeap = 0;
static char heap_pool[30 * 1024];

static void* PlcMalloc(uint32_t size) {
	static int sizeTotal = 0;
	sizeTotal += size;
	void* prt = heap_malloc(plcHeap, size);
	LOG("%d,%d,%p", size,sizeTotal, prt);
	return prt;
}
static void PlcFree(void* ptr) {
	LOG("%p", ptr);
	heap_free(plcHeap, ptr);
}
static void PlcPrint(const char* fmt, ...) {
	static char buf[256];
	va_list args;
	va_start(args, fmt);
	vsprintf(buf, fmt, args);
	va_end(args);
	TRACE(0, "%s", buf);
}

uint32_t plc_api_demo_init(uint32_t rate, uint16_t channels, uint16_t width, uint32_t frameSample) {
	if (!isRegister) {
		isRegister = true;
		plc_api_register_music_plc_int16();
		plc_api_register_music_plc_int32();
	}

	if (!plcHeap) {
		plcHeap = heap_register(heap_pool, sizeof(heap_pool));
	}
	uint32_t plc_id = 0;
	plc_api_param_t initParam;
	memset(&initParam, 0, sizeof(plc_api_param_t));
	initParam.malloc_cb = PlcMalloc;
	initParam.free_cb = PlcFree;
	initParam.print_cb = PlcPrint;
	plc_id = plc_api_create(&initParam);
	if (!plc_id) {
		LOG("plc_api_create fail");
		return 0;
	}
	plc_api_ret_t ret = PLC_API_RET_SUCCESS;
	ret |= plc_api_set(plc_id, "rate", (void*)(uint32_t)rate);
	ret |= plc_api_set(plc_id, "ch", (void*)(uint32_t)channels);
	ret |= plc_api_set(plc_id, "fSample", (void*)(uint32_t)frameSample);
	ret |= plc_api_set(plc_id, "chSelect", (void*)(uint32_t)0xffff);
	if (width == 2) ret |= plc_api_set(plc_id, "dataType", (void*)"i16");
	else if (width == 4) ret |= plc_api_set(plc_id, "dataType", (void*)"i32");
	else {
		LOG("width %d is fail",width);
		return 0;
	}
	ret |= plc_api_set(plc_id, "mode", (void*)"musicPlc");
	ret |= plc_api_set(plc_id, "paramSet", (void*)"param");//keyapp,param
	uint32_t samplePerMs = rate / 1000;
	ret |= plc_api_set(plc_id, "overlap", (void*)(uint32_t)(1 * samplePerMs));
	ret |= plc_api_set(plc_id, "holdAL", (void*)(uint32_t)(0 * samplePerMs));
	ret |= plc_api_set(plc_id, "attAL", (void*)(uint32_t)(20 * samplePerMs));
	ret |= plc_api_set(plc_id, "gainANL", (void*)(uint32_t)(20 * samplePerMs));
	//ret |= plc_api_set(plc_id, "forceMT", (void*)1);
	ret |= plc_api_set(plc_id, "seek", (void*)(uint32_t)(15 * samplePerMs));
	ret |= plc_api_set(plc_id, "noSeek", (void*)(uint32_t)(4 * samplePerMs));
	ret |= plc_api_set(plc_id, "match", (void*)(uint32_t)(4 * samplePerMs));
	if (!plc_id) {
		LOG("plc_api_set fail");
		return 0;
	}
	ret |= plc_api_open(plc_id);
	if (!plc_id) {
		LOG("plc_api_open fail");
		return 0;
	}
	return plc_id;
}

bool plc_api_demo_run(uint32_t id, uint8_t* in, int32_t in_len, int32_t* in_used, uint8_t* out, int32_t* p_out_len, uint16_t is_lost) {
	plc_api_ret_t  ret = plc_api_run(id, in, in_len, in_used, out, p_out_len, is_lost);
	if (ret != PLC_API_RET_SUCCESS) return false;
	return true;
}


bool plc_api_demo_deinit(uint32_t id) {
	plc_api_close(id);
	plc_api_destory(id);
	return true;
}



#include "gadf.h"
#include "gadf_porting_api.h"
typedef struct ThreadParam{
	void* info;
	bool isFinish;
};
void plc_api_demo_thread(void*param) {
	uint32_t cnt = 0;
	if (!param) return;
	while (1) {
		if (++cnt > 1 * 10) 
			break;
		gadf_run_by_info(((ThreadParam*)param)->info);
	}
	((ThreadParam*)param)->isFinish = true;
}
void plc_api_demo() {
	extern void gadf_register_info_plc_16k2ch(); gadf_register_info_plc_16k2ch();
	ThreadParam params[2];
	params[0].info = gadf_get_register_info();
	params[0].isFinish = false;
	GadfThreadStart("plc_demo0", 0, plc_api_demo_thread, (void*)&params[0], 4*1024);

	params[1].info = gadf_get_register_info();
	params[1].isFinish = false;
	GadfThreadStart("plc_demo1", 0, plc_api_demo_thread, (void*)&params[1], 4 * 1024);
#if WIN32
	while (1) {
		bool isFinish = true;
		for (uint16_t n = 0; n < sizeof(params) / sizeof(params[0]); n++) {
			if (!params[n].isFinish) {
				isFinish = false;
				break;
			}
		}
		if (isFinish) break;
	}
#endif
}



