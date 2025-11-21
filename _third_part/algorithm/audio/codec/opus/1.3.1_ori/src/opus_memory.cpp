#include "opus_memory.h"
#include <string.h>


#ifdef OPUS_IN_ROM
#define OPUS_MEMORY_FUNCLIST_DEFALT_PTR ((void**)(0x400080000))
#else
static void* opus_memory_funclist[3];
static void* p_register = (void *)&opus_memory_funclist;
#define OPUS_MEMORY_FUNCLIST_DEFALT_PTR ((void**)(&p_register))
#endif

void opus_memory_register_inner(OpusMemory_t *memory)
{
    OpusMemory_t* pOpusMemory = (OpusMemory_t*)(*OPUS_MEMORY_FUNCLIST_DEFALT_PTR);
    pOpusMemory->malloc_cb = memory->malloc_cb;
    pOpusMemory->realloc_cb = memory->realloc_cb;
    pOpusMemory->free_cb = memory->free_cb;
}

void *opus_malloc_inner(uint32_t size)
{
    return ((OpusMemory_t*)(*OPUS_MEMORY_FUNCLIST_DEFALT_PTR))->malloc_cb(size);
}

void *opus_realloc_inner(void *rmem, uint32_t newsize)
{
    return ((OpusMemory_t*)(*OPUS_MEMORY_FUNCLIST_DEFALT_PTR))->realloc_cb(rmem, newsize);
}

void *opus_calloc_inner(uint32_t count, uint32_t size)
{
    void *ptr = opus_malloc_inner(count*size);
    if (ptr != NULL)
        memset(ptr,0,count*size);
    return ptr;
}

void opus_free_inner(void *buf)
{
    ((OpusMemory_t*)(*OPUS_MEMORY_FUNCLIST_DEFALT_PTR))->free_cb(buf);
}
