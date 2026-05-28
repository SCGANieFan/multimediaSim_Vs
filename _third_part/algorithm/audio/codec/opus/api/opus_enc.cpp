#include "opus.h"
#include "opus_private.h"
#include "celt.h"
#include "opus_enc.h"

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

static OpusRet_t toOpusFrameDuration(int in, int* out) {
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
        return OPUS_RET_FAIL;
    }
    *out = out0;
    return OPUS_RET_SUCCESS;
}


static void int_to_char(opus_uint32 i, unsigned char ch[4]) {
    ch[0] = i >> 24;
    ch[1] = (i >> 16) & 0xFF;
    ch[2] = (i >> 8) & 0xFF;
    ch[3] = i & 0xFF;
}

OpusRet_t OpusEnc_c::RunNoHead(OpusData_c& iData, OpusData_c& oData) {
    if (iData.Size() < _pcmFrameByte) {
        LOG_OPUS("%d < %d", iData.Size(), _pcmFrameByte);
        return OPUS_RET_FAIL;
    }
    int ret = opus_encode(_hd, (short*)iData.Data(), _pcmFrameSample, (unsigned char*)oData.LeftData(), oData.LeftSize());
    if (ret < 0) {
        LOG_OPUS("enc fail, (%d,%s), (%p,%p,%d,%p,%d)",
            ret, ret2str[-ret], _hd, iData.Data(), iData.Size(), oData.LeftData(), oData.LeftSize());
        oData.Used(oData.Size());
        return OPUS_RET_FAIL;
    }
    iData.Used(_pcmFrameByte);
    if (ret != oData.Append(ret)){
        LOG_OPUS("odata is not enough, %d", ret, oData.Size());
        return OPUS_RET_FAIL;
    }
    return OPUS_RET_SUCCESS;
}
OpusRet_t OpusEnc_c::RunWithHead(OpusData_c& iData, OpusData_c& oData) {
    const int headByte = 8;
    if (iData.Size() < _pcmFrameByte) {
        LOG_OPUS("%d < %d", iData.Size(), _pcmFrameByte);
        return OPUS_RET_FAIL;
    }
    if (oData.LeftSize() < headByte) {
        LOG_OPUS("outByte not enough (%d,%d)", oData.LeftSize(), headByte);
        //oData._max = 0;
        return OPUS_RET_FAIL;
    }
    int ret = opus_encode(_hd, (short*)iData.Data(), _pcmFrameSample, (uint8_t*)oData.LeftData() + headByte, oData.LeftSize() - headByte);
    if (ret < 0) {
        LOG_OPUS("enc fail, (%d,%s) (%p,%p,%d,%p,%d)",
            ret, ret2str[-ret], _hd, iData.Data(), iData.Size(), oData.LeftData(), oData.LeftSize());
        return OPUS_RET_FAIL;
    }
    iData.Used(_pcmFrameByte);
    int_to_char(ret, (uint8_t*)oData.LeftData());
    opus_uint32 enc_final_range = 0;
    opus_encoder_ctl(_hd, OPUS_GET_FINAL_RANGE(&enc_final_range));
    int_to_char(enc_final_range, (uint8_t*)oData.LeftData() + 4);
    if (oData.Append(ret + 8) < (ret + 8)) {
        LOG_OPUS("outByte not enough (%d,%d)", oData.LeftSize(), ret + 8);
        return OPUS_RET_FAIL;
    }
    return OPUS_RET_SUCCESS;
}
OpusEnc_c::OpusEnc_c(){
    _encStackTable[0][0] = 31 * 1024;//8k1ch
    _encStackTable[0][1] = 31 * 1024;//8k2ch
    _encStackTable[1][0] = 31 * 1024;//12k1ch
    _encStackTable[1][1] = 31 * 1024;//12k2ch
    _encStackTable[2][0] = 31 * 1024;//16k1ch
    _encStackTable[2][1] = 31 * 1024;//16k3ch
    _encStackTable[3][0] = 31 * 1024;//24k1ch
    _encStackTable[3][1] = 31 * 1024;//24k2ch
    _encStackTable[4][0] = 31 * 1024;//48k1ch
    _encStackTable[4][1] = 31 * 1024;//48k2ch
}
OpusEnc_c::~OpusEnc_c() {
}

