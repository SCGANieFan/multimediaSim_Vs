#pragma once
#include "opus.h"
#include "gasf.h"
#include "opus_codec_com.h"

class OpusDec_c :public OpusCodec_c
{
public:
    OpusDec_c() {}
    virtual ~OpusDec_c() {}
public:
    enum DataFlag_e : uint32_t {
        IsPlc = OpusData_c::CustReserve0,
    };
public:
    virtual OpusRet_t Open() override;
    virtual OpusRet_t Set(uint32_t key, void* val) override;
    virtual OpusRet_t Get(uint32_t key, void* val) override;
    virtual OpusRet_t Run(OpusData_c& iData, OpusData_c& oData) override;
    //virtual OpusRet_t Receive(GasfData_c& iData) override;
    //virtual OpusRet_t Generate(GasfData_c& oData) override;
    virtual OpusRet_t Close() override;
#if 0
public:
    template<uint8_t Ch>
    static uint32_t Byte2Sample(uint32_t byte) {
        return byte >> (2 * Ch);
    }
#endif
public:
    OpusBasePort_t _opus_bp;
    OpusDecoder* _hd = 0;
    uint32_t _fs = 0;
    uint32_t _framePcmByte = 0;
    uint32_t _framePcmSample = 0;
    uint8_t _ch = 0;
    bool _isFirstRun = true;
};
