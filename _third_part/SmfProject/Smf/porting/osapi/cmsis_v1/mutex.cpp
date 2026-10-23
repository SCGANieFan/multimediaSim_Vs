#include "mutex.h"
#include "SmfPorting.h"
#include "smf_debug.h"
#include "cmsis_os2.h"
using namespace smf;

mutex::mutex(bool init) {
	if (init) {
		initialize();
	}
}
bool mutex::initialize() {
	if (_mutex)
		return true;
	if (intr_locked())
		return true;
	osMutexAttr_t attr;
	attr.name = 0;
	attr.attr_bits = osMutexRecursive | osMutexPrioInherit;
	attr.cb_mem = 0;
	attr.cb_size = 0;
	auto id = osMutexNew(&attr);
	returnIfErrC(false, !id);
	_mutex = id;
	return true;
#if 0
	osMutexDef_t os_mutex_def_x =	{ 
		NULL
		,osMutexRecursive | osMutexPrioInherit
		,NULL//(StaticSemaphore_t*)_cb//(instances == 1) ? (&os_thread_cb_##name) : NULL, 
		,0//sizeof(StaticSemaphore_t)//(instances == 1) ? sizeof(StaticTask_t) : 0U, 
	};
	//
	_mutex = osMutexCreate(&os_mutex_def_x);
	//dbgTestPPL(_mutex);
	returnIfErrC(false, !_mutex);
	return true;
#endif
}
mutex::~mutex() {
	if(_mutex && !intr_locked())
		osMutexDelete((osMutexId_t)_mutex);
}

void mutex::lock() {
	if (_mutex && !intr_locked())
		osMutexAcquire((osMutexId_t)_mutex, osWaitForever);
}
bool mutex::try_lock() {
	returnIf(false, intr_locked());
	return _mutex?(osMutexAcquire((osMutexId_t)_mutex, 0) == osOK):false;
}
void mutex::unlock() {
	if (_mutex && !intr_locked())
		osMutexRelease((osMutexId_t)_mutex);
}

bool timed_mutex::lock_for(uint32_t timeout) {
	returnIf(false, intr_locked());
	if (_mutex)
		osMutexAcquire((osMutexId_t)_mutex, timeout);

	return true;
}

bool timed_mutex::lock_until(uint32_t timepoint) {
	return false;
}
