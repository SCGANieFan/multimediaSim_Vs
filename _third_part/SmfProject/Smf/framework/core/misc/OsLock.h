#pragma once
#include <stdint.h>

namespace smf {
	class OsLock {
	public:
		OsLock();
		OsLock(bool en);
		~OsLock();
	protected:
		bool _lock = false;
	};
}