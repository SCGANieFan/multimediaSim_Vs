#pragma once
#include "SmfICS.h"

namespace smf {
	namespace ics {
		typedef struct mmgr_t {
			uint32_t buff[12 * 32 / 4 + 10 * 10 / 4 + 24 * 4 / 4 + 4 / 4];

			//uint32_t item[3 * 32];
			//uint32_t focus[(10 * 10 + 4 + 3) / 4];
			//uint32_t policy[4][6];
			//uint8_t lock;
			//uint8_t cpuids;
			//uint8_t revs[2];
		}mmgr_t;
	}
}