OpusRet_t OpusEnc_c::Open() noexcept {
    LOG_OPUS("v%s", OPUS_VERSION);
    int err = 0;
    uint8_t idxFs = 0;
    uint8_t idxCh = 0;
    if (_fs == 8000) { idxFs = 0; }
    else if (_fs == 12000) { idxFs = 1; }
    else if (_fs == 16000) { idxFs = 2; }
    else if (_fs == 24000) { idxFs = 3; }
    else if (_fs == 48000) { idxFs = 4; }
    else { return OPUS_RET_FAIL; }

    if (_ch == 1) { idxCh = 0; }
    else if (_ch == 2) { idxCh = 1; }
    else { return OPUS_RET_FAIL; }

    int stackSize = _encStackTable[idxFs][idxCh];
    _opus_bp.malloc_cb = _bp.malloc_cb;
    _opus_bp.free_cb = _bp.free_cb;
    _opus_bp.realloc_cb = _bp.realloc_cb;

    _hd = opus_encoder_create((OpusBasePort_t*)&_opus_bp, _fs, _ch, _application, &err, stackSize);
    if (!_hd) {
        LOG_OPUS("opus api enc create fail, (%d,%s)", err, ret2str[-err]);
        Close();
        return OPUS_RET_FAIL;
    }
    int fd;
    if (toOpusFrameDuration((int)_frame0p1Ms, &fd) != OPUS_RET_SUCCESS)return OPUS_RET_FAIL;
    opus_encoder_ctl(_hd, OPUS_SET_EXPERT_FRAME_DURATION(fd)); //OPUS_FRAMESIZE_ARG
    opus_encoder_ctl(_hd, OPUS_SET_FORCE_MODE(_encMode)); //OPUS_AUTO
    opus_encoder_ctl(_hd, OPUS_SET_BANDWIDTH(OPUS_BANDWIDTH_FULLBAND)); //OPUS_AUTO
    opus_encoder_ctl(_hd, OPUS_SET_VBR(!!(int)_useVbr));
    opus_encoder_ctl(_hd, OPUS_SET_BITRATE((int)_bitRate));
    opus_encoder_ctl(_hd, OPUS_SET_COMPLEXITY((int)_complexity));
    opus_encoder_ctl(_hd, OPUS_SET_SIGNAL((int)_signalType));
    _pcmFrameSample = _fs * _frame0p1Ms / 10000;
    _pcmFrameByte = _ch * 2 * _pcmFrameSample;
    _encodedFrameByte = _bitRate * _frame0p1Ms / 80000;
    return OPUS_RET_SUCCESS;
}
OpusRet_t OpusEnc_c::Set(uint32_t key, void* val) noexcept {
    switch (key) {
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
        if (_hd) opus_encoder_ctl(_hd, OPUS_SET_BITRATE((int)_bitRate));
        break;
    }
    case Str2Key("f0p1ms"): {
        if (_hd) return OPUS_RET_FAIL;
        _frame0p1Ms = (uint32_t)val;
        break;
    }
    case Str2Key("vbr"): {
        _useVbr = (bool)(uint32_t)val;
        if (_hd) opus_encoder_ctl(_hd, OPUS_SET_VBR(!!(int)_useVbr));
        break;
    }
    case Str2Key("cpx"): {
        _complexity = (uint32_t)val;
        if (_hd) opus_encoder_ctl(_hd, OPUS_SET_COMPLEXITY((int)_complexity));
        break;
    }
    case Str2Key("encmode"): {
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
    case Str2Key("signalType"): {
        _signalType = (uint32_t)val;
        if (_hd) opus_encoder_ctl(_hd, OPUS_SET_SIGNAL(_signalType));
        break;
    }
    //case Str2Key("stackTb"): _stackTable = (uint32_t(*)[2])val; break;
    case Str2Key("stk8k1ch"):
        _encStackTable[0][0] = (uint32_t)val; return OPUS_RET_SUCCESS;
    case Str2Key("stk8k2ch"):
        _encStackTable[0][1] = (uint32_t)val; return OPUS_RET_SUCCESS;
    case Str2Key("stk12k1ch"):
        _encStackTable[1][0] = (uint32_t)val; return OPUS_RET_SUCCESS;
    case Str2Key("stk12k2ch"):
        _encStackTable[1][1] = (uint32_t)val; return OPUS_RET_SUCCESS;
    case Str2Key("stk16k1ch"):
        _encStackTable[2][0] = (uint32_t)val; return OPUS_RET_SUCCESS;
    case Str2Key("stk16k2ch"):
        _encStackTable[2][1] = (uint32_t)val; return OPUS_RET_SUCCESS;
    case Str2Key("stk24k1ch"):
        _encStackTable[3][0] = (uint32_t)val; return OPUS_RET_SUCCESS;
    case Str2Key("stk24k2ch"):
        _encStackTable[3][1] = (uint32_t)val; return OPUS_RET_SUCCESS;
    case Str2Key("stk48k1ch"):
        _encStackTable[4][0] = (uint32_t)val; return OPUS_RET_SUCCESS;
    case Str2Key("stk48k2ch"):
        _encStackTable[4][1] = (uint32_t)val; return OPUS_RET_SUCCESS;
    default:
        break;
    }
    return OPUS_RET_SUCCESS;
}
OpusRet_t OpusEnc_c::Get(uint32_t key, void* val) noexcept {
    return OPUS_RET_SUCCESS;
}
OpusRet_t OpusEnc_c::Run(OpusData_c& iData, OpusData_c& oData) noexcept {
    if (_isWithHead) {
        return RunWithHead(iData, oData);
    }
    return RunNoHead(iData, oData);
}
OpusRet_t OpusEnc_c::Close() noexcept {
    if (_hd) {
        opus_encoder_destroy(_hd);
    }
    return OPUS_RET_SUCCESS;
}


