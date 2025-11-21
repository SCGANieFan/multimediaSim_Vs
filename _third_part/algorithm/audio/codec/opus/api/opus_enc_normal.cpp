#include "opus.h"
#include "opus_private.h"
#include "celt.h"
#include "opus_enc_normal.h"

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

void OpusEncNormal_c::int_to_char(opus_uint32 i, unsigned char ch[4]) {
    ch[0] = i >> 24;
    ch[1] = (i >> 16) & 0xFF;
    ch[2] = (i >> 8) & 0xFF;
    ch[3] = i & 0xFF;
}

OpusApiRet_t OpusEncNormal_c::toOpusFrameDuration(int in, int* out) {
    int out0;
    switch (in) {
    case 0u: out0 = OPUS_FRAMESIZE_ARG; break;
    case 25u:out0 = OPUS_FRAMESIZE_2_5_MS; break;
    case 50u:out0 = OPUS_FRAMESIZE_5_MS; break;
    case 100u:out0 = OPUS_FRAMESIZE_10_MS; break;
    case 200u:out0 = OPUS_FRAMESIZE_20_MS; break;
    case 400u:out0 = OPUS_FRAMESIZE_40_MS; break;
    case 600u:out0 = OPUS_FRAMESIZE_60_MS; break;
    case 800u:out0 = OPUS_FRAMESIZE_80_MS; break;
    case 1000u:out0 = OPUS_FRAMESIZE_100_MS; break;
    case 1200u:out0 = OPUS_FRAMESIZE_120_MS; break;
    default:
        return OPUS_API_RET_FAIL;
    }
    *out = out0;
    return OPUS_API_RET_SUCCESS;
}
OpusApiRet_t OpusEncNormal_c::opus_encoder_run_no_head(OpusEncNormal_c* enc, short* in, int inSample, unsigned char* out, int* outByte) {
    int ret = opus_encode(enc->_hd, in, inSample, out, *outByte);
    if (ret < 0) {
        LOG(enc->_basePort->print_cb, "enc fail, (%d,%s), (%p,%p,%d,%p,%d)", ret, ret2str[-ret], enc->_hd, in, inSample, out, *outByte);
        *outByte = 0;
        return OPUS_API_RET_FAIL;
    }
    *outByte = ret;
    return OPUS_API_RET_SUCCESS;
}

OpusApiRet_t OpusEncNormal_c::opus_encoder_run_with_head(OpusEncNormal_c* enc, short* in, int inSample, unsigned char* out, int* outByte) {
    const int headByte = 8;
    if (*outByte < headByte) {
        LOG(enc->_basePort->print_cb, "outByte not enough (%d,%d)", *outByte, headByte);
        *outByte = 0;
        return OPUS_API_RET_FAIL;
    }

    int ret = opus_encode(enc->_hd, in, inSample, out + headByte, *outByte - headByte);
    if (ret < 0) {
        LOG(enc->_basePort->print_cb, "enc fail, (%d,%s) (%p,%p,%d,%p,%d)", ret, ret2str[-ret], enc->_hd, in, inSample, out, *outByte);
        return OPUS_API_RET_FAIL;
    }
    int_to_char(ret, out);
    opus_uint32 enc_final_range = 0;
    opus_encoder_ctl(enc->_hd, OPUS_GET_FINAL_RANGE(&enc_final_range));
    int_to_char(enc_final_range, out + 4);
    *outByte = ret + 8;
    return OPUS_API_RET_SUCCESS;
}

