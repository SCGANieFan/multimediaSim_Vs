#include "MemoryPoolHeaps.h"
#include "SmfString.h"
#include "smf_debug.h"
using namespace smf;

MemoryPoolHeaps::MemoryPoolHeaps(){
	memset(_heaps, 0, sizeof(_heaps));
}
bool MemoryPoolHeaps::Add(void* buff, unsigned size, unsigned thr) {
	unique_lock<mutex> lck(_mtx);
	for (auto& heap : _heaps) {
		if (!heap) {
			heap = SimpleHeap::Create(buff, size, thr);
			return !!heap;
		}
	}
	return false;
}
bool MemoryPoolHeaps::Remove(void* buff) {
	unique_lock<mutex> lck(_mtx);
	for (int i = 0; i < 8; ++i) {
		auto& heap = _heaps[i];
		if (heap) {
			dbgTestPXL("%p<>%p", heap, buff);
			if ((void*)Align4((uint32_t)buff) == heap) {
				if(i == 0) {
					dbgWarnPL();
					return false;
				}
				dbgTestPPL(heap);
				returnIfErrCS(false, heap->GetUsedNodeCount(), "%p,%u", heap, heap->GetUsedNodeCount());
				heap = 0;
				return true;
			}
		}
	}
	return false;
}
MemoryPoolHeaps:: ~MemoryPoolHeaps() {
	//_heap.Uninit();
}
bool MemoryPoolHeaps::is_valid()const {
	for (auto heap : _heaps) {
		if (heap) {
			return true;
		}
	}
	return false;
}
void* MemoryPoolHeaps::alloc(unsigned size, unsigned align){
	unique_lock<mutex> lck(_mtx);
	for (auto heap : _heaps) {
		if (heap) {
			auto ptr = heap->Alloc(size, align);
			if (ptr)
				return ptr;
		}
	}
	return NULL;
}
//void* MemoryPoolHeaps::alloc(unsigned size) {
//	return alloc_align(size,0);
//}
bool MemoryPoolHeaps::free(void* ptr) {
	unique_lock<mutex> lck(_mtx);
	for (auto heap : _heaps) {
		if (heap && heap->IsValid(ptr)) {
			return heap->Free(ptr);
		}
	}
	return false;
}
bool MemoryPoolHeaps::hold(void* ptr) {
	unique_lock<mutex> lck(_mtx);
	for (auto heap : _heaps) {
		if (heap && heap->IsValid(ptr)) {
			return heap->Hold(ptr);
		}
	}
	return false;
}
void* MemoryPoolHeaps::realloc(void* ptr0, unsigned size) {
	unique_lock<mutex> lck(_mtx);
	for (auto heap : _heaps) {
		if (heap) {
			auto ptr = heap->Realloc(ptr0, size);
			if (ptr)
				return ptr;
		}
	}
	return NULL;
}
//void* MemoryPoolHeaps::calloc(unsigned num, unsigned size) {
//	auto ptr = alloc_align(size * num, 0);
//	if (ptr)
//		memset(ptr, 0, num * size);
//	return ptr;
//}
int MemoryPoolHeaps::get_free()const {
	unique_lock<mutex> lck(_mtx);
	int sum = 0;
	for (auto heap : _heaps) {
		if (heap) {
			sum += heap->GetFreeSize();
		}
	}
	return sum;
}
int MemoryPoolHeaps::get_total()const {
	unique_lock<mutex> lck(_mtx);
	int sum = 0;
	for (auto heap : _heaps) {
		if (heap) {
			sum += heap->GetTotalSize();
		}
	}
	return sum;
}
int MemoryPoolHeaps::get_size(void* ptr)const {
	unique_lock<mutex> lck(_mtx);
	for (auto heap : _heaps) {
		if (heap && heap->IsValid(ptr)) {
			return heap->GetSize(ptr);
		}
	}
	return 0;
}
int MemoryPoolHeaps::get_refs(void* ptr)const {
	unique_lock<mutex> lck(_mtx);
	for (auto heap : _heaps) {
		if (heap && heap->IsValid(ptr)) {
			return heap->GetRefs(ptr);
		}
	}
	return 0;
}
//void MemoryPoolHeaps::print()const {
//	_heap.Print();
//}
char* MemoryPoolHeaps::print(char* ptr, char* end) const {
	ptr = IPool::print(ptr, end);
	unique_lock<mutex> lck(_mtx);
	for (auto heap : _heaps) {
		if (heap) {
			ptr += snprintf(ptr, end - ptr, "[%s]", (char*)heap - 8);
			ptr = heap->Print(ptr, end);
		}
	}
	return ptr;
}


