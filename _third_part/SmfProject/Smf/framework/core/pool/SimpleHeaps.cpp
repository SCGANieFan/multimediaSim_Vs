#include "SimpleHeaps.h"
#include "SmfString.h"
#include "smf_common.h"
#include "smf_debug.h"
using namespace smf;

SimpleHeaps::SimpleHeaps(){
	memset(_heaps, 0, sizeof(_heaps));
}
SimpleHeaps:: ~SimpleHeaps() {
	//_heap.Uninit();
}
bool SimpleHeaps::Add(void* buff, unsigned size, unsigned thr) {
	//unique_lock<mutex> lck(_mtx);
	for (auto& heap : _heaps) {
		if (!heap) {
			heap = SimpleHeap::Create((char*)buff, (int)size, thr);
			return !!heap;
		}
	}
	return false;
}
bool SimpleHeaps::Remove(void* buff) {
	dbgTestPPL(buff);
	//unique_lock<mutex> lck(_mtx);
	for (auto& heap : _heaps) {
		if (!heap){
			dbgTestPXL("%p,%p", heap, buff);
			if ((void*)Align4((uint32_t)buff) == heap) {
				dbgTestPPL(heap);
					returnIfErrC(false, heap->GetUsedNodeCount());
					heap = 0;
					return true;
			}
		}
	}
	return false;
}
bool SimpleHeaps::IsValid()const {
	for (auto heap : _heaps) {
		if (heap) {
			return true;
		}
	}
	return false;
}
void* SimpleHeaps::Alloc(unsigned size, unsigned align){
	//unique_lock<mutex> lck(_mtx);
	for (auto heap : _heaps) {
		if (heap) {
			auto ptr = heap->Alloc(size, align);
			if (ptr)
				return ptr;
		}
	}
	return NULL;
}
void* SimpleHeaps::Alloc(unsigned size) {
	return Alloc(size,0);
}
bool SimpleHeaps::Free(void* ptr) {
	//unique_lock<mutex> lck(_mtx);
	for (auto heap : _heaps) {
		if (heap && heap->IsValid(ptr)) {
			return heap->Free(ptr);
		}
	}
	return false;
}
bool SimpleHeaps::Hold(void* ptr) {
	//unique_lock<mutex> lck(_mtx);
	for (auto heap : _heaps) {
		if (heap && heap->IsValid(ptr)) {
			return heap->Hold(ptr);
		}
	}
	return false;
}
void* SimpleHeaps::Realloc(void* ptr0, unsigned size) {
	//unique_lock<mutex> lck(_mtx);
	for (auto heap : _heaps) {
		if (heap) {
			auto ptr = heap->Realloc(ptr0, size);
			if (ptr)
				return ptr;
		}
	}
	return NULL;
}
void* SimpleHeaps::Calloc(unsigned num, unsigned size) {
	auto ptr = Alloc(size * num, 0);
	if (ptr)
		memset(ptr, 0, num * size);
	return ptr;
}
int SimpleHeaps::GetFree()const {
	//unique_lock<mutex> lck(_mtx);
	int sum = 0;
	for (auto heap : _heaps) {
		if (heap) {
			sum += heap->GetFreeSize();
		}
	}
	return sum;
}
int SimpleHeaps::GetTotal()const {
	//unique_lock<mutex> lck(_mtx);
	int sum = 0;
	for (auto heap : _heaps) {
		if (heap) {
			sum += heap->GetTotalSize();
		}
	}
	return sum;
}
int SimpleHeaps::GetSize(void* ptr)const {
	//unique_lock<mutex> lck(_mtx);
	for (auto heap : _heaps) {
		if (heap && heap->IsValid(ptr)) {
			return heap->GetSize(ptr);
		}
	}
	return 0;
}
//void SimpleHeaps::print()const {
//	_heap.Print();
//}
char* SimpleHeaps::Print(char* ptr, char* end) const {
	//ptr = IPool::print(ptr, end);
	//unique_lock<mutex> lck(_mtx);
	for (auto heap : _heaps) {
		if (heap) {
			ptr += snprintf(ptr, end - ptr, "[%s]", (char*)heap - 8);
			ptr = heap->Print(ptr, end);
		}
	}
	return ptr;
}
