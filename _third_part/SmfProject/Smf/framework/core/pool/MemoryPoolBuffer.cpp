#include "MemoryPoolBuffer.h"
#include "stdio.h"
#include "smf_debug.h"
using namespace smf;
MemoryPoolBuffer::MemoryPoolBuffer() 
{//dbgTestPL();
}
MemoryPoolBuffer::MemoryPoolBuffer(void* buff, int size)
{//dbgTestPL();
	returnIfErrC0(!Initialize(buff, size));
}
MemoryPoolBuffer:: ~MemoryPoolBuffer() {
	//dbgMemPXL("%p,used:%d,max:%d",this, _used, _used_max);
}

bool MemoryPoolBuffer::Initialize(void* buff, int size) {
	returnIfErrC(false, !buff);
	returnIfErrC(false, !size);
	//returnIfErrC(false, size > 1024*32*4);
	_begin = (uint32_t*)(((uint32_t)buff + 3) & ~3);
	_end = (uint32_t*)(((uint32_t)buff + size) & ~3);
	_ptr = _begin;
	return true;
}
bool MemoryPoolBuffer::Reset() {
	_ptr = _begin;
	return IsValid();
}
//void* MemoryPoolBuffer::alloc(unsigned size) {
//	uint32_t size32 = (size + 3) >> 2;
//	returnIfErrC(0, _ptr + size32 > _end);
//	auto ptr = _ptr;
//	_ptr = ptr + size32;
//	return ptr;
//}
bool MemoryPoolBuffer::free(void* ptr) {
	return true;
}

bool MemoryPoolBuffer::is_valid()const {
	return _ptr;
}
void* MemoryPoolBuffer::alloc(unsigned size, unsigned align) {
	auto ptr = align ? (uint32_t*)(((uint32_t)_ptr + align - 1) / align * align) : _ptr;
	_ptr = ptr + (size >> 2);
	return ptr;
}
//void* MemoryPoolBuffer::realloc(void* ptr, unsigned size) {
//	return _heap.Realloc(ptr, size);
//}
//void* MemoryPoolBuffer::calloc(unsigned num, unsigned size) {
//	return _heap.Calloc(num, size);
//}
int MemoryPoolBuffer::get_free()const {
	return (char*)_end - (char*)_ptr;
}
int MemoryPoolBuffer::get_total()const {
	return (char*)_end - (char*)_begin;
}
//void MemoryPoolBuffer::print()const {
//	dbgTestPXL("%p,%p,%p",_begin,_ptr,_end);
//}
char* MemoryPoolBuffer::print(char* ptr, char* end) const {
	ptr+=snprintf(ptr,end-ptr,"%p,%p,%p", _begin, _ptr, _end);
	return ptr;
}