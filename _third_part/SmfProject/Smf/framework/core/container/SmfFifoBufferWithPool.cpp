#include "SmfFifoBufferWithPool.h"
#include "smf_debug.h"
using namespace smf;

SmfFifoBufferWithPool::~SmfFifoBufferWithPool() {
	Uninit();
}
SmfFifoBufferWithPool::SmfFifoBufferWithPool(int max, int align, IPool* pool) {
	if (max)
		Initialize(max, align, pool);
}
bool SmfFifoBufferWithPool::Initialize(uint32_t max, int align, IPool* pool) {
	Pool(pool);
	//
	auto data = Alloc(max, align); //dbgTestPPL(data);
	returnIfErrC(false, !data);
	Base::Initialize(&_fifoD, data, max);
	return true;
}
void SmfFifoBufferWithPool::Uninit() {
	Free((void*&)_fifoD.data);
}

