#include "OggMemoryManger_c.h"

using namespace ogg_ns;
void OggMemoryManger_c::Init(OggBasePorting_t*base_porting) {
	_basePorting = base_porting;
	for (void*& p : _allocList) {
		p = 0;
	}
}

void* OggMemoryManger_c::malloc(int32_t size) {
	void* ptr = _basePorting->malloc_cb(size);
	if (!ptr)
		return 0;
	for (void*& p : _allocList){
		if (!p){
			p = ptr;
			return p;
		}
	}
	return 0;
}

void* OggMemoryManger_c::realloc(void *bufOri, int32_t size) {
	void* ptr = _basePorting->realloc_cb(bufOri,size);
	if (!ptr)
		return 0;
	for (void*& p : _allocList){
		if (p==bufOri){
			p = ptr;
			return p;
		}
	}
	return 0;
}

void OggMemoryManger_c::free(void *ptr) {
	for (void*& p : _allocList) {
		if (p== ptr) {
			_basePorting->free_cb(p);
			p = 0;
			return;
		}
	}
}


void OggMemoryManger_c::FreeAll () {
	for (void*& p : _allocList) {
		if (p) {
			_basePorting->free_cb(p);
			p = 0;
		}
	}
}







