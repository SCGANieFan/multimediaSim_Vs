#pragma once
#include "smf_common.h"

namespace smf {
	class ILoader {
	public:
		virtual ~ILoader() {}
		virtual bool Load(void* para) = 0;
		virtual bool Unload(void* para) = 0;
	};
}