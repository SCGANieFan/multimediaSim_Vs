#include "mutex.h"
#include "SmfPorting.h"
#include "smf_debug.h"
#include <mutex>
#include <stdint.h>
#include <string.h>
using namespace smf;

mutex::mutex(bool init) {
	if (init) {
		initialize();
	}
}
bool mutex::initialize(){
	return true;
}
mutex::~mutex() {
}

void mutex::lock() {
}
bool mutex::try_lock() {
	return true;
}
void mutex::unlock() {
}
//
bool timed_mutex::lock_for(uint32_t timeout) {
	return true;
}

bool timed_mutex::lock_until(uint32_t timepoint) {
	return false;
}

