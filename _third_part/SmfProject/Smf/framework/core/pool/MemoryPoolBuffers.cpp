#include "MemoryPoolBuffers.h"
#include "SmfDef.h"
#include "smf_debug.h"
#include "stdlib.h"
using namespace smf;
MemoryPoolBuffers::MemoryPoolBuffers() {
}
MemoryPoolBuffers::MemoryPoolBuffers(smf_pair_t* pairs, uint32_t count){
	returnIfErrC0(!Initialize(pairs, count));
}
MemoryPoolBuffers::~MemoryPoolBuffers() {
	Deinitialize();
}
bool MemoryPoolBuffers::Deinitialize() {
	if (_pairs) {
		smf::Free(_pairs);
		_pairs = 0;
	}
	return true;
}

bool MemoryPoolBuffers::Initialize(smf_pair_t* pairs, uint32_t count) {
	returnIfErrC(false, !pairs);
	returnIfErrC(false, !count);
	returnIfErrC(false, _pairs);
	auto total = 0ul;	
	_pairs = (smf_pair_t*)smf::Alloc(sizeof(smf_pair_t) * count);
	returnIfErrC(false, !_pairs);
	for (int i = 0; i < count; i++) {
		auto& src = pairs[i];
		auto& dst = _pairs[i];
		dst.data = src.data;
		dst.size = src.size;
		total += src.size;
	}
	//sort
	qsort(_pairs, count, sizeof(smf_pair_t)
		, [](const void* a, const void* b) {
		auto pair_a = (const smf_pair_t*)a;
		auto pair_b = (const smf_pair_t*)b;
		return (int)pair_a->size - (int)pair_b->size;
		});
	//
	_pairc = count;
	_total = total;
	_free = total;
	return true;
}

bool MemoryPoolBuffers::free(void* ptr) {
	for (int i = 0; i < _pairc; i++) {
		auto pair = _pairs + i;
		if (pair->data == ptr) {
			if (!(pair->size & 0x80000000))return true;
			pair->size &= ~0x80000000;
			_free += pair->size;
			return true;
		}
	}
	return false;
}

bool MemoryPoolBuffers::is_valid()const {
	return _pairs;
}
void* MemoryPoolBuffers::alloc(unsigned size, unsigned align) {
	for (int i = 0; i < _pairc; i++) {
		auto pair = _pairs + i;
		if (!(pair->size & 0x80000000) && size <= pair->size) {
			_free -= pair->size;
			pair->size |= 0x80000000;
			return pair->data;
		}
	}
	return 0;
}
//void* MemoryPoolBuffers::realloc(void* ptr, unsigned size) {
//	return _heap.Realloc(ptr, size);
//}
//void* MemoryPoolBuffers::calloc(unsigned num, unsigned size) {
//	return _heap.Calloc(num, size);
//}
int MemoryPoolBuffers::get_free()const {
	return _free;
}
int MemoryPoolBuffers::get_total()const {
	return _total;
}
//void MemoryPoolBuffers::print()const {
//	dbgTestPXL("%p,%p,%p",_begin,_ptr,_end);
//}
char* MemoryPoolBuffers::print(char* ptr, char* end) const {
	ptr = IPool::print(ptr, end);
	for (int i = 0; i < _pairc; i++) {
		auto pair = _pairs + i;
		ptr = snprintf(ptr, end, "(%u:%p/%u)", pair->size >> 31, pair->data, pair->size & 0x7fffffff);
	}
	return ptr;
}