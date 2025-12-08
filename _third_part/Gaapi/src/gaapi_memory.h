#pragma once
#include "gaapi_base.h"

namespace gaapi_ns {
    
class GaapiMemory_c:public GaapiBase_c
{
public:
    GaapiMemory_c() {}
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
    void* _allocList[100];
};





};

