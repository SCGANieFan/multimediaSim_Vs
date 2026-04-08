#include <stdio.h>
#include <stdarg.h>
#include "gasf.h"

using namespace GASF_NAME_SPACE;
#define GASF_VERSION "2.0.2"

Gasf_c::Gasf_c() {
    _magic0 = (void*)0xAA55AA55;
    _magic1 = (void*)0x55AA55AA;
}

Gasf_c::~Gasf_c() {
    _magic0 = (void*)0xDEADBEEF;
    _magic1 = (void*)0xDEADBEEF;
}

bool Gasf_c::CreateApiPost(GasfBasePort_t* bp, Gasf_c* api) noexcept {
    api->_bp = *bp;
    //api->Print("<%s>[%s](%d)" "v%s" "\n", Strrchr(__FILE__, '\\') + 1, __func__, __LINE__, GASF_VERSION);
    return true;
}

GasfRet_t Gasf_c::OpenApi(void* api) noexcept {
    if (!api) return GASF_RET_FAIL;
    return ((Gasf_c*)api)->Open(); 
}
GasfRet_t Gasf_c::SetApi(void* api, const char* choose, void* val) noexcept {
    if (!api || !choose) return GASF_RET_FAIL;
    return ((Gasf_c*)api)->Set(Str2Key(choose), val);
}
GasfRet_t Gasf_c::GetApi(void* api, const char* choose, void* val) noexcept {
    if (!api || !choose) return GASF_RET_FAIL;
    return ((Gasf_c*)api)->Get(Str2Key(choose), val);
}
GasfRet_t Gasf_c::RunApi(void* api, GasfData_c& iData, GasfData_c& oData) noexcept {
    if (!api) return GASF_RET_FAIL;
    return ((Gasf_c*)api)->Run(iData, oData);
}
GasfRet_t Gasf_c::ReceiveApi(void* api, GasfData_c& iData) noexcept {
    if (!api) return GASF_RET_FAIL;
    return ((Gasf_c*)api)->Receive(iData);
}
GasfRet_t Gasf_c::GenerateApi(void* api, GasfData_c& oData) noexcept {
    if (!api) return GASF_RET_FAIL;
    return ((Gasf_c*)api)->Generate(oData);
}
GasfRet_t Gasf_c::CloseApi(void* api) noexcept {
    if (!api) return GASF_RET_FAIL;
    return ((Gasf_c*)api)->Close();
}

bool Gasf_c::DestroyApi(void* api) noexcept {
    if (!api) return false;
    Gasf_c* gasf = (Gasf_c*)api;
    if (gasf->_magic0 != (void*)0xAA55AA55
        || gasf->_magic1 != (void*)0x55AA55AA) {
        gasf->Print("<%s>[%s](%d)" "warn, %p,(%p,%p)" "\n", Strrchr(__FILE__, GASF_PATH_SEP) + 1, __func__, __LINE__, api, gasf->_magic0, gasf->_magic1);
        return false;
    }
    GasfBasePort_t bp = gasf->_bp;
    gasf->~Gasf_c();
    if(!bp.free_cb){
        gasf->Print("<%s>[%s](%d)" "warn, no free cb" "\n", Strrchr(__FILE__, GASF_PATH_SEP) + 1, __func__, __LINE__);
        return false;
    }
    bp.free_cb(gasf);
    return true;
}


void Gasf_c::Print(const char* fmt, ...) noexcept {
    if (_bp.print_cb && fmt) {
        va_list va;
        va_start(va, fmt);
        const int32_t bufSize = static_cast<int32_t>(sizeof(_logBuf));
        u32 n = vsnprintf((char*)_logBuf, bufSize, fmt, va);
        n = (n < 0) ? 0 : ((n >= bufSize) ? (bufSize - 1) : n);
        if (n > 0) _bp.print_cb((const char*)_logBuf, n);
        va_end(va);
    }
}