OpusApiRet_t OpusEncNormal_c::Init(OpusApi_BasePort_t* basePort, int fs, int channels, bool isWithHead){
    if (!basePort)
        return OPUS_API_RET_FAIL;
    int err = 0;
    int application = OPUS_APPLICATION_VOIP;
#if 0
    int application = OPUS_APPLICATION_VOIP;
    int application = OPUS_APPLICATION_AUDIO;
    int application = OPUS_APPLICATION_RESTRICTED_LOWDELAY;
#endif
    _basePort = basePort;
    int stackSize = 31 * 1024;
    _hd = opus_encoder_create((OpusBasePort_t*)_basePort, fs, channels, application, &err, stackSize);
    if (!_hd) {
        LOG(_basePort->print_cb, "opus api enc create fail, (%d,%s)", err, ret2str[-err]);
        Deinit();
        return OPUS_API_RET_FAIL;
    }
    opus_encoder_ctl(_hd, OPUS_SET_EXPERT_FRAME_DURATION(OPUS_FRAMESIZE_ARG));
    opus_encoder_ctl(_hd, OPUS_SET_FORCE_MODE(OPUS_AUTO));
    opus_encoder_ctl(_hd, OPUS_SET_BANDWIDTH(OPUS_AUTO));
    opus_encoder_ctl(_hd, OPUS_SET_VBR(0));
    opus_encoder_ctl(_hd, OPUS_SET_BITRATE((int)12000));
    opus_encoder_ctl(_hd, OPUS_SET_COMPLEXITY((int)0));
#if 0
    //opus_encoder_ctl(_hd, OPUS_SET_BANDWIDTH(OPUS_BANDWIDTH_NARROWBAND));
    //opus_encoder_ctl(_hd, OPUS_SET_BANDWIDTH(OPUS_BANDWIDTH_MEDIUMBAND));
    //opus_encoder_ctl(_hd, OPUS_SET_BANDWIDTH(OPUS_BANDWIDTH_WIDEBAND));
    //opus_encoder_ctl(_hd, OPUS_SET_BANDWIDTH(OPUS_BANDWIDTH_FULLBAND));
#endif
    if (isWithHead) {
        _run_cb = opus_encoder_run_with_head;
    }
    else {
        _run_cb = opus_encoder_run_no_head;
    }
    return OPUS_API_RET_SUCCESS;
}
OpusApiRet_t OpusEncNormal_c::Deinit(){
    if (_hd) {
        opus_encoder_destroy(_hd);
    }
    return OPUS_API_RET_SUCCESS;
}
OpusApiRet_t OpusEncNormal_c::Set(OpusApi_EncSetChhoose_e choose, void* val){
    switch (choose) {
    case OpusApi_EncSetChhoose_e::OPUS_API_ENC_SET_BIT_RATE:{
        opus_encoder_ctl(_hd, OPUS_SET_BITRATE((int)val));
        break;
    }
    case OpusApi_EncSetChhoose_e::OPUS_API_ENC_SET_FRAME_DURATION_0P1MS:{
        int fd;
        if (toOpusFrameDuration((int)val, &fd) != OPUS_API_RET_SUCCESS)
            return OPUS_API_RET_FAIL;
        opus_encoder_ctl(_hd, OPUS_SET_EXPERT_FRAME_DURATION(fd));
        break;
    }
    case OpusApi_EncSetChhoose_e::OPUS_API_ENC_SET_USE_VBR:{
        opus_encoder_ctl(_hd, OPUS_SET_VBR(!!(int)val));
        break;
    }
    case OpusApi_EncSetChhoose_e::OPUS_API_ENC_SET_COMPLEXITY:{
        opus_encoder_ctl(_hd, OPUS_SET_COMPLEXITY((int)val));
        break;
    }
    case OpusApi_EncSetChhoose_e::OPUS_API_ENC_SET_MODE:{
        int v = (int)val;
        if(v!= OpusApi_EncSetMode_e::OPUS_API_ENC_SET_MODE_AUTO
            && v != OpusApi_EncSetMode_e::OPUS_API_ENC_SET_MODE_SILK_ONLY
            && v != OpusApi_EncSetMode_e::OPUS_API_ENC_SET_MODE_HYBRID
            && v != OpusApi_EncSetMode_e::OPUS_API_ENC_SET_MODE_CELT_ONLY)
            return OPUS_API_RET_FAIL;
        opus_encoder_ctl(_hd, OPUS_SET_FORCE_MODE(v));
        break;
    }
    case OpusApi_EncSetChhoose_e::OPUS_API_ENC_SET_OUT_CH: {
        int v = (int)val;
        opus_encoder_ctl(_hd, OPUS_SET_FORCE_CHANNELS(v));
        break;
    }
    case OpusApi_EncSetChhoose_e::OPUS_API_ENC_SET_APPLICATION: {
        int v = (int)val;
        opus_encoder_ctl(_hd, OPUS_SET_APPLICATION(v));
        break;
    }
    default:
        break;
    }
    return OPUS_API_RET_SUCCESS;
}
OpusApiRet_t OpusEncNormal_c::Get(OpusApi_EncGetChhoose_e choose, void* val) {
    return OPUS_API_RET_SUCCESS;
}
OpusApiRet_t OpusEncNormal_c::Run(short* in, int inSample, unsigned char* out, int* outByte) {
    return _run_cb(this, in, inSample, out, outByte);
}

EXTERNC void OpusEncoderNormalRegister() {
    OpusCodecCreaterRegister<OpusEncNormal_c>(OPUS_ENCODE_TO_ID(OpusApi_EncChoose_c::OPUS_API_ENC_CHOOSE_NORMAL));
}

