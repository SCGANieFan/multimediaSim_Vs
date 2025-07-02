#pragma once
#include <stdlib.h>
#include "SmfBase.h"

namespace smf {

		void* Alloc(unsigned size, unsigned align) {
			return malloc(size);
		}
		void* Alloc(unsigned size) {
			return malloc(size);
		}
		void* Calloc(unsigned count, unsigned size) {
			return calloc(count, size);
		}
		void* Realloc(void* ptr, unsigned size) {
			return realloc(ptr, size);
		}
		void Free(void* ptr) {
			free(ptr);
		}

		//
		void* SmfBase::operator new(size_t size) {
			return Alloc(size, 8);
		}
		void* SmfBase::operator new(size_t size, void* ptr) {
			return ptr;
		}
		void SmfBase::operator delete(void* ptr) {
			Free(ptr);
		}
}
