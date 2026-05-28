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

OpusRet_t OpusDec_c::Open() noexcept {
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
OpusRet_t OpusDec_c::Set(uint32_t key, void* val) noexcept {
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
OpusRet_t OpusDec_c::Get(uint32_t key, void* val) noexcept {
    return OPUS_RET_SUCCESS;
}
OpusRet_t OpusDec_c::Run(OpusData_c& iData, OpusData_c& oData) noexcept {
#if 0
    {
        static uint32_t cnt = 0;
        uint32_t sample = opus_packet_get_samples_per_frame((const unsigned char*)iData.Data(), _fs);
        LOG_OPUS("%d,%d", ++cnt, sample);
        if (cnt == 12)
            int a = 1;
    }
#endif
#if 0
    if (_isFirstRun) {
        _framePcmSample = opus_packet_get_samples_per_frame((const unsigned char*)iData.Data(), _fs);
        _framePcmByte = _framePcmSample * _ch * 2;
        _isFirstRun = false;
    }
#else
    _framePcmSample = opus_packet_get_samples_per_frame((const unsigned char*)iData.Data(), _fs);
    _framePcmByte = _framePcmSample * _ch * 2;
#endif
    int32_t frameCntPerPacket = opus_packet_get_nb_frames((const unsigned char*)iData.Data(), iData.Size());
    if (frameCntPerPacket < 0) {
        LOG_OPUS("packet parse error, %d,%s", frameCntPerPacket, ret2str[-frameCntPerPacket]);
        return OPUS_RET_FAIL;
    }
    uint32_t framePcmSamplePerPacket = _framePcmSample * frameCntPerPacket;
    uint32_t framePcmBytePerPacket = _framePcmByte * frameCntPerPacket;
    bool isPlc = iData.HasFlag(IsPlc);
    if (oData.LeftSize() < framePcmBytePerPacket) {
        LOG_OPUS("oData is not enough, %d < %d = %d * %d", oData.LeftSize(), framePcmBytePerPacket, _framePcmByte, frameCntPerPacket);
        return OPUS_RET_FAIL;
    }

    int ret = opus_decode(_hd, (unsigned char*)iData.Data(), iData.Size(), (short*)oData.LeftData(), oData.LeftSize(), isPlc);
    if (ret < 0) {
        LOG_OPUS("opus api dec fail, (%d,%s), (%p,%p,%d,%p,%d)",
            ret, ret2str[-ret], _hd, iData.Data(), iData.Size(), oData.LeftData(), oData.LeftSize());
        LOG_OPUS("%x,%x", ((uint8_t*)iData.Data())[0], ((uint8_t*)iData.Data())[1]);
        //oData._len = 0;
        return OPUS_RET_FAIL;
    }
    if (ret != framePcmSamplePerPacket) {
        uint8_t* ptr = (uint8_t*)iData.Data();
        LOG_OPUS("head may be error, %d != %d, (%02x, %02x)", framePcmSamplePerPacket, ret, ptr[0], ptr[1]);
        return OPUS_RET_FAIL;
    }
    oData.Append(framePcmBytePerPacket);
    return OPUS_RET_SUCCESS;
}
OpusRet_t OpusDec_c::Close() noexcept {
    if (_hd) {
        opus_decoder_destroy(_hd);
    }
    return OPUS_RET_SUCCESS;
}




