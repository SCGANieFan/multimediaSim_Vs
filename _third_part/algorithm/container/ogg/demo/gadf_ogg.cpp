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
            oData._flag = 1;
            return true;
        }
        uint8_t* iHead = (uint8_t*)_buf + _bufUsedByte;
        uint32_t frameByte = ((uint32_t)iHead[0] << 24) | ((uint32_t)iHead[1] << 16) | ((uint32_t)iHead[2] << 8) | (uint32_t)iHead[3];
        if ((_bufByteMax - _bufUsedByte) < (frameByte + 8)) {
            _isEos = true;
            oData._flag = 1;
            return true;
        }
        oData._buf = iHead + 8;
        oData._size = frameByte;
        oData._offset = 0;
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
        oData._buf = _oBuf;
        oData._max = _oBufMax;
    }
    if (iData._size) {
        memcpy(oData.LeftData(), iData.Data(), iData._size);
        oData._size += iData._size;
        iData._size = 0;
        iData._offset = 0;
    }
    oData._flag = iData._flag;
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
    oData._buf = oBuf;
    oData._max = oDataMax;
    return true;
}
bool GadfOggSinkFile_c::DeInit() {
    if (oData._buf) _bp._free(oData._buf);
    oData._buf = 0;
    oData._max = 0;
    bool ret = ogg_demo_muxer_deinit(_ogg);
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
    int32_t iByte = iData._size;
    uint8_t* oBuff = (uint8_t*)oData.LeftData();
    int32_t oByte = oData.LeftSize();

    bool isEos = iData._flag;
    bool ret = ogg_demo_muxer_run(_ogg, iBuff, &iByte, oBuff, &oByte, isEos);
    if (!ret) {
        return false;
    }
    iData._size -= iByte;
    iData._offset += iByte;
    if (!iData._size) {
        iData._offset = 0;
    }
    oData._size += oByte;
    if (oData._size) {
        GadfFileWrite(_fp, oData.Data(), oData._size);
        _bufByte += oData._size;
        oData._size -= oData._size;
    }
    return true;
}


