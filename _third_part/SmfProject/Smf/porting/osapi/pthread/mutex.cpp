#include "mutex.h"
#include "SmfPorting.h"
#include "smf_debug.h"
#include <mutex>
#include <stdint.h>
#include <string.h>
#include <pthread.h>
using namespace smf;

mutex::mutex(bool init) {
	if (init) {
		initialize();
	}
}
bool mutex::initialize() {
	if (_mutex)
		return true;
	pthread_mutex_t* mt = (pthread_mutex_t*)malloc(sizeof(pthread_mutex_t));
	if (mt) {
		pthread_mutexattr_t attr;
		pthread_mutexattr_init(&attr);
		pthread_mutexattr_settype(&attr, PTHREAD_MUTEX_RECURSIVE);
		pthread_mutex_init(mt, &attr);
		_mutex = (void *)mt;
		pthread_mutexattr_destroy(&attr);
	}
	else {
		dbgErrPXL("mt malloc failed");
	}
	return _mutex;
}
mutex::~mutex() {
	if (_mutex) {
		pthread_mutex_destroy((pthread_mutex_t*)_mutex);
		free(_mutex);
		_mutex = 0;
	}
}
void mutex::lock() {
	if (_mutex)
		pthread_mutex_lock((pthread_mutex_t*)_mutex);
}
bool mutex::try_lock() {
	return false;
}
void mutex::unlock() {
	if (_mutex)
		pthread_mutex_unlock((pthread_mutex_t *)_mutex);
}
