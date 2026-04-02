#include "opus_codec_com.h"
using namespace GASF_NAME_SPACE;

GasfRet_t OpusCodec_c::SetApi(void* api, const char* choose, void* val) {
    LOG_OPUS_WITH_HANDLE(api, "%s,%d", choose ? choose : "", (int32_t)val);
    return Gasf_c::SetApi(api, choose, val);
}


