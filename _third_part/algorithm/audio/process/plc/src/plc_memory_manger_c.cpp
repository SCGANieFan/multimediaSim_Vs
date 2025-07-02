#pragma once

#include "plc_memory_manger_c.h"

#define CHECK_STEP_ON_MEMORY 1

#if  CHECK_STEP_ON_MEMORY
#define CHECK_STEP_ON_MEMORY_MAGIC (0x0F0F0F0F0F0F0F0F)
#endif


void plc_memory_manger_c::Init(plc_base_port_c*base_porting) {
	_basePorting = base_porting;
	for (void*& p : _allocList) {
		p = 0;
	}
}

void* plc_memory_manger_c::malloc(i32 size) {
#if  CHECK_STEP_ON_MEMORY
	size += 8;
#endif
	void* ptr = _basePorting->malloc(size);
	if (!ptr)
		return 0;
#if  CHECK_STEP_ON_MEMORY
	*((u64*)ptr) = CHECK_STEP_ON_MEMORY_MAGIC;
	ptr = (u8*)ptr + 8;
#endif
	for (void*& p : _allocList){
		if (!p){
			p = ptr;
			return p;
		}
	}
	return 0;
}

void plc_memory_manger_c::free(void *ptr) {
	for (void*& p : _allocList) {
		if (p== ptr) {
#if  CHECK_STEP_ON_MEMORY
			p = (u8*)p - 8;
			u64 magic = *(u64*)p;
			ALGO_ASSERT(magic == CHECK_STEP_ON_MEMORY_MAGIC);
#endif
			_basePorting->free(p);
			p = 0;
			return;
		}
	}
}


void plc_memory_manger_c::FreeAll () {
	for (void*& p : _allocList) {
		if (p) {
#if  CHECK_STEP_ON_MEMORY
			p = (u8*)p - 8;
			u64 magic = *(u64*)p;
			ALGO_ASSERT(magic == CHECK_STEP_ON_MEMORY_MAGIC);
#endif
			_basePorting->free(p);
			p = 0;
		}
	}
}







