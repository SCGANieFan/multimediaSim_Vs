#include "Semaphore.h"
#include "SmfPorting.h"
#include "smf_debug.h"
#include <string.h>
#include <semaphore.h>
#include <time.h>
#include <nuttx/clock.h>
#include <errno.h>

using namespace smf;

semaphore::semaphore(int cnt, bool init) {
	if (init)initialize(cnt);
}

semaphore::~semaphore() {
	if (_sema) {
		sem_destroy((sem_t *)_sema);
		free(_sema);
		_sema = 0;
	}
}
bool semaphore::initialize(int cnt) {
	if (!_sema) {
		sem_t* psem = (sem_t*)malloc(sizeof(sem_t));
		returnIfErrC(false, !psem);
		sem_init(psem, 0, cnt);
		_sema = psem;
		returnIfErrC(false, !_sema);
	}
	return true;
}
uint32_t semaphore::count()const {
	returnIfErrC(0, !_sema);
	return 0;
}
bool semaphore::reset(int cnt) {
	returnIfErrC(false, !initialize());
	while (try_wait());
	while (cnt--) {
		post();
	}
	return true;
}
bool semaphore::post() {
	returnIfErrC(false, !initialize());
	sem_post((sem_t *)_sema);
	return true;
}

bool semaphore::wait() {
	returnIfErrC(false, !initialize());
	auto rst = sem_wait((sem_t *)_sema);
	while (rst == -1 && errno == EINTR) {
		rst = sem_wait((sem_t *)_sema);
		dbgWarnPXL("sem_wait failed retry,errno=%d",errno);
	}

	if (rst < 0) {
		dbgWarnPXL("%d,%d",rst, errno);
		return false;
	}

	return true;
}
bool semaphore::try_wait() {
	returnIfErrC(false, !initialize());
	int ret = sem_trywait((sem_t *)_sema);
	if (ret == 0) return true;
	else return false;
}
bool semaphore::wait_for(uint32_t timeout) {
	returnIfErrC(false, !initialize());
	if (timeout >= 0x7fffffff) {
		wait();
	}
	else {
		struct timespec ts;
		struct timespec ts2;
		ts2.tv_sec = timeout/1000;
		ts2.tv_nsec = (timeout%1000)*1000*1000;
		struct timespec currtime;
		clock_gettime(CLOCK_REALTIME, &currtime);
		clock_timespec_add(&ts2, &currtime,&ts);
		returnIfErrC(false, !_sema);
		int ret = sem_timedwait((sem_t *)_sema, &ts);
		while (ret == -1 && errno == EINTR) {
			ret = sem_timedwait((sem_t *)_sema, &ts);
			dbgWarnPXL("sem_wait failed retry,errno=%d",errno);
		}
		if (ret != 0) {
			auto en = get_errno();
			if (en != ETIMEDOUT) {
				dbgErrPXL("ret = %d, errno=%d", ret, en);
			}
			return false;
		}
	}

	return true;
}
bool semaphore::wait_until(uint32_t timepoint) {
	returnIfErrC(false, !initialize());
	return false;
}
