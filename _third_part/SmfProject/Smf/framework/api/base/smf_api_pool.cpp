#include "IPool.h"
#include "SmfDef.h"
#include "smf_debug.h"
using namespace smf;

EXTERNC void* smf_malloc(uint32_t size) {
	return Pools().Globle().Alloc(size);
}
EXTERNC void* smf_realloc(void* ptr, uint32_t size) {
	return Pools().Globle().Realloc(ptr, size);
}
EXTERNC void* smf_calloc(uint32_t num, uint32_t size) {
	return Pools().Globle().Calloc(num, size);
}
EXTERNC void smf_free(void* ptr) {
	Pools().Globle().Free(ptr);
}

EXTERNC void* smf_find_malloc(const char* plname, uint32_t size) {
	return Pools().Alloc(plname, size);
}
EXTERNC void* smf_find_realloc(const char* plname, void* ptr, uint32_t size) {
	return Pools().Realloc(plname, ptr, size);
}
EXTERNC void* smf_find_calloc(const char* plname, uint32_t num, uint32_t size) {
	return Pools().Calloc(plname, num, size);
}
EXTERNC void smf_find_free(const char* plname, void* ptr) {
	Pools().Free(plname, ptr);
}

EXTERNC void* smf_pool_get(const char* plname) {
	return Pools().Find(plname);
}
EXTERNC void* smf_pool_malloc(void* pool, uint32_t size) {
	return ((IPool*)pool)->Alloc(size);
}
EXTERNC void* smf_pool_realloc(void* pool, void* ptr, uint32_t size) {
	return ((IPool*)pool)->Realloc(ptr, size);
}
EXTERNC void* smf_pool_calloc(void* pool, uint32_t num, uint32_t size) {
	return ((IPool*)pool)->Calloc(num, size);
}
EXTERNC void smf_pool_free(void* pool, void* ptr) {
	((IPool*)pool)->Free(ptr);
}

EXTERNC void* smf_debug_malloc(void* pool, uint32_t size, const char* file, int line) {
	auto ptr = ((IPool*)pool)->Alloc(size);
	dbgTestPXL("%p[%d]%s/%d", ptr, size, (file ? dbgPath(file) : ""), line);
	return ptr;
}
EXTERNC void* smf_debug_realloc(void* pool, void* ptr, uint32_t size, const char* file, int line) {
	auto ptrnew = ((IPool*)pool)->Realloc(ptr, size);
	dbgTestPXL("%p/%p[%d]%s/%d", ptrnew, ptr, size, (file ? dbgPath(file) : ""), line);
	return ptrnew;
}
EXTERNC void* smf_debug_calloc(void* pool, uint32_t num, uint32_t size, const char* file, int line) {
	auto ptr = ((IPool*)pool)->Calloc(num, size);
	dbgTestPXL("%p[%dx%d]%s/%d", ptr, num, size, (file ? dbgPath(file) : ""), line);
	return ptr;
}
EXTERNC void smf_debug_free(void* pool, void* ptr, const char* file, int line) {
	dbgTestPXL("%p,%s/%d", ptr, (file ? dbgPath(file) : ""), line);
	((IPool*)pool)->Free(ptr);
}

EXTERNC void* smf_malloc_uncache(uint32_t size) {
	return IPool::Globle().AllocUncache(size);
}

EXTERNC bool smf_free_uncache(void* addr) {
	return IPool::Globle().FreeUncache(addr);
}

EXTERNC void* smf_find_pool_alloc(const char* plname, uint32_t size) {
	return smf_find_malloc(plname, size);
}

EXTERNC void smf_find_pool_free(const char* plname, void* ptr) {
	smf_find_free(plname, ptr);
}
