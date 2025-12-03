#include "gaapi_memory.h"

using namespace gaapi_ns;
void GaapiMemory_c::Init(GaapiBasePort_t* base_porting) {
	_bp = base_porting;
	for (void*& p : _allocList) {
		p = 0;
	}
}
void GaapiMemory_c::DeInit() {
	_bp = 0;
	for (void*& p : _allocList) {
		p = 0;
	}
}
void* GaapiMemory_c::Malloc(int32_t size) {
	void* ptr = _bp->malloc_cb(size);
	if (!ptr)
		return 0;
	for (void*& p : _allocList) {
		if (!p) {
			p = ptr;
			return p;
		}
	}
	return 0;
}

void* GaapiMemory_c::Realloc(void* bufOri, int32_t size) {
	void* ptr = _bp->realloc_cb(bufOri, size);
	if (!ptr)
		return 0;
	for (void*& p : _allocList) {
		if (p == bufOri) {
			p = ptr;
			return p;
		}
	}
	return 0;
}

void GaapiMemory_c::Free(void* ptr) {
	for (void*& p : _allocList) {
		if (p == ptr) {
			_bp->free_cb(p);
			p = 0;
			return;
		}
	}
}


void GaapiMemory_c::FreeAll() {
	for (void*& p : _allocList) {
		if (p) {
			_bp->free_cb(p);
			p = 0;
		}
	}
}







