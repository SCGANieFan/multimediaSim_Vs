#if 0
#include "gasf_memory.h"

using namespace GASF_NAME_SPACE;
#define CHECK_STEP_ON_MEMORY 0
#if  CHECK_STEP_ON_MEMORY
#define CHECK_STEP_ON_MEMORY_MAGIC (0x0F0F0F0F0F0F0F0F)
#endif


void GasfMemory_c::Init(GasfBasePort_t* base_porting) {
	_bp = base_porting;
	for (uint32_t n = 0; n < _allocListNum; n++) {
		_allocList[n] = 0;
	}
}
void GasfMemory_c::DeInit() {
	_bp = 0;
	for (uint32_t n = 0; n < _allocListNum; n++) {
		_allocList[n] = 0;
	}
}
void* GasfMemory_c::Malloc(int32_t size) {
#if  CHECK_STEP_ON_MEMORY
	size += 8;
#endif
	void* ptr = _bp->malloc_cb(size);
	if (!ptr)
		return 0;
#if  CHECK_STEP_ON_MEMORY
	* ((u64*)ptr) = CHECK_STEP_ON_MEMORY_MAGIC;
	ptr = (u8*)ptr + 8;
#endif
	for (uint32_t n = 0; n < _allocListNum; n++) {
		if (!_allocList[n]) {
			_allocList[n] = ptr;
			return _allocList[n];
		}
	}
	return 0;
}

void* GasfMemory_c::Realloc(void* bufOri, int32_t size) {
#if  CHECK_STEP_ON_MEMORY
	bufOri = (u8*)bufOri - 8;
	u64 magic = *(u64*)bufOri;
	ASSERT(magic == CHECK_STEP_ON_MEMORY_MAGIC);
	void* ptr = _bp->realloc_cb(bufOri, size + 8);
#else
	void* ptr = _bp->realloc_cb(bufOri, size);
#endif
	if (!ptr)
		return 0;
#if  CHECK_STEP_ON_MEMORY
	* ((u64*)ptr) = CHECK_STEP_ON_MEMORY_MAGIC;
	ptr = (u8*)ptr + 8;
#endif
	for (uint32_t n = 0; n < _allocListNum; n++) {
		if (_allocList[n] == bufOri) {
			_allocList[n] = ptr;
			return _allocList[n];
		}
	}
	return 0;
}

void GasfMemory_c::Free(void* ptr) {
	for (uint32_t n = 0; n < _allocListNum; n++) {
		if (_allocList[n] == ptr) {
#if  CHECK_STEP_ON_MEMORY
			_allocList[n] = (u8*)_allocList[n] - 8;
			u64 magic = *(u64*)_allocList[n];
			ASSERT(magic == CHECK_STEP_ON_MEMORY_MAGIC);
#endif
			_bp->free_cb(_allocList[n]);
			_allocList[n] = 0;
			return;
		}
	}
}


void GasfMemory_c::FreeAll() {
	for (uint32_t n = 0; n < _allocListNum; n++) {
		if (_allocList[n]) {
#if  CHECK_STEP_ON_MEMORY
			_allocList[n] = (u8*)_allocList[n] - 8;
			u64 magic = *(u64*)_allocList[n];
			ASSERT(magic == CHECK_STEP_ON_MEMORY_MAGIC);
#endif
			_bp->free_cb(_allocList[n]);
			_allocList[n] = 0;
		}
	}
}







#endif