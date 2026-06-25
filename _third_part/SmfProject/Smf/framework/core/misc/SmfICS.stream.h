#pragma once
#include "stdint.h"

namespace smf {
	namespace ics {
		typedef struct stream_t {
			uint64_t buff[128 + 128 / 8 * 2 + 32 / 8 * 4 + 32 / 8];
		}stream_t;
	}
}

