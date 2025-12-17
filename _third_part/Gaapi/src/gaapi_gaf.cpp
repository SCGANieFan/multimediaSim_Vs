#include "gaapi_gaf.h"
#include "gaapi_static.h"

namespace gaapi_ns {

GaapiGaf_c::GaapiGaf_c() {
}
GaapiGaf_c::~GaapiGaf_c() {
}


void GaapiGaf_c::Destory(GaapiGaf_c* gaf) {
    if (gaf) {
        void(*free_cb)(void* buf) = gaf->_bp.free_cb;
        gaf->~GaapiGaf_c();
        free_cb(gaf);
    }
}


void gaapi_gaf_register(const char* type, FuncCreate_t funcCreate) {
	GaapiInit();
	GaapiGafRegister()->Register(type, funcCreate);
}


GaapiGaf_c* gaapi_gaf_create(const char* type, GaapiBasePort_t* bp) {
	return GaapiGafRegister()->Create(type, bp);
}

};