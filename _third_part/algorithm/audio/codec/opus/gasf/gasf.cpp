#include <stdio.h>
#include <stdarg.h>
#include "gasf.h"

using namespace GASF_NAME_SPACE;
#define GASF_VERSION "2.0.2"

Gasf_c::Gasf_c() {
}

Gasf_c::~Gasf_c() {
}

bool Gasf_c::CreateApiPost(GasfBasePort_t* bp, Gasf_c* api) {
    api->_magic0 = (void*)api;
    api->_bp = *bp;
    api->_magic1 = (void*)api;
    //api->Print("<%s>[%s](%d)" "v%s" "\n", Strrchr(__FILE__, '\\') + 1, __func__, __LINE__, GASF_VERSION);
    return true;
}

GasfRet_t Gasf_c::OpenApi(void* api) { 
    if (!api) return GASF_RET_FAIL;
    return ((Gasf_c*)api)->Open(); 
}
GasfRet_t Gasf_c::SetApi(void* api, const char* choose, void* val) { 
    if (!api || !choose) return GASF_RET_FAIL;
    return ((Gasf_c*)api)->Set(Str2Key(choose), val);
}
GasfRet_t Gasf_c::GetApi(void* api, const char* choose, void* val) { 
    if (!api || !choose) return GASF_RET_FAIL;
    return ((Gasf_c*)api)->Get(Str2Key(choose), val);
}
GasfRet_t Gasf_c::RunApi(void* api, GasfData_c& iData, GasfData_c& oData) { 
    if (!api) return GASF_RET_FAIL;
    return ((Gasf_c*)api)->Run(iData, oData);
}
GasfRet_t Gasf_c::ReceiveApi(void* api, GasfData_c& iData) { 
    if (!api) return GASF_RET_FAIL;
    return ((Gasf_c*)api)->Receive(iData);
}
GasfRet_t Gasf_c::GenerateApi(void* api, GasfData_c& oData) { 
    if (!api) return GASF_RET_FAIL;
    return ((Gasf_c*)api)->Generate(oData);
}
GasfRet_t Gasf_c::CloseApi(void* api) { 
    if (!api) return GASF_RET_FAIL;
    return ((Gasf_c*)api)->Close();
}

bool Gasf_c::DestoryeApi(void* api) {
    if (!api) return false;
    Gasf_c* gasf = (Gasf_c*)api;
    if (gasf->_magic0 != api
        || gasf->_magic1 != api) {
        gasf->Print("<%s>[%s](%d)" "warn, %p,(%p,%p)" "\n", Strrchr(__FILE__, '\\') + 1, __func__, __LINE__, api, gasf->_magic0, gasf->_magic1);
    }
    GasfBasePort_t bp = gasf->_bp;
    gasf->~Gasf_c();
    bp.free_cb(gasf);
    return true;
}


void Gasf_c::Print(const char* fmt, ...) {
    if (_bp.print_cb) {
        va_list va;
        va_start(va, fmt);
#if 1
        u32 n = vsnprintf((char*)_logBuf, sizeof(_logBuf), fmt, va);
        _bp.print_cb((const char*)_logBuf, n);
#else
        vprintf(fmt, va);
#endif
        va_end(va);
    }
}


