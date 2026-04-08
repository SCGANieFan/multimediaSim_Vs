
#include "opus.h"
#include "opus_private.h"
#include "opus_multistream.h"
#include "celt.h"
#include "opus_ms_dec.h"
using namespace GASF_NAME_SPACE;
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

OpusRet_t OpusMSDec_c::Open() noexcept {
    LOG_OPUS("v%s", OPUS_VERSION);
    int err = 0;
    _opus_bp.malloc_cb = _bp.malloc_cb;
    _opus_bp.free_cb = _bp.free_cb;
    _opus_bp.realloc_cb = _bp.realloc_cb;
    int stackSize = 20 * 1024;
    int streams = (_ch + 1) / 2;
    int coupled_streams = _ch / 2;
    unsigned char mapping[255];
    for (u8 n = 0; n < 255; n++) {
        mapping[n] = n;
    }
    _hd = opus_multistream_decoder_create(
        (OpusBasePort_t*)&_opus_bp,
        _fs,
        _ch,
        streams,
        coupled_streams,
        (const unsigned char*)mapping,
        &err,
        stackSize
    );
    if (err != OPUS_OK) {
        LOG_OPUS("opus api dec Cannot create decoder: (%d,%s)", err, ret2str[-err]);
        Close();
        return OPUS_RET_FAIL;
    }
    return OPUS_RET_SUCCESS;
}
OpusRet_t OpusMSDec_c::Set(uint32_t key, void* val) noexcept {
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
OpusRet_t OpusMSDec_c::Get(uint32_t key, void* val) noexcept {
    return OPUS_RET_SUCCESS;
}
OpusRet_t OpusMSDec_c::Run(OpusData_c& iData, OpusData_c& oData) noexcept {
    if (_isFirstRun) {
        _framePcmSample = opus_packet_get_samples_per_frame((const unsigned char*)iData.Data(), _fs);
        _framePcmByte = _framePcmSample * _ch * 2;
        _isFirstRun = false;
    }
    bool isPlc = iData.HasFlag(IsPlc);
    if (oData.LeftSize() < _framePcmByte) return OPUS_RET_FAIL;
    int ret = opus_multistream_decode(_hd, (unsigned char*)iData.Data(), iData.Size(), (short*)oData.LeftData(), oData.LeftSize(), isPlc);
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
OpusRet_t OpusMSDec_c::Close() noexcept {
    if (_hd) {
        opus_multistream_decoder_destroy(_hd);
    }
    return OPUS_RET_SUCCESS;
}




