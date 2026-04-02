#include "opus_api.h"
#include "opus_dec.h"
#include "opus_enc.h"
#include "opus_ms_dec.h"
#include "opus_ms_enc.h"
using namespace GASF_NAME_SPACE;

static OpusApiRet_t RetConvert(OpusRet_t ret) {
#if 0
    switch (ret)
    {
    case OPUS_RET_SUCCESS: return OPUS_API_RET_SUCCESS;
    case OPUS_RET_FAIL: return OPUS_API_RET_FAIL;
    default:return OGG_API_RET_FAIL;
    }
#else
    return ret;
#endif
}


EXTERNC{

//enc 
OpusApiRet_t opus_api_create_encoder(void** pHd, OpusApi_CreateEncParam_t* param) {
    GasfBasePort_t bp;
    bp.malloc_cb = (void* (*)(uint32_t))(param->basePort.malloc_cb);
    bp.realloc_cb = (void* (*)(void*, uint32_t))param->basePort.realloc_cb;
    bp.free_cb = (void (*)(void*))param->basePort.free_cb;
    bp.print_cb = (void (*)(const char*, uint32_t))param->basePort.print_cb;
    void* hd = OpusCodec_c::CreateApi<OpusEnc_c>(&bp);
    if (!hd) return OPUS_API_RET_FAIL;
    if (pHd) *pHd = hd;
    return OPUS_API_RET_SUCCESS;
}

OpusApiRet_t opus_api_create_ms_encoder(void** pHd, OpusApi_CreateEncParam_t* param) {
    GasfBasePort_t bp;
    bp.malloc_cb = (void* (*)(uint32_t))(param->basePort.malloc_cb);
    bp.realloc_cb = (void* (*)(void*, uint32_t))param->basePort.realloc_cb;
    bp.free_cb = (void (*)(void*))param->basePort.free_cb;
    bp.print_cb = (void (*)(const char*, uint32_t))param->basePort.print_cb;
    void* hd = OpusCodec_c::CreateApi<OpusMSEnc_c>(&bp);
    if (!hd) return OPUS_API_RET_FAIL;
    if (pHd) *pHd = hd;
    return OPUS_API_RET_SUCCESS;
}

OpusApiRet_t opus_api_open_encoder(void* hd) {
    auto ret = OpusCodec_c::OpenApi(hd);
    return RetConvert(ret);
}

OpusApiRet_t opus_api_encoder_set(void* hd, const char* choose, void* val) {
    auto ret = OpusCodec_c::SetApi(hd, choose, val);
    return RetConvert(ret);
}

OpusApiRet_t opus_api_encoder_get(void* hd, const char* choose, void* val) {
    auto ret = OpusCodec_c::GetApi(hd, choose, val);
    return RetConvert(ret);
}

OpusApiRet_t opus_api_encoder_run(void* hd, unsigned char* pcm, int* pcmByte, unsigned char* encodedFrame, int* encodedFrameByte) {
    OpusData_c iData;
    iData.Init(pcm, *pcmByte, *pcmByte);
    OpusData_c oData;
    oData.Init(encodedFrame, *encodedFrameByte);
    auto ret = OpusCodec_c::RunApi(hd, iData, oData);
    if (ret == OPUS_RET_SUCCESS) {
        *pcmByte = iData.Used();
        *encodedFrameByte = oData.Size();
    }
    return RetConvert(ret);
}

OpusApiRet_t opus_api_close_encoder(void* hd) {
    auto ret = OpusCodec_c::CloseApi(hd);
    return RetConvert(ret);
}

OpusApiRet_t opus_api_destory_encoder(void* hd) {
    auto ret = OpusCodec_c::DestoryeApi(hd);
    return RetConvert(ret);
}

OpusApiRet_t opus_api_create_ms_decoder(void** pHd, OpusApi_CreateDecParam_t* param) {
    GasfBasePort_t bp;
    bp.malloc_cb = (void* (*)(uint32_t))(param->basePort.malloc_cb);
    bp.realloc_cb = (void* (*)(void*, uint32_t))param->basePort.realloc_cb;
    bp.free_cb = (void (*)(void*))param->basePort.free_cb;
    bp.print_cb = (void (*)(const char*, uint32_t))param->basePort.print_cb;
    void* hd = OpusCodec_c::CreateApi<OpusMSDec_c>(&bp);
    if (!hd) return OPUS_API_RET_FAIL;
    *pHd = hd;
    return OPUS_API_RET_SUCCESS;
}

OpusApiRet_t opus_api_create_decoder(void** pHd, OpusApi_CreateDecParam_t * param) {
    GasfBasePort_t bp;
    bp.malloc_cb = (void* (*)(uint32_t))(param->basePort.malloc_cb);
    bp.realloc_cb = (void* (*)(void*, uint32_t))param->basePort.realloc_cb;
    bp.free_cb = (void (*)(void*))param->basePort.free_cb;
    bp.print_cb = (void (*)(const char*, uint32_t))param->basePort.print_cb;
    void* hd = OpusCodec_c::CreateApi<OpusDec_c>(&bp);
    if (!hd) return OPUS_API_RET_FAIL;
    *pHd = hd;
    return OPUS_API_RET_SUCCESS;
}

OpusApiRet_t opus_api_open_decoder(void* hd) {
    OpusRet_t ret = OpusCodec_c::OpenApi(hd);
    return RetConvert(ret);
}

OpusApiRet_t opus_api_decoder_set(void* hd, const char* choose, void* val) {
    OpusRet_t ret = OpusCodec_c::SetApi(hd,choose,val);
    return RetConvert(ret);
}

OpusApiRet_t opus_api_decoder_get(void* hd, const char* choose, void* val) {
    OpusRet_t ret = OpusCodec_c::GetApi(hd,choose,val);
    return RetConvert(ret);
}

OpusApiRet_t opus_api_decoder_run(void* hd, unsigned char* encodedOneFrame, int encodedOneFrameByte, unsigned char* decodecPcm, int* decodecPcmByte, bool isDoPlc) {
    OpusData_c iData;
    iData.Init(encodedOneFrame, encodedOneFrameByte, encodedOneFrameByte);
    if (isDoPlc) iData.SetFlag(OpusDec_c::DataFlag_e::IsPlc);
    OpusData_c oData;
    oData.Init(decodecPcm, *decodecPcmByte);
    OpusRet_t ret = OpusCodec_c::RunApi(hd, iData, oData);
    if (ret == OPUS_RET_SUCCESS) {
        *decodecPcmByte = oData.Size();
    }
    return RetConvert(ret);
}

OpusApiRet_t opus_api_close_decoder(void* hd) {
    OpusRet_t ret = OpusCodec_c::CloseApi(hd);
    return RetConvert(ret);
}

OpusApiRet_t opus_api_destory_decoder(void* hd) {
    OpusRet_t ret = OpusCodec_c::DestoryeApi(hd);
    return RetConvert(ret);
}

}




