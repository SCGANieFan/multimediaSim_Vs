#include "PoolList.h"
#include "smf_debug.h"
#include "smf_arm_dsp.h"
#include "SmfString.h"
//
#define dbgPoolPXL(s,...) //dbgMemPXL(s,##__VA_ARGS__)
//
using namespace smf;
///
PoolList::PoolList(IPool** pools, uint32_t max):IPool(false) {
	_pools = pools;
	_count = max;
	memset(_pools, 0, 4*_count);
}
PoolList::~PoolList() {
	for (int i = 0; i < _count; i++) {
		auto pool = _pools[i];
		if (pool) {
			delete pool;
		}
	}
}
bool PoolList::is_valid() const { return _pools; }
bool PoolList::Add(IPool*pool0) {
	for (int i = 0; i < _count; i++) {
		auto& pool = _pools[i];
		if (!pool) {
			pool = pool0;
			return true;
		}
	}
	return false;
}
bool PoolList::Remove(IPool*pool0) {
	for (int i = 0; i < _count; i++) {
		auto& pool = _pools[i];
		if (pool == pool0) {
			delete pool;
			pool = 0;
			return true;
		}
	}
	return true;
}
void* PoolList::alloc(unsigned size, unsigned align) {
	for (int i = 0; i < _count; i++) {
		auto pool = _pools[i];
		if (pool) {
			//dbgPoolPXL("%p,%s,%d,%d",pool,pool->Name(),size,pool->GetAllocLimit());
			auto ptr = pool->Alloc(size,align);
			if (ptr) {
				return ptr;
			}
		}
	};
	return 0;
}
void* PoolList::realloc(void* ptr, unsigned size) {
	for (int i = 0; i < _count; i++) {
		auto pool = _pools[i];
			if (pool) {
				dbgPoolPXL("%p,%s,%d,%d", pool, pool->Name(), size, pool->GetAllocLimit());
				ptr = pool->Realloc(ptr, size);
				if (ptr) {
					return ptr;
				}
			}
	};
	return 0;
}
//void* PoolList::alloc_align(unsigned size, unsigned align) {
//	for (int i = 0; i < _count; i++) {
//		auto pool = _pools[i];
//		if (pool) {
//			auto ptr = pool->Alloc(size, align);
//			if (ptr) {
//				return ptr;
//			}
//		}
//	};
//	return 0;
//}

IPool* PoolList::find(void* ptr) const {
	for (int i = 0; i < _count; i++) {
		auto& pool = _pools[i];
		if (pool && pool->IsInside(ptr)) {
			return pool;
		}
	}
	return 0;
}
bool PoolList::free(void* ptr) {
	auto pool = find(ptr);
	return pool?pool->Free(ptr):false;
}

int PoolList::get_free()const {
	auto size = 0;
	for (int i = 0; i < _count; i++) {
		auto pool = _pools[i];
		if (pool) {
			size += pool->GetFree();
		}
	}
	return size;
}
int PoolList::get_total()const {
	auto size = 0;
	for (int i = 0; i < _count; i++) {
		auto pool = _pools[i];
		if (pool) {
			size += pool->GetTotal();
		}
	}
	return size;
}
int PoolList::get_size(void* ptr)const {
	auto pool = find(ptr);
	return pool ? pool->GetSize(ptr) : 0;
}
//void PoolList::print()const {
//	for (int i = 0; i < _count; i++) {
//		auto pool = _pools[i];
//		if (pool) {
//			pool->Print();
//		}
//	}
//}
char* PoolList::print(char* ptr, char* end) const {
	for (int i = 0; i < _count; i++) {
		auto pool = _pools[i];
		if (pool) {
			ptr=pool->Print(ptr,end);
		}
	}
	return ptr;
}