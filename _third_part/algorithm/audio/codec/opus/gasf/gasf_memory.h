#if 0
#pragma once
#include "gasf_base.h"

namespace GASF_NAME_SPACE {

class GasfMemory_c:public GasfBase_c
{
public:
    GasfMemory_c(void** allocList, uint32_t allocListNum) {
        _allocList = allocList;
        _allocListNum = allocListNum;
    }
    ~GasfMemory_c() {}
public:
    void Init(GasfBasePort_t* base_porting);
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
    GasfBasePort_t* _bp = 0;
    uint32_t _allocListNum = 0;
    void** _allocList = 0;
};

};

#endif