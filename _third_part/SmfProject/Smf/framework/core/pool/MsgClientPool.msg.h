#pragma once
#include "SmfMsg.h"

namespace smf {
	namespace msg {
		namespace pool {
			enum class POOL {
				null = 0,
				Pool0 = SMF_MSG_POOL,
				Pool1 = Pool0+4,
				Pool2 = Pool1+4,
				Pool3 = Pool2+4,

				Alloc = 0,
				AllocResponse,
				Free,
				FreeResponse,
			};

			constexpr POOL operator+(POOL a, POOL b) {
				return (POOL)((uint16_t)a + (uint16_t)b);
			}
			constexpr POOL operator-(POOL a, POOL b) {
				return (POOL)((uint16_t)a - (uint16_t)b);
			}

			struct Alloc_t {
				uint64_t name;
				uint32_t size;
				uint32_t align;
				uint64_t buff;
			};

			struct Free_t {
				uint64_t name;
				uint64_t buff;
			};
		}
	}
}

