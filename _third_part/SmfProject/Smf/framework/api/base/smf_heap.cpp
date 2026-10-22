#include "smf_heap.h"
#include "SmfDef.h"
#include "SimpleHeap.h"
#include "mutex.h"
#include "smf_debug.h"
using namespace smf;

EXTERNC void* smf_heap_init(void* addr, uint32_t size) {
	returnIfErrC(0, !addr || !size);
	auto hsize = sizeof(SimpleHeap);
	returnIfErrC(0, size < hsize);
	unique_lock<mutex> lck(Mutex());	
	auto heap = new(addr) SimpleHeap();
	returnIfErrC(0, !heap);
	heap->Initialize((uint8_t*)addr + hsize, size - hsize);
	return heap;
}
EXTERNC void* smf_heap_malloc(void* heap, uint32_t size, uint32_t align) {
	returnIfErrC(0, !heap);
	unique_lock<mutex> lck(Mutex());
	return ((SimpleHeap*)heap)->Alloc(size, align);
}
EXTERNC void* smf_heap_calloc(void* heap, uint32_t num, uint32_t size) {
	returnIfErrC(0, !heap);
	unique_lock<mutex> lck(Mutex());
	return ((SimpleHeap*)heap)->Calloc(num, size);
}
EXTERNC void* smf_heap_realloc(void* heap, void* ptr, uint32_t size) {
	returnIfErrC(0, !heap);
	unique_lock<mutex> lck(Mutex());
	return ((SimpleHeap*)heap)->Realloc(ptr, size);
}
EXTERNC void smf_heap_free(void* heap, void* ptr) {
	returnIfErrC0(!heap);
	unique_lock<mutex> lck(Mutex());
	((SimpleHeap*)heap)->Free(ptr);
}
