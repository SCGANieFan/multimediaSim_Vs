#include "gaapi.h"
#include "gaapi_static.h"

using namespace gaapi_ns;
#define VERSION "1.0.0"

Gaapi_c::Gaapi_c() {
#if 0
    _bp.malloc_cb = 0;
    _bp.realloc_cb = 0;
    _bp.free_cb = 0;
    _bp.print_cb = 0;
#endif
}

Gaapi_c::~Gaapi_c() {

}

void Gaapi_c::CreateApi_0() {
    GaapiInit();
}
uint32_t Gaapi_c::CreateApi_1(GaapiBasePort_t* bp, Gaapi_c* api) {
    api->_bp = *bp;
    uint32_t id = GaapiIdManager()->Add(api);
    api->_id = id;
    if (!id) {
        api->~Gaapi_c();
        bp->free_cb(api);
    }
    return id;
}

GaapiRet_t Gaapi_c::OpenApi(uint32_t id) { return GaapiIdManager()->Id2Api(id)->Open(); }
GaapiRet_t Gaapi_c::SetApi(uint32_t id, const char* choose, void* val) { return GaapiIdManager()->Id2Api(id)->Set(choose, val); }
GaapiRet_t Gaapi_c::GetApi(uint32_t id, const char* choose, void* val) { return GaapiIdManager()->Id2Api(id)->Get(choose, val); }
GaapiRet_t Gaapi_c::RunApi(uint32_t id, GaapiData_c& iData, GaapiData_c& oData) { return GaapiIdManager()->Id2Api(id)->Run(iData, oData); }
GaapiRet_t Gaapi_c::ReceiveApi(uint32_t id, GaapiData_c& iData) { return GaapiIdManager()->Id2Api(id)->Receive(iData); }
GaapiRet_t Gaapi_c::GenerateApi(uint32_t id, GaapiData_c& oData) { return GaapiIdManager()->Id2Api(id)->Generate(oData); }
GaapiRet_t Gaapi_c::CloseApi(uint32_t id) { return GaapiIdManager()->Id2Api(id)->Close(); }

bool Gaapi_c::DestoryeApi(uint32_t id) {
    GaapiIdManager_c* im = GaapiIdManager();
    Gaapi_c* api = im->Id2Api(id);
    if (api) {
        GaapiBasePort_t bp = api->_bp;
        api->~Gaapi_c();
        bp.free_cb(api);
        im->Remove(id);
    }
    return true;
}





