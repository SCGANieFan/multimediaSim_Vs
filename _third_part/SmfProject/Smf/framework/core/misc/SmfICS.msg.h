#pragma once
#include "stdint.h"
#define SMF_MSG_MAX 0x400

namespace smf {
	namespace ics {
		typedef struct {
			uint8_t route[SMF_MSG_MAX / 4];
		}msg_t;
	}
}

