#pragma once
#include"MTF.Type.h"
#include"MTF.Memory_c.h"
namespace mtf_ns {

class MTF_MemoryCallback_c:public MTF_IMemory_c
{
public:
    MTF_MemoryCallback_c() {}
    ~MTF_MemoryCallback_c() {}
public:
    bool Init(Malloc_cb malloc, Realloc_cb realloc, Calloc_cb calloc, Free_cb free);
    bool DeInit();
public:

    virtual mtf_void* Malloc(mtf_i32 size) override;
    virtual mtf_void* Realloc(mtf_void* block, mtf_i32 size) override;
    virtual mtf_void* Calloc(mtf_i32 count, mtf_i32 size) override;
    virtual mtf_void Free(mtf_void* block) override;
private:
    Malloc_cb _malloc = 0;
    Realloc_cb _realloc = 0;
    Calloc_cb _calloc = 0;
    Free_cb _free = 0;
};

}


