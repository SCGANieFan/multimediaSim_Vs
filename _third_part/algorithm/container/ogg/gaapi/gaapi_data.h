#pragma once
#include "gaapi_base.h"

namespace ogg_gaapi_ns {

class GaapiData_c :public GaapiBase_c
{
public:
    GaapiData_c() {}
    ~GaapiData_c() {}
public:
    void Init(void* buf, uint32_t max);
    void Init(void* buf, uint32_t size, uint32_t max);
    void Deinit();
    void* Buf() { return _buf;}
    void* Data() { return (uint8_t*)_buf + _offset; }
    void* LeftData() { return (uint8_t*)_buf + _offset + _size; }
    uint32_t Size() { return _size; }
    uint32_t LeftSize() { return _max - _offset - _size; }
    uint32_t Append(void* buf, uint32_t size);
    uint32_t Append(uint32_t size);
    uint32_t Used() { return _offset; }
    uint32_t Used(uint32_t size);
    uint32_t ClearUsed();
    uint32_t Flag(uint32_t flag) { _flag |= flag; return _flag; }
    uint32_t Flag() { return _flag; }
private:
    void* _buf = 0;
    uint32_t _size = 0;
    uint32_t _max = 0;
    uint32_t _offset = 0;
    uint32_t _flag = 0;
};

};

