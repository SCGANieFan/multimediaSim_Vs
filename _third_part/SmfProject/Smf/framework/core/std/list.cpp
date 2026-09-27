#include "list.hh"
#include "smf_debug.h"
#include "PoolAllocator.h"
#include "HeapAllocator.h"
using namespace smf;
///

#if 0//test list allocator
void test_list_allocator() {
	//GlobleAllocator
	list<int> _list;

	//
	char _buff[1024];
	TempAlloc _xalloc;

	//BufferAllocator	
	list<int, BufferAllocator<int>> _list_buffer(BufferAllocator<int>(_buff, 1024));

	//CallbackAllocator
	list<int, CallbackAllocator<int>> _list_callback(CallbackAllocator<int>(&TempAlloc::Alloc, &TempAlloc::Free));

	//TPtrAllocator	
	list<int, TPtrAllocator<int, TempAlloc>> _list_tptr{ TPtrAllocator<int, TempAlloc>(&_xalloc) };
	list<int, TAllocator<int, TempAlloc>> _list_t{ TAllocator<int, TempAlloc>(_xalloc) };

	//PoolAllocator
	list<int, PoolAllocator<int>> _list_pool(PoolAllocator<int>("glb"));
	list<int, TPtrAllocator<int, IPool>> _list_poolx(TPtrAllocator<int, IPool>(&Pools().Globle()));

	//HeapAllocator
	list<int, HeapAllocator<int>> _list_heap(HeapAllocator<int>(_buff, 1024, 256));
}
#endif