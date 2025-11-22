#include <stdint.h>
#include "opus_api_private.h"
#include "opus_api.h"

//OpusApiEnc_c
OpusApiEnc_c::OpusApiEnc_c() {
    _basePort.malloc_cb = 0;
    _basePort.realloc_cb = 0;
    _basePort.free_cb = 0;
    _basePort.print_cb = 0;

    encStackTable[0][0] = 31 * 1024;//8k1ch
    encStackTable[0][1] = 31 * 1024;//8k2ch
    encStackTable[1][0] = 31 * 1024;//12k1ch
    encStackTable[1][1] = 31 * 1024;//12k2ch
    encStackTable[2][0] = 31 * 1024;//16k1ch
    encStackTable[2][1] = 31 * 1024;//16k3ch
    encStackTable[3][0] = 31 * 1024;//24k1ch
    encStackTable[3][1] = 31 * 1024;//24k2ch
    encStackTable[4][0] = 31 * 1024;//48k1ch
    encStackTable[4][1] = 31 * 1024;//48k2ch
}
OpusApiEnc_c::~OpusApiEnc_c() {
    if (_enc) {
        _enc->Close();
        _basePort.free_cb(_enc);
    }
}

inline OpusApiRet_t OpusApiEnc_c::Create(void** pHd, OpusApi_CreateEncParam_t* param) {
    OpusApiRet_t ret = OpusApi_c::Create<OpusApiEnc_c>(pHd, &param->basePort);
    if (ret != OPUS_API_RET_SUCCESS) return ret;
    LOG(param->basePort.print_cb, "success,%p", *pHd);
    return ret;
}
inline OpusApiRet_t OpusApiEnc_c::Open() {
    LOG(_basePort.print_cb, "%p,(%u,%u,%u),(%u,%u,%u)",
        this, _fs, _ch, _choose,
        _frame0p1Ms, _bitRate, _complexity);
    uint32_t id = OPUS_ENCODE_TO_ID(_choose);
    _enc = (OpusEnc_c*)OpusCodecCreaterDoCreate(id, &_basePort);
    if (!_enc) {
        LOG(_basePort.print_cb, "create encoder fail, %d", _choose);
        return OPUS_API_RET_FAIL;
    }
    OpusApiRet_t ret = OPUS_API_RET_SUCCESS;
    ret |= _enc->Set("basePort", (void*)&_basePort);
    ret |= _enc->Set("fs", (void*)_fs);
    ret |= _enc->Set("ch", (void*)(uint32_t)_ch);
    ret |= _enc->Set("hasHead", (void*)_isWithHead);
    ret |= _enc->Set("bitrate", (void*)_bitRate);
    ret |= _enc->Set("f0p1ms", (void*)_frame0p1Ms);
    ret |= _enc->Set("vbr", (void*)_useVbr);
    ret |= _enc->Set("cpx", (void*)(uint32_t)_complexity);
    ret |= _enc->Set("encmode", (void*)_encMode);
    ret |= _enc->Set("app", (void*)application);
    ret |= _enc->Set("stackTb", (void*)encStackTable);
    if (ret != OPUS_API_RET_SUCCESS) {
        LOG(_basePort.print_cb, "set fail");
    }
    ret = _enc->Open();
    if (ret != OPUS_API_RET_SUCCESS) {
        Close();
        return ret;
    }
    LOG(_basePort.print_cb, "success, %p, %p", this, _enc);
    return OPUS_API_RET_SUCCESS;
}
OpusApiRet_t OpusApiEnc_c::Set(const char* choose, void* val) {
    LOG(_basePort.print_cb, "set %p,%s,%d", this, choose, (uint32_t)val);
    switch (Str2Key(choose)) {
    case Str2Key("bitrate"): {
        _bitRate = (uint32_t)val;
        if (_enc) return _enc->Set(choose, (void*)_bitRate);
        return OPUS_API_RET_SUCCESS;
    }
    case Str2Key("f0p1ms"): {
        _frame0p1Ms = (uint32_t)val;
        if (_enc) return _enc->Set("f0p1ms", (void*)_frame0p1Ms);
        return OPUS_API_RET_SUCCESS;
    }
    case Str2Key("vbr"): {
        _useVbr = (bool)(uint32_t)val; 
        if (_enc) return _enc->Set("vbr", (void*)_useVbr);
        return OPUS_API_RET_SUCCESS;
    }
    case Str2Key("cpx"): {
        _complexity = (uint8_t)(uint32_t)val;
        if (_enc) return _enc->Set("cpx", (void*)(uint32_t)_complexity);
        return OPUS_API_RET_SUCCESS;
    }
    case Str2Key("encmode"): {
        _encMode = (int32_t)val;
        if (_enc) return _enc->Set("encmode", (void*)_encMode);
        return OPUS_API_RET_SUCCESS;
    }
    case Str2Key("outch"): {
        _encOutChannels = (uint8_t)(uint32_t)val;
        if (_enc) return _enc->Set("outch", (void*)(uint32_t)_encOutChannels);
        return OPUS_API_RET_SUCCESS;
    }
    case Str2Key("app"): {
        application = (uint32_t)val;
        if (_enc) return _enc->Set("app", (void*)application);
        return OPUS_API_RET_SUCCESS;
    }
    case Str2Key("choose"): {
        _choose = (OpusApi_EncChoose_e)(uint32_t)val; return OPUS_API_RET_SUCCESS;
    }
    case Str2Key("fs"): {
        _fs = (uint32_t)val; return OPUS_API_RET_SUCCESS;
    }
    case Str2Key("ch"): {
        _ch = (uint32_t)val; return OPUS_API_RET_SUCCESS;
    }
    case Str2Key("hasHead"): {
        _isWithHead = (uint32_t)val; return OPUS_API_RET_SUCCESS;
    }
    case Str2Key("stk8k1ch"):
        encStackTable[0][0] = (uint32_t)val; return OPUS_API_RET_SUCCESS;
    case Str2Key("stk8k2ch"):
        encStackTable[0][1] = (uint32_t)val; return OPUS_API_RET_SUCCESS;
    case Str2Key("stk12k1ch"):
        encStackTable[1][0] = (uint32_t)val; return OPUS_API_RET_SUCCESS;
    case Str2Key("stk12k2ch"):
        encStackTable[1][1] = (uint32_t)val; return OPUS_API_RET_SUCCESS;
    case Str2Key("stk16k1ch"):
        encStackTable[2][0] = (uint32_t)val; return OPUS_API_RET_SUCCESS;
    case Str2Key("stk16k2ch"):
        encStackTable[2][1] = (uint32_t)val; return OPUS_API_RET_SUCCESS;
    case Str2Key("stk24k1ch"):
        encStackTable[3][0] = (uint32_t)val; return OPUS_API_RET_SUCCESS;
    case Str2Key("stk24k2ch"):
        encStackTable[3][1] = (uint32_t)val; return OPUS_API_RET_SUCCESS;
    case Str2Key("stk48k1ch"):
        encStackTable[4][0] = (uint32_t)val; return OPUS_API_RET_SUCCESS;
    case Str2Key("stk48k2ch"):
        encStackTable[4][1] = (uint32_t)val; return OPUS_API_RET_SUCCESS;
    }
    return OPUS_API_RET_FAIL;
}
inline OpusApiRet_t OpusApiEnc_c::Get(const char* choose, void* val) {
    LOG(_basePort.print_cb, "get %p,%s,%d", this, choose, (uint32_t)val);
    return _enc->Get(0, val);
}
inline OpusApiRet_t OpusApiEnc_c::Run(unsigned char* pcm, int* pcmByte, unsigned char* encodedFrame, int* encodedFrameByte) {
    if (!pcm
        || !pcmByte
        || *pcmByte <= 0
        || !encodedFrame
        || !encodedFrameByte
        || *encodedFrameByte <= 0) {
        LOG(_basePort.print_cb, "%p,%d,%p,%d", pcm, *pcmByte, encodedFrame, *encodedFrameByte);
        return OPUS_API_RET_FAIL;
    }
    OpusData_c iData;
    iData._buf = pcm;
    iData._len = *pcmByte;
    iData._offset = 0;
    OpusData_c oData;
    oData._buf = encodedFrame;
    oData._max = *encodedFrameByte;
    OpusApiRet_t ret = _enc->Run(iData, oData);
    *pcmByte = iData._offset;
    *encodedFrameByte = oData._len;
    return ret;
}
inline OpusApiRet_t OpusApiEnc_c::Close() {
    LOG(_basePort.print_cb, "%p,%p", this, _enc);
    if (_enc) {
        _enc->Close();
        _basePort.free_cb(_enc);
        _enc = 0;
    }
    return OPUS_API_RET_SUCCESS;
}
inline OpusApiRet_t OpusApiEnc_c::Destory(void* hd) {
    return OpusApi_c::Destory(hd);
}

