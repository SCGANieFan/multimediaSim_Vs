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
bool mutex::initialize() {
	if (_mutex)
		return true;
	_mutex = new std::recursive_mutex();
	return _mutex;
}
mutex::~mutex() {
	if (_mutex){
		delete (std::recursive_mutex*)_mutex;
	}
}

void mutex::lock() {
	auto& mtx = *(std::recursive_mutex*)_mutex;
	mtx.lock();
}
bool mutex::try_lock() {
	auto& mtx = *(std::recursive_mutex*)_mutex;
	return mtx.try_lock();
}
void mutex::unlock() {
	auto& mtx = *(std::recursive_mutex*)_mutex;
	mtx.unlock();	
}
//
#if 0
timed_mutex::timed_mutex() {
	auto buff = BasePool().Alloc(sizeof(std::timed_mutex));
	auto mtx = new(buff) std::timed_mutex();
	_mutex = mtx;
}
timed_mutex::~timed_mutex() {
	if (_mutex) {
		auto& mtx = *(std::timed_mutex*)_mutex;
		mtx.unlock();
		sleep_for(1);
		mtx.~timed_mutex();
		BasePool().Free(_mutex);
	}
}

void timed_mutex::lock() {
	auto& mtx = *(std::timed_mutex*)_mutex;
	mtx.lock();
}
void timed_mutex::unlock() {
	auto& mtx = *(std::timed_mutex*)_mutex;
	mtx.unlock();
}
//
bool timed_mutex::try_lock() {
	auto& mtx = *(std::timed_mutex*)_mutex;
	return mtx.try_lock();
}
bool timed_mutex::lock_for(uint32_t timeout) {
	auto& mtx = *(std::timed_mutex*)_mutex;
	return mtx.try_lock_for(std::chrono::milliseconds(timeout));
}
bool timed_mutex::lock_until(uint32_t timepoint) {
	auto& mtx = *(std::timed_mutex*)_mutex;
	return mtx.try_lock_until(std::chrono::system_clock::time_point(std::chrono::milliseconds(timepoint)));
}
#endif

//unique_lock::unique_lock(mutex& mtx, uint32_t timeout_ms):_mtx(mtx) {
//	mtx.lock_for(timeout_ms);
//}
//unique_lock::unique_lock(mutex& mtx): _mtx(mtx) {
//	mtx.lock();
//}
//unique_lock::~unique_lock() {
//	_mtx.unlock();
//}
