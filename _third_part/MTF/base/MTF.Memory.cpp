#if 1
#include "MTF.Memory.h"
#include "MTF.Memory_c.h"
//#include "MTF.String.h"
#include "MTF.Printer.h"

using namespace mtf_ns;

namespace mtf_ns {

MTF_IMemory_c* MTF_MemoryGetMemory(const char* name) {
    return MTF_Memory_c::GetMemory(name);
}

void* Malloc(mtf_i32 size) {
    return MTF_Memory_c::Malloc_s(size);
}
void* MallocName(const char* name, mtf_i32 size) {
    return MTF_Memory_c::MallocName(name, size);
}

void* Realloc(void* block, int32_t size) {
    return MTF_Memory_c::Realloc(block, size);
}
void* ReallocName(const char* name, void* block, int32_t size) {
    return MTF_Memory_c::ReallocName(name, block, size);
}
void* Calloc(int32_t count, int32_t size) {
    return MTF_Memory_c::Calloc(count, size);
}
void* CallocName(const char* name, int32_t count, int32_t size) {
    return MTF_Memory_c::CallocName(name, count, size);
}
void Free(void* block) {
    MTF_Memory_c::Free(block);
}
void FreeName(const char* name, void* block) {
    MTF_Memory_c::FreeName(name, block);
}
bool MTF_MemoryRegister(const char* name, mtf_u8* buf, mtf_u32 len) {
    return MTF_Memory_c::Register(name, buf, len);
}
bool MTF_MemoryRegister(const char* name, Malloc_cb malloc, Realloc_cb realloc, Calloc_cb calloc, Free_cb free) {
    return MTF_Memory_c::Register(name, malloc, realloc, calloc, free);
}

}

#endif