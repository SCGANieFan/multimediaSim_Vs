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

OpusApiRet_t OpusDecNormal_c::Init(OpusApi_BasePort_t* basePort, int fs, int channels) {
    _basePort = basePort;
    int err = 0;
    int stackSize = 10 * 1024;
    _hd = opus_decoder_create((OpusBasePort_t*)_basePort, fs, channels, &err, stackSize);
    if (err != OPUS_OK) {
        LOG(_basePort->print_cb, "opus api dec Cannot create decoder: (%d,%s)\n", err, ret2str[-err]);
        Deinit();
        return OPUS_API_RET_FAIL;
    }
    return OPUS_API_RET_SUCCESS;
}
OpusApiRet_t OpusDecNormal_c::Deinit() {
    OpusApi_BasePort_t* basePort = _basePort;
    if (_hd) {
        opus_decoder_destroy(_hd);
    }
    return OPUS_API_RET_SUCCESS;
}
OpusApiRet_t OpusDecNormal_c::Set(OpusApi_DecSetChhoose_e choose, void* val) {
    //opus_decoder_ctl(hd, request, ##__VA_ARGS__)
    return OPUS_API_RET_SUCCESS;
}
OpusApiRet_t OpusDecNormal_c::Get(OpusApi_DecGetChhoose_e choose, void* val) {
    //opus_decoder_ctl(hd, request, ##__VA_ARGS__)
    return OPUS_API_RET_SUCCESS;
}
OpusApiRet_t OpusDecNormal_c::Run(unsigned char* in, int inByte, short* out, int* outSample, bool isPlc) {
    int ret = opus_decode(_hd, in, inByte, out, *outSample, isPlc);
    if (ret < 0) {
        LOG(_basePort->print_cb, "opus api dec fail, (%d,%s), (%p,%p,%d,%p,%d)", ret, ret2str[-ret], _hd, in, inByte, out, *outSample);
        *outSample = 0;
        return OPUS_API_RET_FAIL;
    }
    *outSample = ret;
    return OPUS_API_RET_SUCCESS;
}


EXTERNC void OpusDecoderNormalRegister() {
    OpusCodecCreaterRegister<OpusDecNormal_c>(OPUS_DECODE_TO_ID(OpusApi_DecChoose_c::OPUS_API_DEC_CHOOSE_NORMAL));
}

