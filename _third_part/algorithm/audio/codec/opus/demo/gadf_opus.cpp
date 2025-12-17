#include <string.h>
#include "gadf_opus.h"
#include "opus_demo.h"

bool GadfOpusDecSourceArray_c::Generate(GadfData_c& oData) {
    if (!_buf
        || !_bufByteMax) {
        return false;
    }
    uint8_t* iHead = (uint8_t*)_buf + _bufUsedByte;
    uint32_t frameByte = ((uint32_t)iHead[0] << 24) | ((uint32_t)iHead[1] << 16) | ((uint32_t)iHead[2] << 8) | (uint32_t)iHead[3];
    if ((_bufByteMax - _bufUsedByte) < (frameByte + 8)) {
        return false;
    }
    oData.Init(iHead + 8, frameByte, frameByte);
    _bufUsedByte += frameByte + 8;
    return true;
}

bool GadfOpusEncSourceArray_c::Set(const char* key, void* val) {
    uint64_t key0 = Str2Key(key);
    switch (key0)
    {
    case Str2Key("fByte"):_fByte = (uint32_t)val; return true;
    default:
        break;
    }
    return GadfSourceArray_c::Set(key, val);
}

bool GadfOpusEncSourceArray_c::Generate(GadfData_c& oData) {
    if (!_buf
        || !_bufByteMax
        || !_fByte) {
        return false;
    }
    if ((_bufByteMax - _bufUsedByte) < _fByte) {
        return false;
    }
    oData.Init(_buf + _bufUsedByte, _fByte, _fByte);
    _bufUsedByte += _fByte;
    return true;
}

bool GadfOpusEnc_c::Set(const char* key, void* val) {
    uint64_t key0 = Str2Key(key);
    switch (key0)
    {
    case Str2Key("fs"):_fs = (uint32_t)val; return true;
    case Str2Key("ch"):_ch = (uint8_t)(uint32_t)val; return true;
    case Str2Key("width"):_width = (uint8_t)(uint32_t)val; return true;
    case Str2Key("bps"):_bitRate = (uint32_t)val; return true;
    case Str2Key("fd0p1ms"):_frame0p1Ms = (uint32_t)val; return true;
    case Str2Key("haveHead"):_isWithHead = (bool)(uint32_t)val; return true;
    default:
        break;
    }
    return GadfAlgo_c::Set(key, val);
}


bool GadfOpusEnc_c::Init() {
    if (!GadfAlgo_c::Init())return false;
    _enc = opus_demo_encoder_init(_fs, _ch, _width, _bitRate, _frame0p1Ms, _isWithHead);
    if (!_enc) {
        return false;
    }
    _oBufMax = 4 * 1024;
    _oBuf = _bp._malloc(_oBufMax);
    if (_oBuf)return false;
    return true;
}
bool GadfOpusEnc_c::Process(GadfData_c& iData, GadfData_c& oData) {
    if (!oData.Data()) {
        oData.Init(_oBuf, _oBufMax);
    }
    uint8_t* iBuff = (uint8_t*)iData.Data();
    int32_t iByte = iData.Size();
    uint8_t* oBuff = (uint8_t*)oData.LeftData();
    int32_t oByte = oData.LeftSize();
    bool ret = opus_demo_encoder_run(_enc, iBuff, &iByte, oBuff, &oByte);
    if (!ret) {
        return false; 
    }
    iData.Used(iByte);
    oData.Append(oByte);
    return true;
}
bool GadfOpusEnc_c::DeInit() {
    bool ret = opus_demo_encoder_deinit(_enc);
    if (!ret) return false;
    _enc = 0;
    if (_oBuf) {
        _bp._free(_oBuf);
    }
    _oBufMax = 0;
    if (!GadfAlgo_c::DeInit())return false;
    return true;
}

bool GadfOpusDec_c::Set(const char* key, void* val) {
    uint64_t key0 = Str2Key(key);
    switch (key0)
    {
    case Str2Key("fs"):_fs = (uint32_t)val; return true;
    case Str2Key("ch"):_ch = (uint8_t)(uint32_t)val; return true;
    case Str2Key("width"):_width = (uint8_t)(uint32_t)val; return true;
    default:
        break;
    }
    return GadfAlgo_c::Set(key, val);
}

bool GadfOpusDec_c::Init() {
    if (!GadfAlgo_c::Init())return false;
    _dec = opus_demo_decoder_init(_fs, _ch, 2);
    if (!_dec)return false;
    _oBufMax = 4 * 1024;
    _oBuf = _bp._malloc(_oBufMax);
    if (_oBuf)return false;

    return true;
}
bool GadfOpusDec_c::Process(GadfData_c& iData, GadfData_c& oData) {
    if (!oData.Data()) {
        oData.Init(_oBuf, _oBufMax);
    }
    uint8_t* iBuff = (uint8_t*)iData.Data();
    uint32_t iByte = iData.Size();
    uint8_t* oBuff = (uint8_t*)oData.LeftData();
    int32_t oByte = oData.LeftSize();
    bool ret = opus_demo_decoder_run(_dec, iBuff, iByte, oBuff, &oByte, false);
    if (!ret) return false;
    iData.Used(iData.Size());
    iData.Clear();
    oData.Append(oByte);
    return true;
}
bool GadfOpusDec_c::DeInit() {
    bool ret = opus_demo_decoder_deinit(_dec);
    if (!ret) return false;
    _dec = 0;
    if (_oBuf) {
        _bp._free(_oBuf);
    }
    _oBufMax = 0;
    if (!GadfAlgo_c::DeInit())return false;

    return true;
}

