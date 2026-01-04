#pragma once
#include "gaapi_base.h"

namespace ogg_gaapi_ns {

class GaapiMemory_c:public GaapiBase_c
{
public:
    GaapiMemory_c(void** allocList, uint32_t allocListNum) {
        _allocList = allocList;
        _allocListNum = allocListNum;
    }
    ~GaapiMemory_c() {}
public:
    void Init(GaapiBasePort_t* base_porting);
    void DeInit();
    void* Malloc(int32_t size);
    void* Realloc(void* bufOri, int32_t size);
    void Free(void* ptr);
    void FreeAll();
    template<class T>
    void* New() {
        void* ptr = Malloc(sizeof(T));
        if (!ptr)
            return 0;
        return new(ptr) T();
    }
    template<class T>
    void Delete(void* ptr) {
        ((T*)ptr)->~T();
        Free(ptr);
    }
private:
    GaapiBasePort_t* _bp = 0;
    uint32_t _allocListNum = 0;
    void** _allocList = 0;
};

template <uint32_t Num>
class GaapiMemoryInstance_c :public GaapiMemory_c
{
public:
    GaapiMemoryInstance_c() :GaapiMemory_c(&_allocListInstance[0], Num) {}
    ~GaapiMemoryInstance_c() {}
private:
    void* _allocListInstance[Num];
};




};

