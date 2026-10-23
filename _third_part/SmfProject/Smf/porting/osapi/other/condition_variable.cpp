#if 0
#include "condition_variable.h"
#include "SmfPorting.h"
#include "smf_debug.h"
#include <mutex>
#include <condition_variable>
#include <string.h>
using namespace smf;

condition_variable::condition_variable() {
	auto buff = BasePool().Alloc(sizeof(std::condition_variable));
	auto condition = new(buff) std::condition_variable();
	_condition = condition;
}
condition_variable::~condition_variable() {
	if (_condition) {
		auto& condition = *(std::condition_variable*)_condition;
		condition.notify_all();
		sleep_for(1);
		condition.~condition_variable();
		BasePool().Free(_condition);
	}
}

void condition_variable::notify_one() {
	auto& condition = *(std::condition_variable*)_condition;
	condition.notify_one();
}
void condition_variable::notify_all() {
	auto& condition = *(std::condition_variable*)_condition;
	condition.notify_all();
}
void condition_variable::wait(unique_lock<mutex>& lock) {
	auto& condition = *(std::condition_variable*)_condition;
	std::unique_lock<std::mutex> lck(*(std::mutex*)lock.Mutex().Handle());
	return condition.wait(lck);
}
bool condition_variable::wait_for(unique_lock<mutex>& lock, uint32_t timeout) {
	auto& condition = *(std::condition_variable*)_condition;
	std::unique_lock<std::mutex> lck(*(std::mutex*)lock.Mutex().Handle());
	return condition.wait_for(lck,std::chrono::milliseconds(timeout)) 
		== std::cv_status::no_timeout;
}
bool condition_variable::wait_until(unique_lock<mutex>& lock, uint32_t timepoint) {
	auto& condition = *(std::condition_variable*)_condition;
	std::unique_lock<std::mutex> lck(*(std::mutex*)lock.Mutex().Handle());
	return condition.wait_until(
		lck,std::chrono::system_clock::time_point(std::chrono::milliseconds(timepoint))
	) == std::cv_status::no_timeout;
}

#endif