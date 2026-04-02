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
OpusApiRet_t OpusEncNormal_c::opus_encoder_run_no_head(OpusEncNormal_c* enc, OpusData_c& iData, OpusData_c& oData) {
    if (iData._len < enc->_pcmFrameByte) {
        LOG(enc->_basePort->print_cb, "%d < %d", iData._len, enc->_pcmFrameByte);
        return OPUS_API_RET_FAIL;
    }
    int ret = opus_encode(enc->_hd, (short*)iData._buf, enc->_pcmFrameSample, (unsigned char*)oData._buf, oData._max);
    if (ret < 0) {
        LOG(enc->_basePort->print_cb, "enc fail, (%d,%s), (%p,%p,%d,%p,%d)", 
            ret, ret2str[-ret], enc->_hd, iData._buf, iData._len, oData._buf, oData._max);
        oData._len = 0;
        return OPUS_API_RET_FAIL;
    }
    iData._len -= enc->_pcmFrameByte;
    iData._offset += enc->_pcmFrameByte;
    oData._len = ret;
    return OPUS_API_RET_SUCCESS;
}

OpusApiRet_t OpusEncNormal_c::opus_encoder_run_with_head(OpusEncNormal_c* enc, OpusData_c& iData, OpusData_c& oData) {
    const int headByte = 8;
    if (iData._len < enc->_pcmFrameByte) {
        LOG(enc->_basePort->print_cb, "%d < %d", iData._len, enc->_pcmFrameByte);
        return OPUS_API_RET_FAIL;
    }
    if (oData._max < headByte) {
        LOG(enc->_basePort->print_cb, "outByte not enough (%d,%d)", oData._max, headByte);
        oData._max = 0;
        return OPUS_API_RET_FAIL;
    }
    int ret = opus_encode(enc->_hd, (short*)iData._buf, enc->_pcmFrameSample, (uint8_t*)oData._buf + headByte, oData._max - headByte);
    if (ret < 0) {
        LOG(enc->_basePort->print_cb, "enc fail, (%d,%s) (%p,%p,%d,%p,%d)", 
            ret, ret2str[-ret], enc->_hd, iData._buf, iData._len, oData._buf, oData._max);
        return OPUS_API_RET_FAIL;
    }
    iData._len -= enc->_pcmFrameByte;
    iData._offset += enc->_pcmFrameByte;
    int_to_char(ret, (uint8_t*)oData._buf);
    opus_uint32 enc_final_range = 0;
    opus_encoder_ctl(enc->_hd, OPUS_GET_FINAL_RANGE(&enc_final_range));
    int_to_char(enc_final_range, (uint8_t*)oData._buf + 4);
    oData._len = ret + 8;
    return OPUS_API_RET_SUCCESS;
}

