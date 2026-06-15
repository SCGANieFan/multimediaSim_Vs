#pragma once
#include "smf_common.h"

namespace smf {
	template<uint32_t N>
	struct NSmfFifo : public smf_fifo_t {
		char buff[N];
	};
}
