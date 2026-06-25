#include "OsLock.h"
#include "SmfPorting.h"
using namespace smf;

OsLock::OsLock() {
	_lock = os_lock();
}
OsLock::OsLock(bool en) {
	if (en)_lock = os_lock();
}
OsLock::~OsLock() {
	if(_lock)os_unlock();
}
