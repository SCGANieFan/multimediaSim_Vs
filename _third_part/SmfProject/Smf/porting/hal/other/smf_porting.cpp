#include "smf_common.h"
#include "smf_debug.h"
#include "SmfPorting.h"
//#include "SmfDef.h"
//#include <chrono>
//#include <ctime>
//#include <time.h>

namespace smf {
	int intr_lock() {
		return 1;
	}

	bool intr_locked() {
		return false;
	}

	void intr_unlock(int v) {
	}
	//static int64_t _rtc_diff = 0;
	//void smf_calibrate_rtc(uint32_t systime, uint32_t rtc) {
	//	_rtc_diff = (int64_t)rtc - systime;
	//}

	//uint32_t get_rtc_ms() {
	//	return get64_rtc_ms();
	//}

	//uint64_t get64_rtc_ms() {
	//	return get_local_ms() + _rtc_diff;
	//}
	const char* get_platform() {
		return TO_STR(__core__);
	}

	int get_thread_idx() {
		return 0;
	}
#if 0
	char* rtc_printf(char* ptr, char* end) {
		return rtc_printf(ptr, end, "%04d-%02d-%02d_%02d:%02d:%02d_%03d", 0);
		//return rtc_printf(ptr,end,"%F_%T_%03d", 0);
	}
	char* rtc_printf(char* ptr, char* end, const char* pattern, void* others) {
		auto ms = get64_rtc_ms();
		std::time_t sec = ms / 1000;
		std::tm* ptm = std::localtime(&sec);
		return snprintf(ptr, end, pattern, ptm->tm_year+1900, ptm->tm_mon+1, ptm->tm_mday, ptm->tm_hour, ptm->tm_min, ptm->tm_sec, (int)(ms % 1000), others);
		//std::strftime(ptr, end-ptr, pattern, ptm);
	}

	char* rtc_printf_short(char* ptr, char* end) {
		return rtc_printf(ptr, end, "%04d%02d%02d_%02d%02d%02d_%03d");
	}
#endif
	void* addr_remap_to_dev(const void* addr)
	{
		return (void*)addr;
	}

	void* addr_remap_to_cpu(const void* addr)
	{
		return (void*)addr;
	}

	void bus_lock(void) {
	}
	void bus_unlock(void) {
	}

	void* address_to_uncache(void* addr) {
		return addr;
	}
	void* address_to_cache(void* addr) {
		return addr;
	}
}

#ifdef WIN32
EXTERNC const char* smf_version() {
	static const char* v = "smf@x.x.x.x";
	return v;
}
namespace smf {
	void overlay_load(const char* type, const char* keys) {}
	bool overlay_load(uint32_t id) { return true; }
	bool overlay_unload_all() { return true; }
}
#endif
