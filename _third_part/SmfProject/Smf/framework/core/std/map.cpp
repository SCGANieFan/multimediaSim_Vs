#include "map.hh"
#include "smf_debug.h"
#include "PoolAllocator.h"
#include "HeapAllocator.h"
using namespace smf;
///

#if 0//test map allocator
void test_map_allocator(){
	//GlobleAllocator
	map_t<int,int> _map;
	//
	char _buff[1024];
	TempAlloc _xalloc;

	//BufferAllocator
	map_t<int, int, BufferAllocator<std::pair<const int, int>>> _map_buffer(BufferAllocator<std::pair<const int, int>>(_buff, 1024));

	//CallbackAllocator
	map_t<int, int, CallbackAllocator<std::pair<const int, int>>> _map_callback(CallbackAllocator<std::pair<const int, int>>(&TempAlloc::Alloc, &TempAlloc::Free));

	//TPtrAllocator	
	map_t<int, int, TPtrAllocator<std::pair<const int, int>, TempAlloc>> _map_tptr{ TPtrAllocator<std::pair<const int, int>, TempAlloc>(&_xalloc) };

	//TAllocator
	map_t<int, int, TAllocator<std::pair<const int, int>, TempAlloc>> _map_t{ TAllocator<std::pair<const int, int>, TempAlloc>(_xalloc) };

	//PoolAllocator
	map_t<int, int, PoolAllocator<std::pair<const int, int>>> _map_pool(PoolAllocator<std::pair<const int, int>>("glb"));

	map_t<int, int, TPtrAllocator<std::pair<const int, int>, IPool>> _map_poolx(TPtrAllocator<std::pair<const int, int>, IPool>(&Pools().Globle()));

	//HeapAllocator
	map_t<int, int, HeapAllocator<std::pair<const int, int>>> _map_heap(HeapAllocator<std::pair<const int, int>>(_buff, 1024, 256));
}
#endif
