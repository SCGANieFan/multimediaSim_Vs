#include "smf_common.h"
#include "smf_debug.h"
#include "SmfPorting.h"
#include "cmsis.h"
#include "hal_cmu.h"
#include "hal_sleep.h"
#include <atomic>
//#include <time.h>

#ifdef CHIP_BEST1700
#include "hal_maincmu_best1700.h"
#endif
EXTERNC int osGetThreadIntId();
namespace smf {
	int8_t  _gIntlocked = 0;
	int intr_lock() {
		int rst = int_lock();
		_gIntlocked++;
		return rst;
	}
	void intr_unlock(int v) {
		if (--_gIntlocked < 0)_gIntlocked = 0;
		int_unlock(v);
	}
	bool intr_locked() {
		return _gIntlocked > 0;
	}
	//void rtc_calibrate_ms(int64_t diff) {
	//	_rtc_diff = diff;
	//}
	//void smf_calibrate_rtc(uint32_t systime, uint32_t rtc) {
	//	_rtc_diff = (int64_t)rtc - systime;
	//}

	//uint32_t get_rtc_ms() {
		//return get_local_ms() + _rtc_diff;
	//	return get_timestamp() + _rtc_diff;
	//}

	//uint64_t get64_rtc_ms() {
	//	return get_local_ms() + _rtc_diff;
	//}

	//time_t get_time64(time_t*) {
	//	return (time_t)get_rtc_ms() / 1000;
	//}

	const char* get_platform() {
		return TO_STR(__core__);
	}

	WEAK int get_thread_idx() {
		return osGetThreadIntId();
	}
#if 0
	static char _rtc_buff[32];
	const char* get_rtc_string() {
		auto rtc = get_rtc_ms();
		auto timer = (time_t)rtc / 1000;
		auto tm = localtime(&timer);
		sprintf(_rtc_buff, "%4d-%02d-%02d %02d:%02d:%02d.%03d", result.tm_year, result.tm_mon, result.tm_mday, result.tm_hour, result.tm_min, result.tm_sec, rtc % 1000);
		return _rtc_buff;
	}

	const char* get_rtc_short() {
		auto rtc = get_rtc_ms();
		auto timer = (time_t)rtc / 1000;
		auto tm = localtime(&timer);
		//sprintf(_rtc_buff, "%02d:%02d:%02d.%03d", result.tm_hour, result.tm_min, result.tm_sec, rtc % 1000);
		sprintf(_rtc_buff, "%02u%02u%02u.%03d", result.tm_hour, result.tm_min, result.tm_sec, rtc % 1000);
		return _rtc_buff;
	}
#endif
#if 0
	char* rtc_printf(char* ptr, char* end) {
		auto rtc = get_rtc_ms();
		auto timer = (time_t)rtc / 1000;
		struct tm result = { 0 };
		localtime_r(&timer, &result);
		ptr += snprintf(ptr, end - ptr, "%4d-%02d-%02d %02d:%02d:%02d.%03d", result.tm_year, result.tm_mon, result.tm_mday, result.tm_hour, result.tm_min, result.tm_sec, rtc % 1000);
		// auto tm = localtime(&timer);
		// ptr+=snprintf(ptr, end-ptr, "%4d-%02d-%02d %02d:%02d:%02d.%03d",tm->tm_year, tm->tm_mon, tm->tm_mday, tm->tm_hour, tm->tm_min, tm->tm_sec, rtc % 1000);
		return ptr;
	}
	char* rtc_printf(char* ptr, char* end, const char* pattern, void* others) {
		auto rtc = get_rtc_ms();
		auto timer = (time_t)rtc / 1000;
		struct tm result = { 0 };
		localtime_r(&timer, &result);
		ptr += snprintf(ptr, end - ptr, pattern, result.tm_year, result.tm_mon, result.tm_mday, result.tm_hour, result.tm_min, result.tm_sec, rtc % 1000, others);
		// auto tm = localtime(&timer);
		// ptr+=snprintf(ptr, end-ptr, "%4d-%02d-%02d %02d:%02d:%02d.%03d",tm->tm_year, tm->tm_mon, tm->tm_mday, tm->tm_hour, tm->tm_min, tm->tm_sec, rtc % 1000);
		return ptr;
	}
	char* rtc_printf_short(char* ptr, char* end) {
		return rtc_printf(ptr, end, "%04d%02d%02d_%02d%02d%02d_%03d");
	}
#endif
#if 0
	char* rtc_printf_short(char* ptr, char* end) {
		auto rtc = get_rtc_ms();
		struct tm result = { 0 };
		auto timer = (time_t)rtc / 1000;
		// auto tm = localtime(&timer);
		localtime_r(&timer, &result);
		ptr += snprintf(ptr, end - ptr, "%02u:%02u:%02u.%03d", result.tm_hour, result.tm_min, result.tm_sec, rtc % 1000);
		return ptr;
	}
#endif
#if 0
	void* tcm_switch_to_dev(uint32_t addr){
#ifdef CHIP_BEST1700
		return (void*)dsp_hifi4_addr_cpu_to_dev(addr);
#else
		return (void*)addr;
#endif
	}
#endif

#ifndef ADDR_CPU_TO_DEV
#define ADDR_CPU_TO_DEV
#endif
#ifndef ADDR_DEV_TO_CPU
#define ADDR_DEV_TO_CPU
#endif
	void* addr_remap_to_dev(const void* addr)
	{
		return (void*)(addr ? ADDR_CPU_TO_DEV(((uint32_t)addr)) : 0);
	}

	void* addr_remap_to_cpu(const void* addr)
	{
		return (void*)(addr ? ADDR_DEV_TO_CPU(((uint32_t)addr)) : 0);
	}

	void bus_lock(void) {
		hal_bus_wake_lock(HAL_BUS_WAKE_LOCK_USER_CP);
	}
	void bus_unlock(void) {
		hal_bus_wake_unlock(HAL_BUS_WAKE_LOCK_USER_CP);
	}

#ifndef PSRAM_SIZE
#define PSRAM_SIZE SYS_PSRAM_NS_SIZE
#endif
	void* address_to_uncache(void* addr) {
		//return hal_addr_to_uncache(addr);
#if PSRAM_BASE && PSRAM_NC_BASE && PSRAM_SIZE
		if ((uint32_t)addr >= PSRAM_BASE && (uint32_t)addr < PSRAM_BASE + PSRAM_SIZE) {
			return (char*)addr - PSRAM_BASE + PSRAM_NC_BASE;
		}
#endif
		return addr;
	}
	void* address_to_cache(void* addr) {
		//return hal_addr_to_cache(addr);
#if PSRAM_BASE && PSRAM_NC_BASE && PSRAM_SIZE
		if ((uint32_t)addr >= PSRAM_NC_BASE && (uint32_t)addr < PSRAM_NC_BASE + PSRAM_SIZE) {
			return (char*)addr - PSRAM_NC_BASE + PSRAM_BASE;
		}
#endif
		return addr;
	}
}