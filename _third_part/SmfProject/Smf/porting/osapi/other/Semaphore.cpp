#include "Semaphore.h"
#include "SmfPorting.h"
#include "smf_debug.h"
#include <mutex>
#include <condition_variable>
#include <string.h>
using namespace smf;

class sema_t {
public:
	sema_t(int cnt = 0) :_cnt(cnt){}
	std::mutex _mutex;
	std::condition_variable _condition;
	int _cnt;
};

semaphore::semaphore(int cnt, bool init) {
	if (init)initialize(cnt);
}
semaphore::~semaphore() {
	if (_sema) {
		auto sema = (sema_t*)_sema;
		//
		//sema->_condition.notify_all();
		//sema->_condition.~condition_variable();
		//
		//sema->_mutex.unlock();
		//sema->_mutex.~mutex();
		delete sema;
	}
}
bool semaphore::initialize(int cnt) {
	if (!_sema) {
		_sema = new sema_t(cnt);
		returnIfErrC(false, !_sema);
	}
	return true;
}
uint32_t semaphore::count()const {
	returnIfErrC(0, !_sema);
	auto sema = (sema_t*)_sema;
	return sema->_cnt;
}
bool semaphore::reset(int cnt) {
	returnIfErrC(false, !initialize());
	auto sema = (sema_t*)_sema;
	auto& mutex = sema->_mutex;
	auto& condition = sema->_condition;
	int last_cnt = sema->_cnt;
	std::unique_lock <std::mutex> lock{ mutex };
	sema->_cnt = cnt;
	if (last_cnt <= 0 && cnt > 0) {
		condition.notify_one();
	}
	return true;
}
bool semaphore::post() {
	returnIfErrC(false, !initialize());
	auto sema = (sema_t*)_sema;
	auto& mutex = sema->_mutex;
	auto& condition = sema->_condition;
	std::unique_lock <std::mutex> lock{ mutex };
	if (sema->_cnt++ < 0) {
		condition.notify_one();
	}
	return true;
}
bool semaphore::wait() {
	returnIfErrC(false, !initialize());
	auto sema = (sema_t*)_sema;
	auto& mutex = sema->_mutex;
	auto& condition = sema->_condition;
	std::unique_lock <std::mutex> lock{ mutex };
	if (sema->_cnt-- <= 0){
		condition.wait(lock);
	}
	return true;
}
bool semaphore::try_wait() {
	returnIfErrC(false, !initialize());
	auto sema = (sema_t*)_sema;
	auto& mutex = sema->_mutex;
	//auto& condition = sema->condition;
	std::unique_lock <std::mutex> lock{ mutex };
	return sema->_cnt-- > 0;
}
bool semaphore::wait_for(uint32_t timeout) {
	returnIfErrC(false, !initialize());
	auto sema = (sema_t*)_sema;
	auto& mutex = sema->_mutex;
	auto& condition = sema->_condition;
	std::unique_lock <std::mutex> lock{ mutex };
	if (sema->_cnt-- <= 0) {
		auto sts = condition.wait_for(lock,
			std::chrono::milliseconds(timeout)
		);
		return sts == std::cv_status::no_timeout;
	}
	return true;
}
bool semaphore::wait_until(uint32_t timepoint) {
	returnIfErrC(false, !initialize());
	auto sema = (sema_t*)_sema;
	auto& mutex = sema->_mutex;
	auto& condition = sema->_condition;
	std::unique_lock <std::mutex> lock{ mutex };
	if (sema->_cnt-- <= 0) {
		auto sts = condition.wait_until(lock,
			std::chrono::system_clock::time_point(std::chrono::milliseconds(timepoint))
		);
		return sts == std::cv_status::no_timeout;
	}
	return true;
}
