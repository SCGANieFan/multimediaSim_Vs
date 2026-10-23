/*
 * hifi4.c
 *
 *  Created on: 2021/10/29
 *      Author: wangjg
 */
#include "SmfPorting.h"
#include "smf_debug.h"
#include <thread>
#include <chrono>
namespace smf {
	void* get_thread_id(void) {
		return 0;//(void*)std::this_thread::get_id();
	}

	void sleep_for(uint32_t ms) {
		std::this_thread::sleep_for(std::chrono::milliseconds(ms));
	}
	void sleep_until(uint64_t ms) {
		std::this_thread::sleep_until(std::chrono::system_clock::time_point(std::chrono::milliseconds(ms)));
	}
	void yield() {
		std::this_thread::yield();
	}
}


