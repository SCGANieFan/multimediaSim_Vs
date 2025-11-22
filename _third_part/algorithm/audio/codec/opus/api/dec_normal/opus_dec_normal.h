#pragma once
#include "opus_api_private.h"
#include "opus.h"

class OpusDecNormal_c :public OpusCodec_c
{
public:
    OpusDecNormal_c() {}
    virtual ~OpusDecNormal_c() {}
public:
    virtual OpusApiRet_t Open() final;
    virtual OpusApiRet_t Close() final;
    virtual OpusApiRet_t Set(const char* choose, void* val) final;
    virtual OpusApiRet_t Get(const char* choose, void* val) final;
    virtual OpusApiRet_t Run(OpusData_c& iData, OpusData_c& oData) final;
public:
    template<uint8_t Ch>
    static uint32_t Byte2Sample(uint32_t byte) {
        return byte >> (2 * Ch);
    }
public:
    OpusApi_BasePort_t* _basePort = 0;
    OpusDecoder* _hd = 0;
    uint32_t _fs = 0;
    uint32_t _framePcmByte = 0;
    uint32_t _framePcmSample = 0;
    uint8_t _ch = 0;
    bool _isFirstRun = true;
};

