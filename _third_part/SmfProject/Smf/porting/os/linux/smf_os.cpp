#include "SmfPortingThread.h"
#include "SmfFCC.h"
namespace smf {
	using Item_t = ThreadItem_t;
	void* SmfGetThreadID(const char* name) {
		return 0;
	}
	bool SmfThreadUpdateInfo(Item_t* item) {
		if (item->_name == fcc32("IDLE"))
			item->_usage = 10000;
		return true;
	}

	bool os_lock(void) {
		return true;
	}
	void os_unlock(void) {
	}
}