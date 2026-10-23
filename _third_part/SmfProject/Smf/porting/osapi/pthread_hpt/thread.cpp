#include "thread.h"
#include "SmfPortingThread.h"
#include "SmfFCC.h"
#include "smf_debug.h"
#include <pthread.h>

using namespace smf;
using Item_t = ThreadItem_t;

thread::thread() {
}
thread::thread(const char* name
	, void(*func)(void*)
	, void* para
	, unsigned stack_size
	, void* stack
	, uint32_t priority
) {
	Start(name, func, para, stack_size, stack, priority);
}
thread::~thread() {
	//dbgTestPL();
}
const char* thread::Name()const {
	return _name;
}
void* thread::ID() const {
	return _thread_id;
}
struct ThreadAttributes {
	ThreadAttributes() {
		pthread_attr_init(&attr);
		pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED);
	}
	~ThreadAttributes() { pthread_attr_destroy(&attr); }
	pthread_attr_t* operator&() { return &attr; }
	pthread_attr_t attr;
};
bool thread::Start(const char* name
	, void(*func)(void*)
	, void* para
	, unsigned stack_size
	, void* stack
	, uint32_t priority
) {
	returnIfErrC(false, !name);
	returnIfErrC(false, !func);
	returnIfErrC(false, !stack_size);
	_name = name;
	//dbgTestPSL(name);
	returnIfErrCS(false, strnlen(name, 8) >= 8, "%s", name);
	ThreadAttributes attr;
	sched_param sp;
	memset(&sp, 0, sizeof(sp));
	pthread_attr_setstacksize(&attr, stack_size);
	pthread_attr_getschedparam(&attr, &sp);
	// int max = sched_get_priority_max(SCHED_FIFO);
	// int min = sched_get_priority_min(SCHED_FIFO);
	// auto avg = (min + max) / 2;
	//if(!strcmp(name, "mix")) sp.sched_priority = 223;
	//if(!strcmp(name, "src")) sp.sched_priority = 223;
	pthread_attr_setschedparam(&attr, &sp);

	auto item = SmfThreadHold(name);
	returnIfErrC(false, !item);
	item->_name = fcc64(name);
	item->_para = para;
	item->_func = func;
	item->_stackSize = stack_size;

	pthread_t thread_ = 0;
	int ret = pthread_create(
		&thread_
		, &attr
		, [](void* const obj_)->void* {
			auto item = (Item_t*)obj_;
			auto pid = pthread_self();
			auto it_name = (const char*)(&item->_name);
			pthread_setname_np(pid, it_name);
			dbgInfoPXL("begin:%s, PID:%d", it_name, pid);
			item->_func(item->_para);
			dbgInfoPXL("end:%s", it_name);
			SmfThreadRelease(item);
			return 0;
		}
		, item);

	returnIfErrCS(false, ret, "[%d]%s,%u", ret, name, stack_size);

	_thread_id = (void *)thread_;
	item->_id = _thread_id;
	return true;
}
void thread::join() {
}
void thread::detach() {
}