EXTERNC{
//enc 
OpusApiRet_t opus_api_create_encoder(void** pHd, OpusApi_CreateEncParam_t * param){
    return OpusApiEnc_c::Create(pHd,param);
}

OpusApiRet_t opus_api_open_encoder(void* hd){
    if (!hd) return OPUS_API_RET_SUCCESS;
    return ((OpusApiEnc_c*)hd)->Open();
}

OpusApiRet_t opus_api_encoder_set(void* hd, const char* choose, void* val) {
    if (!hd) return OPUS_API_RET_SUCCESS;
    return ((OpusApiEnc_c*)hd)->Set(choose, val);
}

OpusApiRet_t opus_api_encoder_get(void* hd, const char* choose, void* val) {
    if (!hd) return OPUS_API_RET_SUCCESS;
    return ((OpusApiEnc_c*)hd)->Get(choose, val);
}

OpusApiRet_t opus_api_encoder_run(void* hd, unsigned char* pcm, int* pcmByte, unsigned char* encodedFrame, int* encodedFrameByte) {
    if (!hd) return OPUS_API_RET_SUCCESS;
    return ((OpusApiEnc_c*)hd)->Run(pcm,pcmByte,encodedFrame,encodedFrameByte);
}

OpusApiRet_t opus_api_close_encoder(void* hd) {
    if (!hd) return OPUS_API_RET_SUCCESS;
    return ((OpusApiEnc_c*)hd)->Close();
}

OpusApiRet_t opus_api_destory_encoder(void* hd) {
    return OpusApiEnc_c::Destory(hd);
}

}
