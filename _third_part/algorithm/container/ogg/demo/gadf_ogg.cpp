#include <string.h>
#include "gadf_ogg.h"
#include "gadf_porting_api.h"
#include "ogg_demo.h"

bool GadfOggMuxSourceArray_c::Set(const char* key, void* val) {
    uint64_t key0 = Str2Key(key);
    switch (key0)
    {
    case Str2Key("type"):_type = (const char*)val; return true;
    default:
        break;
    }
    return GadfSourceArray_c::Set(key, val);
}

bool GadfOggMuxSourceArray_c::Generate(GadfData_c& oData) {
    if (!_buf
        || !_bufByteMax
        || !_type) {
        return false;
    }
    if (Str2Key(_type) == Str2Key("opus")) {
        if (_isEos) {
            return false;
        }
        if ((_bufByteMax - _bufUsedByte) < 8) {
            _isEos = true;
            oData.Flag(0x00000001);
            return true;
        }
        uint8_t* iHead = (uint8_t*)_buf + _bufUsedByte;
        uint32_t frameByte = ((uint32_t)iHead[0] << 24) | ((uint32_t)iHead[1] << 16) | ((uint32_t)iHead[2] << 8) | (uint32_t)iHead[3];
        if ((_bufByteMax - _bufUsedByte) < (frameByte + 8)) {
            _isEos = true;
            oData.Flag(0x00000001);
            return true;
        }
        oData.Init(iHead + 8, frameByte, frameByte);
        _bufUsedByte += frameByte + 8;
        return true;
    }
    else {
        return false;
    }
}

bool GadfOggMux_c::Set(const char* key, void* val) {
#if 0
    uint64_t key0 = Str2Key(key);
    switch (key0)
    {
    case Str2Key("fs"):_fs = (uint32_t)val; return true;
    default:
        break;
    }
#endif
    return GadfAlgo_c::Set(key, val);
}


bool GadfOggMux_c::Init() {
    if (!GadfAlgo_c::Init())return false;
    _oBufMax = 5 * 1024;
    _oBuf = _bp._malloc(_oBufMax);
    if (_oBuf)return false;
    return true;
}
bool GadfOggMux_c::Process(GadfData_c& iData, GadfData_c& oData) {
    if (!oData.Data()) {
        oData.Init(_oBuf, _oBufMax);
    }
    uint32_t iSize = iData.Size();
    if (iSize) {
        memcpy(oData.LeftData(), iData.Data(), iSize);
        oData.Append(iSize);
        iData.Used(iSize);
        iData.Clear();
    }
    oData.Flag(iData.Flag());
    return true;
}
bool GadfOggMux_c::DeInit() {
    if (_oBuf) {
        _bp._free(_oBuf);
    }
    _oBufMax = 0;
    if (!GadfAlgo_c::DeInit())return false;
    return true;
}

bool GadfOggSinkFile_c::Init() {
    if (!GadfSinkFile_c::Init())return false;
    _ogg = ogg_demo_muxer_init(_fs, _ch, _width, _frame0p1Ms);
    if (!_ogg) {
        return false;
    }
    uint32_t oDataMax = 5 * 1024;
    uint8_t* oBuf = (uint8_t*)_bp._malloc(oDataMax);
    if (!oBuf)return false;
    oData.Init(oBuf, oDataMax);
    return true;
}
bool GadfOggSinkFile_c::DeInit() {
    //iBuff
    uint8_t* iBuff = 0;
    int32_t iByte = 0;
    oData.Clear();
    uint8_t* oBuff = (uint8_t*)oData.LeftData();
    int32_t oByte = oData.LeftSize();
    bool ret = ogg_demo_muxer_run(_ogg, iBuff, &iByte, oBuff, &oByte, true);
    if (ret) {
        oData.Append(oByte);
        if (oData.Size()) {
            GadfFileWrite(_fp, oData.Data(), oData.Size());
            oData.Used(oData.Size());
        }
    }

    if (oData.Buf()) _bp._free(oData.Buf());
    oData.DeInit();

    ret = ogg_demo_muxer_deinit(_ogg);
    if (!ret) return false;
    _ogg = 0;
    if (!GadfSinkFile_c::DeInit())return false;
    return true;
}
bool GadfOggSinkFile_c::Set(const char* key, void* val) {
    uint64_t key0 = Str2Key(key);
    switch (key0)
    {
    case Str2Key("fs"):_fs = (uint32_t)val; return true;
    case Str2Key("ch"):_ch = (uint8_t)(uint32_t)val; return true;
    case Str2Key("width"):_width = (uint8_t)(uint32_t)val; return true;
    case Str2Key("fd0p1ms"):_frame0p1Ms = (uint32_t)val; return true;
    default:
        break;
    }
    return GadfSinkFile_c::Set(key, val);
}
bool GadfOggSinkFile_c::Receive(GadfData_c& iData) {
    if (!_fp) {
        return false;
    }
    uint8_t* iBuff = (uint8_t*)iData.Data();
    int32_t iByte = iData.Size();
    uint8_t* oBuff = (uint8_t*)oData.LeftData();
    int32_t oByte = oData.LeftSize();
    bool isEos = false;
    bool ret = ogg_demo_muxer_run(_ogg, iBuff, &iByte, oBuff, &oByte, isEos);
    if (!ret) {
        return false;
    }
    iData.Used(iByte);
    iData.Clear();
    oData.Append(oByte);
    if (oData.Size()) {
        GadfFileWrite(_fp, oData.Data(), oData.Size());
        oData.Used(oData.Size());
    }
    return true;
}


