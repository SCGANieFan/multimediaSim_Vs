#pragma once
#include "gasf_base.h"

namespace GASF_NAME_SPACE {

class GasfData_c :public GasfBase_c
{
public:
    enum DataFlag_e : uint32_t
    {
        None = 0,
        Reserve0 = 1u << 10,
        Reserve1 = 1u << 11,
        Reserve2 = 1u << 12,
        Reserve3 = 1u << 13,
        Reserve4 = 1u << 14,
        Reserve5 = 1u << 15,
        Reserve6 = 1u << 16,
        Reserve7 = 1u << 17,
        CustReserve0 = 1u << 20,
        CustReserve1 = 1u << 21,
        CustReserve2 = 1u << 22,
        CustReserve3 = 1u << 23,
        CustReserve4 = 1u << 24,
        CustReserve5 = 1u << 25,
        CustReserve6 = 1u << 26,
        CustReserve7 = 1u << 27,
        Max = 1u << 31,
    };
    enum DataStoreMode_e : u8
    {
        Continue = 1u << 0,
        Interlace = 1u << 1,
    };
public:
    GasfData_c() {}
    ~GasfData_c() {}
public:
    void Init(void* buf, uint32_t max, uint32_t size = 0);
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
    //flag
    uint32_t SetFlag(uint32_t flag) { _flag |= flag; return _flag; }
    uint32_t CleanFlag(uint32_t flag) { _flag &= (~flag); return _flag; }
    uint32_t Flag() { return _flag; }
    bool HasFlag(uint32_t flag) { return _flag & flag; }
private:
    void* _buf = 0;
    uint32_t _size = 0;
    uint32_t _max = 0;
    uint32_t _offset = 0;
    uint32_t _flag = DataFlag_e::None;
    //DataStoreMode_e _storeMode = DataStoreMode_e::Continue;
};

};

