#include "SmfBase.h"
#include "smf_debug.h"

using namespace smf;
using namespace smf::api;
//
void* SmfBase::operator new(size_t size) {
	return Alloc(size, 8);
}
void* SmfBase::operator new(size_t size,void* ptr) {
	return ptr;
}
void SmfBase::operator delete(void* ptr) {
	Free(ptr);
}

