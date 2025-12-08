#pragma once
#include "gaapi_base.h"
#include "gaapi_data.h"

namespace gaapi_ns {

class Gaapi_c :public GaapiBase_c
{
public:
    Gaapi_c();
    virtual ~Gaapi_c();
public:
    template<class T>
    static uint32_t CreateApi(GaapiBasePort_t* bp) {
        CreateApi_0();
        if (!bp || !bp->malloc_cb || !bp->free_cb)return GAAPI_RET_FAIL;
        Gaapi_c* api = (Gaapi_c*)bp->malloc_cb(sizeof(T));
        if (!api) { return GAAPI_RET_FAIL; }
        new(api) T();
        return CreateApi_1(bp, api);
    }
    static void CreateApi_0();
    static uint32_t CreateApi_1(GaapiBasePort_t* bp, Gaapi_c* api);
    static GaapiRet_t OpenApi(uint32_t id);
    static GaapiRet_t SetApi(uint32_t id, const char* choose, void* val);
    static GaapiRet_t GetApi(uint32_t id, const char* choose, void* val);
    static GaapiRet_t RunApi(uint32_t id, GaapiData_c& iData, GaapiData_c& oData);
    static GaapiRet_t ReceiveApi(uint32_t id, GaapiData_c& iData);
    static GaapiRet_t GenerateApi(uint32_t id, GaapiData_c& oData);
    static GaapiRet_t CloseApi(uint32_t id);
    static bool DestoryeApi(uint32_t id);
public:
    virtual GaapiRet_t Open() { return GAAPI_RET_FAIL; }
    virtual GaapiRet_t Set(const char* choose, void* val) { return GAAPI_RET_FAIL; }
    virtual GaapiRet_t Get(const char* choose, void* val) { return GAAPI_RET_FAIL; }
    virtual GaapiRet_t Run(GaapiData_c& iData, GaapiData_c& oData) { return GAAPI_RET_FAIL; }
    virtual GaapiRet_t Receive(GaapiData_c& iData) { return GAAPI_RET_FAIL; }
    virtual GaapiRet_t Generate(GaapiData_c& oData) { return GAAPI_RET_FAIL; }
    virtual GaapiRet_t Close() { return GAAPI_RET_FAIL; }
public:
    GaapiBasePort_t _bp;
    uint32_t _id = 0;
};


};

