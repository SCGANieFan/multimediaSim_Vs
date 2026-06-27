#include "MemoryPoolHeapX.h"
#include "smf_debug.h"
using namespace smf;
#if 0
MemoryPoolHeap::MemoryPoolHeap(){
}
bool MemoryPoolHeap::Initialize(void* buff, unsigned size, unsigned thr) {
	return _heap.Initialize(buff, size, thr);
}
MemoryPoolHeap:: ~MemoryPoolHeap() {
	_heap.Uninit();
}
bool MemoryPoolHeap::is_valid()const {
	return _heap.IsValid();
}
void* MemoryPoolHeap::alloc_align(unsigned size, unsigned align){
	unique_lock<mutex> lck(_mtx);
	return _heap.Alloc(size,align);
}
void* MemoryPoolHeap::alloc(unsigned size0) {
	unique_lock<mutex> lck(_mtx);
	return _heap.Alloc(size0);
}
bool MemoryPoolHeap::free(void* ptr) {
	unique_lock<mutex> lck(_mtx);
	return _heap.Free(ptr);
}
bool MemoryPoolHeap::hold(void* ptr) {
	unique_lock<mutex> lck(_mtx);
	return _heap.Hold(ptr);
}
void* MemoryPoolHeap::realloc(void* ptr, unsigned size) {
	unique_lock<mutex> lck(_mtx);
	return _heap.Realloc(ptr, size);
}
//void* MemoryPoolHeap::calloc(unsigned num, unsigned size) {
//	unique_lock<mutex> lck(_mtx);
//	return _heap.Calloc(num, size);
//}
int MemoryPoolHeap::get_free()const {
	return _heap.GetFreeSize();
}
int MemoryPoolHeap::get_total()const {
	return _heap.GetTotalSize();
}
int MemoryPoolHeap::get_size(void* ptr)const {
	return _heap.GetSize(ptr);
}
int MemoryPoolHeap::get_refs(void* ptr)const {
	return _heap.GetRefs(ptr);
}
//void MemoryPoolHeap::print()const {
//	_heap.Print();
//}
char* MemoryPoolHeap::print(char* ptr, char* end) const {
	ptr = IPool::print(ptr, end);
	return _heap.Print(ptr, end);
}
#endif
