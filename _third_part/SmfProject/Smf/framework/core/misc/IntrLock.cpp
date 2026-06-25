#include "IntrLock.h"
#include "SmfPorting.h"
using namespace smf;

IntrLock::IntrLock():_lock(intr_lock()) {
}
IntrLock::~IntrLock() {
	intr_unlock(_lock);
}
