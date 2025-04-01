#pragma once
#include"MAF.Type.h"
class MAF_Memory
{
    friend class MAF_Algorithm;
public:
    typedef maf_void* (*Malloc_t)(maf_int32 size);
    typedef maf_void* (*Realloc_t)(maf_void* block, maf_int32 size);
    typedef maf_void* (*Calloc_t)(maf_int32 count, maf_int32 size);
    typedef maf_void (*Free_t)(maf_void* block);
public:
	MAF_Memory();
	~MAF_Memory();

public:
    //get
    maf_void* GetMalloc();
    maf_void* GetRealloc();
    maf_void* GetCalloc();
    maf_void* GetFree();

    //
    maf_void Init(maf_void* malloc, maf_void* realloc, maf_void* calloc, maf_void* free);

    maf_void* Malloc(maf_int32 size);
    maf_void* Realloc(maf_void* block, maf_int32 size);
    maf_void* Calloc(maf_int32 count, maf_int32 size);
    maf_void Free(maf_void* block);

private:
    Malloc_t _malloc;
    Realloc_t _realloc;
    Calloc_t _calloc;
    Free_t _free;
};

