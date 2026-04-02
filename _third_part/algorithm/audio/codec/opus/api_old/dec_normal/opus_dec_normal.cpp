#include "opus.h"
#include "opus_private.h"
#include "celt.h"
#include "opus_dec_normal.h"

static const char* ret2str[] = {
    "ok",
    "bad arg",
    "buffer too small",
    "internal error",
    "invalid packet",
    "unimplemented",
    "invalid state",
    "alloc fail",
};

OpusApiRet_t OpusDecNormal_c::Open() {
    if (!_basePort) {
        return OPUS_API_RET_FAIL;
    }
    int err = 0;
    int stackSize = 10 * 1024;
    _hd = opus_decoder_create((OpusBasePort_t*)_basePort, _fs, _ch, &err, stackSize);
    if (err != OPUS_OK) {
        LOG(_basePort->print_cb, "opus api dec Cannot create decoder: (%d,%s)\n", err, ret2str[-err]);
        Close();
        return OPUS_API_RET_FAIL;
    }
    return OPUS_API_RET_SUCCESS;
}
OpusApiRet_t OpusDecNormal_c::Close() {
    OpusApi_BasePort_t* basePort = _basePort;
    if (_hd) {
        opus_decoder_destroy(_hd);
    }
    return OPUS_API_RET_SUCCESS;
}
OpusApiRet_t OpusDecNormal_c::Set(const char* choose, void* val) {
    uint64_t key = Str2Key(choose);
    switch (key)
    {
    case Str2Key("basePort"):
        _basePort = (OpusApi_BasePort_t*)val; return OPUS_API_RET_SUCCESS;
    case Str2Key("fs"):
        _fs = (uint32_t)val; return OPUS_API_RET_SUCCESS;
    case Str2Key("ch"):
        _ch = (uint8_t)(uint32_t)val; return OPUS_API_RET_SUCCESS;
    default:
        break;
    }
    return OPUS_API_RET_SUCCESS;
}
OpusApiRet_t OpusDecNormal_c::Get(const char* choose, void* val) {
    //opus_decoder_ctl(hd, request, ##__VA_ARGS__)
    return OPUS_API_RET_SUCCESS;
}
OpusApiRet_t OpusDecNormal_c::Run(OpusData_c & iData, OpusData_c & oData) {
    if (_isFirstRun) {
        _framePcmSample = opus_packet_get_samples_per_frame((const unsigned char*) iData._buf, _fs);
        _framePcmByte = _framePcmSample * _ch * 2;
        _isFirstRun = false;
    }
    bool isPlc = iData._flag;
    if (oData._max < _framePcmByte) return OPUS_API_RET_FAIL;
    int ret = opus_decode(_hd, (unsigned char*)iData._buf, iData._len, (short*)oData._buf, oData._max, isPlc);
    if (ret < 0) {
        LOG(_basePort->print_cb, "opus api dec fail, (%d,%s), (%p,%p,%d,%p,%d)", 
            ret, ret2str[-ret], _hd, iData._buf, iData._len, oData._buf, oData._max);
        //oData._len = 0;
        return OPUS_API_RET_FAIL;
    }
    if (ret != _framePcmSample) {
        LOG(_basePort->print_cb, "%d != %d", _framePcmSample, ret);
        return OPUS_API_RET_FAIL;
    }
    oData._len += _framePcmByte;
    return OPUS_API_RET_SUCCESS;
}


EXTERNC void OpusDecoderNormalRegister() {
    OpusCodecCreaterRegister<OpusDecNormal_c>(OPUS_DECODE_TO_ID(OpusApi_DecChoose_e::OPUS_API_DEC_CHOOSE_NORMAL));
}

