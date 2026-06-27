#include "MemoryPoolHeapXs.h"
#include "smf_debug.h"
using namespace smf;

MemoryPoolHeapXs::MemoryPoolHeapXs(){
}
bool MemoryPoolHeapXs::Initialize(smf_pair_t* pairs, unsigned count, unsigned nodemax) {
	return _heap.Initialize(pairs, count, nodemax);
}
MemoryPoolHeapXs:: ~MemoryPoolHeapXs() {
	_heap.Deinitialize();
}
bool MemoryPoolHeapXs::is_valid()const {
	return _heap.IsValid();
}
void* MemoryPoolHeapXs::alloc(unsigned size, unsigned align){
	unique_lock<mutex> lck(_mtx);
	return _heap.Alloc(size);
}
//void* MemoryPoolHeapXs::alloc(unsigned size0) {
//	unique_lock<mutex> lck(_mtx);
//	return _heap.Alloc(size0);
//}
bool MemoryPoolHeapXs::free(void* ptr) {
	unique_lock<mutex> lck(_mtx);
	return _heap.Free(ptr);
}
//bool MemoryPoolHeapXs::hold(void* ptr) {
//	unique_lock<mutex> lck(_mtx);
//	return _heap.Hold(ptr);
//}
//void* MemoryPoolHeapXs::realloc(void* ptr, unsigned size) {
//	unique_lock<mutex> lck(_mtx);
//	return _heap.Realloc(ptr, size);
//}
////void* MemoryPoolHeapXs::calloc(unsigned num, unsigned size) {
//	unique_lock<mutex> lck(_mtx);
//	return _heap.Calloc(num, size);
//}
int MemoryPoolHeapXs::get_free()const {
	return _heap.GetFreeSize();
}
int MemoryPoolHeapXs::get_total()const {
	return _heap.GetTotalSize();
}
int MemoryPoolHeapXs::get_size(void* ptr)const {
	return _heap.GetSize(ptr);
}
//int MemoryPoolHeapXs::get_refs(void* ptr)const {
//	return _heap.GetRefs(ptr);
//}
//void MemoryPoolHeapXs::print()const {
//	_heap.Print();
//}
char* MemoryPoolHeapXs::print(char* ptr, char* end) const {
	ptr = IPool::print(ptr, end);
	return _heap.Print(ptr, end);
}

