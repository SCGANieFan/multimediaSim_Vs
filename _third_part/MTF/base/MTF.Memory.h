#pragma once
#include"MTF.Type.h"
//#include"MTF.Memory_c.h"

namespace mtf_ns {
typedef void* (*Malloc_cb)(int32_t size);
typedef void* (*Realloc_cb)(void* block, int32_t size);
typedef void* (*Calloc_cb)(int32_t count, int32_t size);
typedef void (*Free_cb)(void* block);

void* Malloc(mtf_i32 size);
void* MallocName(const char* name, mtf_i32 size);
void* Realloc(void* block, int32_t size);
void* ReallocName(const char* name, void* block, int32_t size);
void* Calloc(int32_t count, int32_t size);
void* CallocName(const char* name, int32_t count, int32_t size);
void Free(void* block);
void FreeName(const char* name, void* block);
bool MTF_MemoryRegister(const char* name, mtf_u8* buf, mtf_u32 len);
bool MTF_MemoryRegister(const char* name, Malloc_cb malloc, Realloc_cb realloc, Calloc_cb calloc, Free_cb free);


#if 1

#define MTF_MALLOC(size)            Malloc(size)
#define MTF_REALLOC(block,size)     Realloc(block,size)
#define MTF_CALLOC(count,size)      Calloc(count,size)
#define MTF_FREE(block)             Free(block)
#else
#include<stdlib.h>
#define MTF_MALLOC(size)            malloc(size)
#define MTF_REALLOC(block,size)     realloc(block,size)
#define MTF_CALLOC(count,size)      calloc(count,size)
#define MTF_FREE(block)             free(block)

#endif
}