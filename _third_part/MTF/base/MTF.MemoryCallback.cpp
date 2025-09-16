#if 1
#include "MTF.MemoryCallback.h"
#include "MTF.String.h"
#include "MTF.Printer.h"

using namespace mtf_ns;
bool MTF_MemoryCallback_c::Init(Malloc_cb malloc, Realloc_cb realloc, Calloc_cb calloc, Free_cb free) {
    _malloc = malloc;
    _realloc = realloc;
    _calloc = calloc;
    _free = free;
    return true;
}

bool MTF_MemoryCallback_c::DeInit() {
    return true;
}

mtf_void* MTF_MemoryCallback_c::Malloc(mtf_i32 size)
{
    if (_malloc)
        return _malloc(size);
    return NULL;
}
mtf_void* MTF_MemoryCallback_c::Realloc(mtf_void* block, mtf_i32 size)
{
    if (_realloc)
        return _realloc(block, size);
    return NULL;
}
mtf_void* MTF_MemoryCallback_c::Calloc(mtf_i32 count, mtf_i32 size)
{
    if (_calloc)
        return _calloc(count, size);
    return NULL;
}
mtf_void MTF_MemoryCallback_c::Free(mtf_void* block)
{
    if (_free)
        return _free(block);
}
#if 0
MTF_MemoryCallback_c* MTF_MemoryCallback_c::Create(Malloc_cb malloc, Realloc_cb realloc, Calloc_cb calloc, Free_cb free){
    MTF_MemoryCallback_c* ptr = 0;
    if (malloc) {
        ptr = (MTF_MemoryCallback_c*)malloc(sizeof(MTF_MemoryCallback_c));
    }
    if (ptr) {
        new(ptr)(MTF_MemoryCallback_c);
    }
    return ptr;
}

bool MTF_MemoryCallback_c::Destory(MTF_MemoryCallback_c* ptr, Malloc_cb malloc, Realloc_cb realloc, Calloc_cb calloc, Free_cb free) {
    if (ptr) {
        if (free) {
            free(ptr);
        }
        else {
            return false;
        }
    }
    return true;
}
#endif
namespace mtf_ns {

}

#endif