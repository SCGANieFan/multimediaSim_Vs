#include <stdint.h>
#include "opus_api_private.h"
#include "opus_api.h"

OpusApiDec_c::OpusApiDec_c() {
    _basePort.malloc_cb = 0;
    _basePort.realloc_cb = 0;
    _basePort.free_cb = 0;
    _basePort.print_cb = 0;
    _dec = 0;
}
OpusApiDec_c::~OpusApiDec_c() {
    if (_dec) {
        _dec->Close();
        _basePort.free_cb(_dec);
    }
}

//OpusApiDec_c
inline OpusApiRet_t OpusApiDec_c::Create(void** pHd, OpusApi_CreateDecParam_t* param) {
    OpusApiRet_t ret = OpusApi_c::Create<OpusApiDec_c>(pHd, &param->basePort);
    if (ret != OPUS_API_RET_SUCCESS) return ret;
    LOG(param->basePort.print_cb, "success,%p", *pHd);
    return ret;
}
inline OpusApiRet_t OpusApiDec_c::Open() {
    LOG(_basePort.print_cb, "%p,(%u,%u,%u)",
        this, _fs, _ch, _choose);
    if (_choose > OpusApi_DecChoose_e::OPUS_API_DEC_CHOOSE_MAX)
        return OPUS_API_RET_FAIL;
    uint32_t id = OPUS_DECODE_TO_ID(_choose);
    _dec = (OpusDec_c*)OpusCodecCreaterDoCreate(id, &_basePort);
    if (!_dec) {
        LOG(_basePort.print_cb, "choose error, %d", _choose);
        return OPUS_API_RET_FAIL;
    }
    _dec->Set("basePort", (void*)&_basePort);
    _dec->Set("fs", (void*)_fs);
    _dec->Set("ch", (void*)(uint32_t)_ch);
    OpusApiRet_t ret = _dec->Open();
    if (ret != OPUS_API_RET_SUCCESS) {
        Close();
        return ret;
    }
    LOG(_basePort.print_cb, "success, %p, %p", this, _dec);
    return OPUS_API_RET_SUCCESS;
}

inline OpusApiRet_t OpusApiDec_c::Set(const char* choose, void* val) {
    LOG(_basePort.print_cb, "set (%p, %s, %d)", this, choose, (uint32_t)val);
    uint64_t key = Str2Key(choose);
    switch (key)
    {
    case Str2Key("choose"):_choose = (uint8_t)(uint32_t)val; return OPUS_API_RET_SUCCESS;
    case Str2Key("fs"):_fs = (uint32_t)val; return OPUS_API_RET_SUCCESS;
    case Str2Key("ch"):_ch = (uint8_t)(uint32_t)val; return OPUS_API_RET_SUCCESS;
    default:
        break;
    }
    return _dec->Set(choose, val);
}

inline OpusApiRet_t OpusApiDec_c::Get(const char* choose, void* val) {
    LOG(_basePort.print_cb, "get (%p, %s, %d)", this, choose, (uint32_t)val);
    return _dec->Get(choose, val);
}

inline OpusApiRet_t OpusApiDec_c::Run(unsigned char* encodedOneFrame, int encodedOneFrameByte, unsigned char* decodecPcm, int*decodecPcmByte, bool isPlc) {
#if 1
    if (!isPlc) {
        if (!encodedOneFrame
            || encodedOneFrameByte < 0
            || !decodecPcm
            || !decodecPcmByte
            || *decodecPcmByte <= 0) {
            LOG(_basePort.print_cb, "fail, (%p,%d,%p,%d)",
                encodedOneFrame, encodedOneFrameByte, 
                decodecPcm, *decodecPcmByte);
        }
    }
#endif
    OpusData_c iData;
    iData._buf = encodedOneFrame;
    iData._len = encodedOneFrameByte;
    iData._flag = isPlc;
    OpusData_c oData;
    oData._buf = decodecPcm;
    oData._max = *decodecPcmByte;
    OpusApiRet_t ret = _dec->Run(iData, oData);
    *decodecPcmByte = oData._len;
    return ret;
}
inline OpusApiRet_t OpusApiDec_c::Close() {
    LOG(_basePort.print_cb, "%p,%p", this, _dec);
    if (_dec) {
        _dec->Close();
        _basePort.free_cb(_dec);
        _dec = 0;
    }
    return OPUS_API_RET_SUCCESS;
}
inline OpusApiRet_t OpusApiDec_c::Destory(void* hd) {
    return OpusApi_c::Destory(hd);
}

EXTERNC{
OpusApiRet_t opus_api_create_decoder(void** pHd, OpusApi_CreateDecParam_t* param) {
    return OpusApiDec_c::Create(pHd,param);
}

OpusApiRet_t opus_api_open_decoder(void* hd) {
    if (!hd) return OPUS_API_RET_FAIL;
    return ((OpusApiDec_c*)hd)->Open();
}

OpusApiRet_t opus_api_decoder_set(void* hd, const char* choose, void* val) {
    if(!hd) return OPUS_API_RET_FAIL;
    return ((OpusApiDec_c*)hd)->Set(choose,val);
}

OpusApiRet_t opus_api_decoder_get(void* hd, const char* choose, void* val) {
    if (!hd) return OPUS_API_RET_FAIL;
    return ((OpusApiDec_c*)hd)->Get(choose,val);
}

OpusApiRet_t opus_api_decoder_run(void* hd, unsigned char* encodedOneFrame, int encodedOneFrameByte, unsigned char* decodecPcm, int* decodecPcmByte, bool isDoPlc) {
    if (!hd) return OPUS_API_RET_FAIL;
    return ((OpusApiDec_c*)hd)->Run(encodedOneFrame, encodedOneFrameByte, decodecPcm, decodecPcmByte, isDoPlc);
}

OpusApiRet_t opus_api_close_decoder(void* hd) {
    if (!hd) return OPUS_API_RET_FAIL;
    return ((OpusApiDec_c*)hd)->Close();
}

OpusApiRet_t opus_api_destory_decoder(void* hd) {
    return OpusApiDec_c::Destory(hd);
}

}
