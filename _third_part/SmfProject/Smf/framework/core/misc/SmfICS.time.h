#pragma once
#include "stdint.h"

namespace smf {
	namespace ics {
		typedef struct {
			uint32_t rtc;//ms
			uint32_t sys;//ms
		}localtime_t;
	}
}

