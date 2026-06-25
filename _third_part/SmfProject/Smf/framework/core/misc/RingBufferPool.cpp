#include "RingBufferPool.h"
#include "smf_debug.h"
using namespace smf;

RingBufferPool::~RingBufferPool() {
	Reset(0);
}
bool RingBufferPool::Reset(int max, void* buff) {
	if (_buff0) {
		smf::Free(_buff0);
		_buff0 = 0;
	}
	if (!buff) {
		buff = _buff0 = (char*)smf::Alloc(max,64);
	}
	if (max) {
		returnIfErrC(false,!buff);
		_ptr = _buff = (char*)buff;
		_end = _ptr += max;
	}
	return true;
}
void* RingBufferPool::Alloc(unsigned size) {
	if (_ptr + size <= _end) {
		auto ptr = _ptr;
		_ptr += size;
		return ptr;
	}
	else {
		_ptr = _buff + size;
		return _buff;
	}
}
bool RingBufferPool::Free(void* ptr) {
	return true;
}

