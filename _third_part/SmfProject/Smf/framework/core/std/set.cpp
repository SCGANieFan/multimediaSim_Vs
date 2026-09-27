#include "set.hh"
#include "smf_debug.h"
#include "PoolAllocator.h"
#include "HeapAllocator.h"
using namespace smf;
///

#if 0//test set allocator
void test_set_allocator() {
	//GlobleAllocator
	set_t<int> _set;

	//
	char _buff[1024];
	TempAlloc _xalloc;

	//BufferAllocator	
	set_t<int, BufferAllocator<int>> _set_buffer(BufferAllocator<int>(_buff, 1024));

	//CallbackAllocator
	set_t<int, CallbackAllocator<int>> _set_callback(CallbackAllocator<int>(&TempAlloc::Alloc, &TempAlloc::Free));

	//TPtrAllocator	
	set_t<int, TPtrAllocator<int, TempAlloc>> _set_tptr{ TPtrAllocator<int, TempAlloc>(&_xalloc) };
	set_t<int, TAllocator<int, TempAlloc>> _set_t{TAllocator<int, TempAlloc>(_xalloc)};

	//PoolAllocator
	set_t<int, PoolAllocator<int>> _set_pool(PoolAllocator<int>("glb"));
	set_t<int, TPtrAllocator<int, IPool>> _set_poolx(TPtrAllocator<int, IPool>(&Pools().Globle()));

	//HeapAllocator
	set_t<int, HeapAllocator<int>> _set_heap(HeapAllocator<int>(_buff, 1024, 256));
}
#endif
