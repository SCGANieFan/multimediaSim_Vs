#pragma once
#include <stdint.h>
#include <stddef.h>

namespace smf {
	namespace api {
		class SmfBase {
		public:
			static void* operator new(size_t size);
			static void* operator new(size_t size, void* ptr);
			static void operator delete(void* ptr);
		};
	}

	void* Alloc(unsigned size, unsigned align);
	void* Calloc(unsigned count, unsigned size);
	void* Realloc(void* ptr, unsigned size);
	void Free(void*& ptr);
}
