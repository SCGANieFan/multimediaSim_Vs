#include "thread.h"
#include "SmfPorting.h"
#include "smf_debug.h"
#include <thread>
#include <string.h>
using namespace smf;
thread::thread() {
}
thread::thread(const char* name
	, void(*func)(void*)
	, void* para
	, unsigned stack_size
	, void* stack
	, uint32_t priority
) {
	Start(name ,func, para, stack_size, stack, priority);
}
thread::~thread() {
	//detach();
	delete ((std::thread*)_thread_id);
}
const char* thread::Name()const {
	return _name;
}
void* thread::ID() const{
	return _thread_id;
}
bool thread::Start(const char* name
	, void(*func)(void*)
	, void* para
	, unsigned stack_size
	, void* stack
	, uint32_t priority
) {
	returnIfErrC(false, _name);
	returnIfErrC(false, !name);
	returnIfErrC(false, !func);
	returnIfErrC(false, !stack_size);
	_name = name;
	auto thread0 = new std::thread([name,func,para]() {
		dbgInfoPXL(name);
		func(para);
		dbgInfoPXL(name);
		});
	//
	_thread_id = (void*)thread0;
	returnIfErrCS(false, !_thread_id, "%s,%u", name, stack_size);
	//
	return true;
}
void thread::join() {
	returnIfErrC0(!_thread_id);
	dbgTestPSL(_name);	
	((std::thread*)_thread_id)->join();	
}
void thread::detach() {
	returnIfErrC0(!_thread_id);
	dbgTestPSL(_name);	
	((std::thread*)_thread_id)->detach();
}

//EXTERNC void smf_thread_print() {
//}

//void thread::sleep_for(uint32_t ms) {
//	std::this_thread::sleep_for(std::chrono::milliseconds(ms));
//}
//void thread::sleep_until(uint64_t ms) {
//	std::this_thread::sleep_until(std::chrono::time_point<std::chrono::steady_clock>(std::chrono::milliseconds(ms)));
//}
//void thread::yield() {
//	std::this_thread::yield();
//}
//void* thread::Current() {
//	auto id = std::this_thread::get_id();
//	return (void*)*(uint32_t*)&id;
//}
//char* thread::PrintS(char* ptr, char* end) {
//	return ThreadList::List().Print(ptr, end);
//}