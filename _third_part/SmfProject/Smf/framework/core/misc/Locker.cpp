#include "Locker.h"
#include "SmfPorting.h"
#include "smf_debug.h"

using namespace smf;
Locker::Locker(volatile uint8_t& lock, void*& tid, uint32_t timeout)
	: _lock(&lock)
	, _ptid(&tid)
{
	if (timeout ? !Lock(timeout) : !Lock()) {
		_lock = 0;
	}
}
Locker::Locker(volatile uint8_t& lock, uint32_t timeout)
	: _lock(&lock)
	, _ptid(0)
{
	if (timeout ? !Lock(timeout) : !Lock()) {
		_lock = 0;
	}
}
Locker::~Locker() {
	Unlock();
}

bool Locker::Get() const {
	return _lock ? *_lock : false;
}
bool Locker::Lock() {
	returnIfErrC(false, !_lock);
	auto& lock = *_lock;
	auto tid = _ptid ? get_thread_id() : (void*)0;
	if (lock == 0) {
		lock = 1;
		if (_ptid) {
			*_ptid = tid;
		}
		return true;
	}
	else if (_ptid && *_ptid && *_ptid == tid) {
		lock++;
		return true;
	}
	else {
		dbgTestPL();
		return false;
	}
}
bool Locker::Lock(uint32_t timeout) {
	returnIfErrC(false, !_lock);
	auto& lock = *_lock;
	auto tid = _ptid ? get_thread_id() : (void*)0;
	auto c = timeout;
	do {
		if (lock == 0) {
			lock = 1;
			if (_ptid) {
				*_ptid = tid;
			}
			return true;
		}
		else if (_ptid && *_ptid && *_ptid == tid) {
			lock++;
			return true;
		}
		else if (c-- < 1) {
			dbgErrPDL(timeout);
			return false;
		}
		else {
			sleep_for(1);
		}
	} while (1);
}
void Locker::Unlock() {
	if (_lock) {
		(*_lock)--;
	}
}
//
void AtomicLocker::Release() {
	if (_lock) {
		_lock->store(false, std::memory_order_release);
		_lock = 0;
	}
}
void AtomicLocker::Acquire() {
	if (_lock) {
		bool expected = false;
		while (_lock && !_lock->compare_exchange_weak(expected, true, std::memory_order_acquire, std::memory_order_relaxed)) {
			expected = false; // reset expected value
			sleep_for(1);
		}
	}
}