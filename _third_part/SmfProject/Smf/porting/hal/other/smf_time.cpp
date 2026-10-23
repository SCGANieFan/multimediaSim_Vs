#include "SmfPorting.h"
#include "smf_debug.h"
#include <chrono>

namespace smf {
	void timer_sync() {

	}
	uint32_t get_ms() {
		return (uint32_t)get_local_ms();
	}
	uint32_t get_us() {
		return (uint32_t)get_local_us();
	}
	uint64_t get64_ms() {
		return get_local_ms();
	}
	uint64_t get64_us() {
		return get_local_us();
	}
	int64_t get_local_ms() {
		return std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count();
	}
	int64_t get_local_us() {
		return std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::system_clock::now().time_since_epoch()).count();
	}

	//void smf_calibrate_local_time(uint32_t systime, uint64_t local) {
	//
	//}

	uint64_t get64_ticks() {
		return get64_us();
	}
	uint32_t get_ticks() {
		return get_us();
	}
	uint32_t get_timestamp() {
		return get_ms();
	}
#if 0
	void set64_us(uint64_t us) {
	}

	void set64_ticks(uint64_t ticks) {
	}
#endif
	// uint32_t get_rtc_ms() {
	// 	return 0;
	// }
	// char* rtc_printf(char*ptr, char*, const char*, void*) {
	// 	return ptr;
	// }
	// char* rtc_printf(char* ptr, char* end) {
	// 	return ptr;
	// }
}
