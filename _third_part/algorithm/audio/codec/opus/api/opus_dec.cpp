
#include "opus.h"
#include "opus_private.h"
#include "celt.h"
#include "opus_dec.h"

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

OpusRet_t OpusDec_c::Open(){
    LOG_OPUS("v%s", OPUS_VERSION);
    int err = 0;
    int stackSize = 10 * 1024;
    _opus_bp.malloc_cb = _bp.malloc_cb;
    _opus_bp.free_cb = _bp.free_cb;
    _opus_bp.realloc_cb = _bp.realloc_cb;
    _hd = opus_decoder_create((OpusBasePort_t*)&_opus_bp, _fs, _ch, &err, stackSize);
    if (err != OPUS_OK) {
        LOG_OPUS("opus api dec Cannot create decoder: (%d,%s)", err, ret2str[-err]);
        Close();
        return OPUS_RET_FAIL;
    }
    return OPUS_RET_SUCCESS;
}
OpusRet_t OpusDec_c::Set(uint32_t key, void* val){
    switch (key)
    {
    case Str2Key("fs"):
        _fs = (uint32_t)val; return OPUS_RET_SUCCESS;
    case Str2Key("ch"):
        _ch = (uint8_t)(uint32_t)val; return OPUS_RET_SUCCESS;
    default:
        break;
    }
    return OPUS_RET_SUCCESS;
}
OpusRet_t OpusDec_c::Get(uint32_t key, void* val){
    return OPUS_RET_SUCCESS;
}
OpusRet_t OpusDec_c::Run(OpusData_c& iData, OpusData_c& oData){
    if (_isFirstRun) {
        _framePcmSample = opus_packet_get_samples_per_frame((const unsigned char*)iData.Data(), _fs);
        _framePcmByte = _framePcmSample * _ch * 2;
        _isFirstRun = false;
    }
    bool isPlc = iData.HasFlag(IsPlc);
    if (oData.LeftSize() < _framePcmByte) return OPUS_RET_FAIL;
    int ret = opus_decode(_hd, (unsigned char*)iData.Data(), iData.Size(), (short*)oData.LeftData(), oData.LeftSize(), isPlc);
    if (ret < 0) {
        LOG_OPUS("opus api dec fail, (%d,%s), (%p,%p,%d,%p,%d)",
            ret, ret2str[-ret], _hd, iData.Data(), iData.Size(), oData.LeftData(), oData.LeftSize());
        //oData._len = 0;
        return OPUS_RET_FAIL;
    }
    if (ret != _framePcmSample) {
        LOG_OPUS("%d != %d", _framePcmSample, ret);
        return OPUS_RET_FAIL;
    }
    oData.Append(_framePcmByte);
    return OPUS_RET_SUCCESS;
}
OpusRet_t OpusDec_c::Close() {
    if (_hd) {
        opus_decoder_destroy(_hd);
    }
    return OPUS_RET_SUCCESS;
}




