#include "Semaphore.h"
#include "SmfPorting.h"
#include "smf_debug.h"
#include <string.h>
#include "cmsis_os.h"
using namespace smf;

semaphore::semaphore(int cnt, bool init) {
	if (init)initialize(cnt);
}

semaphore::~semaphore() {
	if (_sema && !intr_locked()) {
		osSemaphoreDelete(_sema);
	}
}
bool semaphore::initialize(int cnt) {
	if (!_sema) {
		returnIf(false, intr_locked());
		osSemaphoreDef_t attr = {
			NULL
			, 0
			, 0
			, 0
		};
		//
		_sema = osSemaphoreCreate(&attr, cnt);
		returnIfErrC(false, !_sema);
	}
	return true;
}
uint32_t semaphore::count()const {
	returnIfErrC(0, !_sema);
	return osSemaphoreGetCount(_sema);
}
bool semaphore::reset(int cnt) {
	returnIfErrC(false, !initialize());
	auto cnt0 = count(); //dbgTestPXL("%d/%d",cnt,cnt0);
	if (cnt > cnt0) {
		cnt = cnt - cnt0;
		while (cnt--)
			post();
	}
	else if (cnt < cnt0) {
		cnt = cnt0 - cnt;
		while (cnt--)
			wait();
	}
	return true;
}
bool semaphore::post() {
	return wait_for(0xffffffff);
}

bool semaphore::try_wait() {
	returnIfErrC(false, !initialize());
	return (osSemaphoreAcquire(_sema, 0) > 0);
}
bool semaphore::wait_for(uint32_t timeout) {
	returnIfErrC(false, !initialize());
	returnIf(false, intr_locked() && timeout);
	auto ret = osSemaphoreAcquire(_sema, timeout);
	return ret >= 0;
}
bool semaphore::wait_until(uint32_t timepoint) {
	returnIfErrC(false, !initialize());
	return false;
}