OpusApiRet_t OpusEncNormal_c::Open(){
    if (!_basePort)
        return OPUS_API_RET_FAIL;
    int err = 0;
    uint8_t idxFs = 0;
    uint8_t idxCh = 0;
    if (_fs == 8000) { idxFs = 0; }
    else if (_fs == 12000) { idxFs = 1; }
    else if (_fs == 16000) { idxFs = 2; }
    else if (_fs == 24000) { idxFs = 3; }
    else if (_fs == 48000) { idxFs = 4; }
    else { return OPUS_API_RET_FAIL; }
  
    if (_ch == 1) { idxCh = 0; }
    else if (_ch == 2) { idxCh = 1; }
    else { return OPUS_API_RET_FAIL; }

    int stackSize = _stackTable[idxFs][idxCh];
    _hd = opus_encoder_create((OpusBasePort_t*)_basePort, _fs, _ch, _application, &err, stackSize);
    if (!_hd) {
        LOG(_basePort->print_cb, "opus api enc create fail, (%d,%s)", err, ret2str[-err]);
        Close();
        return OPUS_API_RET_FAIL;
    }
    opus_encoder_ctl(_hd, OPUS_SET_EXPERT_FRAME_DURATION(OPUS_FRAMESIZE_ARG));
    opus_encoder_ctl(_hd, OPUS_SET_FORCE_MODE(OPUS_AUTO));
    opus_encoder_ctl(_hd, OPUS_SET_BANDWIDTH(OPUS_AUTO));
    opus_encoder_ctl(_hd, OPUS_SET_VBR(_useVbr));
    opus_encoder_ctl(_hd, OPUS_SET_BITRATE((int)_bitRate));
    opus_encoder_ctl(_hd, OPUS_SET_COMPLEXITY((int)_complexity));
    if (_isWithHead) {
        _run_cb = opus_encoder_run_with_head;
    }
    else {
        _run_cb = opus_encoder_run_no_head;
    }
    _pcmFrameSample = _fs * _frame0p1Ms / 10000;
    _pcmFrameByte = _ch * 2 * _pcmFrameSample;
    _encodedFrameByte = _bitRate * _frame0p1Ms / 80000;
    return OPUS_API_RET_SUCCESS;
}
OpusApiRet_t OpusEncNormal_c::Close(){
    if (_hd) {
        opus_encoder_destroy(_hd);
    }
    return OPUS_API_RET_SUCCESS;
}
OpusApiRet_t OpusEncNormal_c::Set(const char* choose, void* val){
    uint64_t key = Str2Key(choose);
    switch (key) {
    case Str2Key("basePort"): {
        _basePort = (OpusApi_BasePort_t*)val;
        break;
    }
    case Str2Key("fs"): {
        _fs = (uint32_t)val;
        break;
    }
    case Str2Key("ch"): {
        _ch = (uint32_t)val;
        break;
    }
    case Str2Key("hasHead"): {
        _isWithHead = (bool)(uint32_t)val;
        break;
    }
    case Str2Key("bitrate"): {
        _bitRate = (uint32_t)val;
        if(_hd) opus_encoder_ctl(_hd, OPUS_SET_BITRATE((int)_bitRate));
        break;
    }
    case Str2Key("f0p1ms"):{
        _frame0p1Ms = (uint32_t)val;
        if (_hd) {
            int fd;
            if (toOpusFrameDuration((int)_frame0p1Ms, &fd) != OPUS_API_RET_SUCCESS)
                return OPUS_API_RET_FAIL;
            opus_encoder_ctl(_hd, OPUS_SET_EXPERT_FRAME_DURATION(fd));
        }
        break;
    }
    case Str2Key("vbr"):{
        _useVbr = (bool)(uint32_t)val;
        if(_hd) opus_encoder_ctl(_hd, OPUS_SET_VBR(!!(int)_useVbr));
        break;
    }
    case Str2Key("cpx"):{
        _complexity = (uint32_t)val;
        if (_hd) opus_encoder_ctl(_hd, OPUS_SET_COMPLEXITY((int)_complexity));
        break;
    }
    case Str2Key("encmode"):{
        _encMode = (int32_t)val;
        if (_hd) opus_encoder_ctl(_hd, OPUS_SET_FORCE_MODE(_encMode));
        break;
    }
    case Str2Key("outch"): {
        _ch = (uint32_t)val;
        if (_hd) opus_encoder_ctl(_hd, OPUS_SET_FORCE_CHANNELS(_ch));
        break;
    }
    case Str2Key("app"): {
        _application = (uint32_t)val;
        if (_hd) opus_encoder_ctl(_hd, OPUS_SET_APPLICATION(_application));
        break;
    }
    case Str2Key("stackTb"): _stackTable = (uint32_t(*)[2])val; break;
    default:
        break;
    }
    return OPUS_API_RET_SUCCESS;
}
OpusApiRet_t OpusEncNormal_c::Get(const char* choose, void* val) {
    return OPUS_API_RET_SUCCESS;
}
OpusApiRet_t OpusEncNormal_c::Run(OpusData_c& iData, OpusData_c& oData) {
    return _run_cb(this, iData, oData);
}

EXTERNC void OpusEncoderNormalRegister() {
    OpusCodecCreaterRegister<OpusEncNormal_c>(OPUS_ENCODE_TO_ID(OpusApi_EncChoose_e::OPUS_API_ENC_CHOOSE_NORMAL));
}

