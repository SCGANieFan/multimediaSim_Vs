#include "unique_ptr.h"
#include "smf_debug.h"
#include <utility>
#include "SmfDef.h"
using namespace smf;
///
static void resetX(void*& _data, IPool* pool, const void* data, int size, int align, bool hold) {
	if (!pool)pool = &BasePool();
	if (_data) {
		pool->Free(_data);
		_data = 0;
	}
	if (data) {
		_data = (void*)data;
		if (hold)
			pool->Hold((void*)data);
	}
	else if (size) {
		_data = pool->Alloc(size, align);
		returnIfErrCS0(!_data,"%d,%d", size, align);
	}
}

void XVoidPtr::ResetX(void* this_, const void* data, int size, int align, bool hold) {
	auto x = (VoidPtr*)this_;
	resetX(x->_data,0,data, size, align, hold);
}

void XVoidBuffer::ResetX(void* this_, const void* data, int size, int align, bool hold) {
	auto x = (VoidBuffer*)this_;
	resetX(x->_data, 0, data, size, align, hold);
	x->_size = size;
}

void XPoolPtr::ResetX(void* this_, const void* data, int size, int align, bool hold) {
	auto x = (PoolPtr*)this_;
	resetX(x->_data, x->_pool, data, size, align, hold);
}

void XPoolBuffer::ResetX(void* this_, const void* data, int size, int align, bool hold) {
	auto x = (PoolBuffer*)this_;
	resetX(x->_data, x->_pool, data, size, align, hold);
	x->_size = size;
}
