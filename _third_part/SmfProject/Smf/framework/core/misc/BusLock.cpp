#include "BusLock.h"
#include "SmfPorting.h"
#include "smf_debug.h"
using namespace smf;
BusLock::BusLock() {
	dbgTestPL(); 
	bus_lock();
}
BusLock::~BusLock() { 
	dbgTestPL(); 
	bus_unlock();
}

