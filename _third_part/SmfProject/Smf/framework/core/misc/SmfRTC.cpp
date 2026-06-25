#include "SmfICS.h"
#include "SmfRTC.h"
#include "SmfPorting.h"
#include "SmfDef.h"
#include <time.h>

namespace smf {
	uint32_t get_rtc_ms() {
		return ics::GetRtcMs();
	}
	uint64_t get64_rtc_ms() {
		return ics::GetRtcMs();
	}
	char* rtc_printf(char* ptr, char* end, const char* pattern, void* others) {
		auto rtc = get64_rtc_ms();
		auto timer = (time_t)rtc / 1000;
		struct tm result = { 0 };
		localtime_r(&timer, &result);
		ptr = snprintf(ptr, end, pattern, result.tm_year, result.tm_mon, result.tm_mday, result.tm_hour, result.tm_min, result.tm_sec, rtc % 1000, others);
		// auto tm = localtime(&timer);
		// ptr+=snprintf(ptr, end-ptr, "%4d-%02d-%02d %02d:%02d:%02d.%03d",tm->tm_year, tm->tm_mon, tm->tm_mday, tm->tm_hour, tm->tm_min, tm->tm_sec, rtc % 1000);
		return ptr;
	}
	char* rtc_printf(char* ptr, char* end) {
		return rtc_printf(ptr, end, "%4d-%02d-%02d %02d:%02d:%02d.%03d", 0);
	}
	char* rtc_printf_short(char* ptr, char* end) {
		return rtc_printf(ptr, end, "%04d%02d%02d_%02d%02d%02d_%03d");
	}
}