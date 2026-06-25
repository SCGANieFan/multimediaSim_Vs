#pragma once
#include "smf_common.h"
namespace smf {
	class IPosition  {
	public:
		virtual uint32_t GetPosition(bool reset = false)const = 0;
	};
}