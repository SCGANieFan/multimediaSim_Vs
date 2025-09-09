#pragma once
#include"MTF.Type.h"
namespace mtf_ns {

typedef void* (*Malloc_cb)(int32_t size);
typedef void* (*Realloc_cb)(void* block, int32_t size);
typedef void* (*Calloc_cb)(int32_t count, int32_t size);
typedef void (*Free_cb)(void* block);

class MTF_Memory_c
{
public:
    MTF_Memory_c() {}
    ~MTF_Memory_c() {}
public:
    void Init(Malloc_cb malloc, Realloc_cb realloc, Calloc_cb calloc, Free_cb free);
    void DeInit();
    void* Malloc(int32_t size);
    void* Realloc(void* block, int32_t size);
    void* Calloc(int32_t count, int32_t size);
    void Free(void* block);
private:
    Malloc_cb _malloc;
    Realloc_cb _realloc;
    Calloc_cb _calloc;
    Free_cb _free;
};

class MTF_MemoryItems_c {
public:
    class Item {
    public:
        Item() {}
        ~Item() {}
    public:
        const char* _name;
        mtf_u32 _id;
        MTF_Memory_c _memory;
    };
public:
    MTF_MemoryItems_c() {}
    MTF_MemoryItems_c(Item* items, mtf_i32 numMax){
        _items = items;
        _numMax = numMax;
    }
    ~MTF_MemoryItems_c() {}
public:
    void Register(const char* name, Malloc_cb malloc, Realloc_cb realloc, Calloc_cb calloc, Free_cb free);
    void Register(const char* name, mtf_u8* buf, mtf_u32 len);
    MTF_Memory_c* GetMemory(const char* name);

public:
    Item* _items = 0;
    mtf_i32 _numMax = 0;
    MTF_Memory_c* _gbl = 0;
};

template<mtf_i32 NumMax>
class MTF_OMemoryItems_c :public MTF_MemoryItems_c {
public:
    MTF_OMemoryItems_c():MTF_MemoryItems_c(_oitems, NumMax) {

    }
    ~MTF_OMemoryItems_c() {}
public:
    Item _oitems[NumMax];
};


bool MTF_MemoryRegister(const char* name, Malloc_cb malloc, Realloc_cb realloc, Calloc_cb calloc, Free_cb free);
bool MTF_MemoryRegister(const char* name, mtf_u8* buf, mtf_u32 len);
MTF_Memory_c* MTF_MemoryGetMemory(const char* name);
void* Malloc(mtf_i32 size);
void* MallocName(const char* name, mtf_i32 size);
void* Realloc(void* block, int32_t size);
void* ReallocName(const char* name, void* block, int32_t size);
void* Calloc(int32_t count, int32_t size);
void* CallocName(const char* name, int32_t count, int32_t size);
void Free(void* block);
void FreeName(const char* name, void* block);
}



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
