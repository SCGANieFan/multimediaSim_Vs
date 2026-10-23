#include "thread.h"
#include "SmfPortingThread.h"
#include "SmfFCC.h"
#include "smf_debug.h"
#include "cmsis_os.h"

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
	Start(name ,func, para, stack_size, stack, priority);
}
thread::~thread() {
	//dbgTestPL();
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
	//returnIfErrC(false, _thread_id);
	returnIfErrC(false, !name);
	returnIfErrC(false, !func);
	returnIfErrC(false, !stack_size);
	_name = name;
	auto item = SmfThreadHold(name);
	returnIfErrC(false, !item);
	item->_name = fcc64(name);
	item->_para = para;
	item->_func = func;
	item->_stackSize = stack_size;
	//
	osThreadDef_t os_thread_def_x =	{
		[](const void* obj_) {
			auto item = (Item_t*)obj_;
			dbgInfoPXL("begin:%s", (const char*)(&item->_name));
			item->_func(item->_para);
			dbgInfoPXL("end:%s", (const char*)(&item->_name));
			SmfThreadRelease(item);
			osThreadExit();
			}
		,{ name
			, osThreadDetached
			,NULL//(instances == 1) ? (&os_thread_cb_##name) : NULL,
			,0//(instances == 1) ? sizeof(StaticTask_t) : 0U,
			,NULL//((stacksz) && (instances == 1)) ? (&os_thread_stack##name) : NULL,
			,8 * (stack_size / 8)
			, priority ? (osPriority_t)priority : osPriorityNormal
			, 0U, 0U
		}
	};
	//
	_thread_id = osThreadCreate(&os_thread_def_x, item);
	item->_id = _thread_id;
	//dbgTestPPL(_thread_id);
	returnIfErrCS(false, !_thread_id, "%s,%u", name, stack_size);
	return true;
}
void thread::join() {

}
void thread::detach() {

}

