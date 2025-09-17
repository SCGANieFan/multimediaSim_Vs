#include <chrono>
#include <thread>
#include "MTF.Porting.h"

// std
#if 0
int64_t TimeMsPorting() {
	auto now = std::chrono::system_clock::now();
	auto duration = now.time_since_epoch();
	auto millis = std::chrono::duration_cast<std::chrono::milliseconds>(duration).count();
	return millis;
}

int64_t TimeUsPorting() {
	auto now = std::chrono::system_clock::now();
	auto timestamp = std::chrono::duration_cast<std::chrono::microseconds>(
		now.time_since_epoch()
	).count();
	return timestamp;
}

void SleepMsPorting(int ms) {
	std::this_thread::sleep_for(std::chrono::milliseconds(ms));
}
#endif

mtf_i64 TimeMsPorting() {
	return std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count();
}

mtf_i64 TimeUsPorting() {
	return std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::system_clock::now().time_since_epoch()).count();
}

mtf_void SleepMsPorting(mtf_i32 ms) {
	std::this_thread::sleep_for(std::chrono::milliseconds(ms));
}

