#pragma once
#include <stdint.h>

namespace smf {
	class IntrLock {
	public:
		IntrLock();
		~IntrLock();
	protected:
		int _lock;
	};
}