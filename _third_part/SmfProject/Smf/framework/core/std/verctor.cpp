#include "vector.hh"
#include "smf_debug.h"
#include "PoolAllocator.h"
#include "HeapAllocator.h"
using namespace smf;
///

#if 0//test vector allocator
void test_vector_allocator() {
	//GlobleAllocator
	vector<int> _vector;

	//
	char _buff[1024];
	TempAlloc _xalloc;

	//BufferAllocator	
	vector<int, BufferAllocator<int>> _vector_buffer(BufferAllocator<int>(_buff, 1024));

	//CallbackAllocator
	vector<int, CallbackAllocator<int>> _vector_callback(CallbackAllocator<int>(&TempAlloc::Alloc, &TempAlloc::Free));

	//TPtrAllocator	
	vector<int, TPtrAllocator<int, TempAlloc>> _vector_tptr{ TPtrAllocator<int, TempAlloc>(&_xalloc) };
	vector<int, TAllocator<int, TempAlloc>> _vector_t{ TAllocator<int, TempAlloc>(_xalloc) };

	//PoolAllocator
	vector<int, PoolAllocator<int>> _vector_pool(PoolAllocator<int>("glb"));
	vector<int, TPtrAllocator<int, IPool>> _vector_poolx(TPtrAllocator<int, IPool>(&Pools().Globle()));

	//HeapAllocator
	vector<int, HeapAllocator<int>> _vector_heap(HeapAllocator<int>(_buff, 1024, 256));
}
#endif