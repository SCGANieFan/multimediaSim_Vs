#pragma once
#include "gaapi_base.h"
#include "gaapi_data.h"

namespace ogg_gaapi_ns {
    class GaapiGaf_c :public GaapiBase_c
    {
    public:
        GaapiGaf_c();
        virtual ~GaapiGaf_c();
    public:
        template<class T>
        static GaapiGaf_c* Create(GaapiBasePort_t* bp) { 
            if (!bp || !bp->malloc_cb || !bp->free_cb)return 0;
            GaapiGaf_c* gaf = (GaapiGaf_c*)bp->malloc_cb(sizeof(T));
            if (!gaf) { return 0; }
            new(gaf) T();
            gaf->_bp = *bp;
            return gaf;
        }
        static void Destory(GaapiGaf_c* gaf);
    public:
        virtual GaapiRet_t Open() { return GAAPI_RET_FAIL; }
        virtual GaapiRet_t Set(uint32_t key, void* val) { return GAAPI_RET_FAIL; }
        virtual GaapiRet_t Get(uint32_t key, void* val) { return GAAPI_RET_FAIL; }
        virtual GaapiRet_t Run(GaapiData_c& iData, GaapiData_c& oData) { return GAAPI_RET_FAIL; }
        virtual GaapiRet_t Receive(GaapiData_c& iData) { return GAAPI_RET_FAIL; }
        virtual GaapiRet_t Generate(GaapiData_c& oData) { return GAAPI_RET_FAIL; }
        virtual GaapiRet_t Close() { return GAAPI_RET_FAIL; }
    public:
        GaapiBasePort_t _bp;
    };

    using FuncCreate_t = GaapiGaf_c* (*)(GaapiBasePort_t* bp);
    void gaapi_gaf_register(const char* type, FuncCreate_t funcCreate);
    template<class T> void gaapi_gaf_register(const char* type) { gaapi_gaf_register(type, GaapiGaf_c::Create<T>); }
    GaapiGaf_c* gaapi_gaf_create(const char* type, GaapiBasePort_t* bp);
};

