#pragma once
#include "gasf.h"
#include "opus.h"
#include "opus_codec_com.h"
#include "opus_private.h"

class OpusEnc_c :public OpusCodec_c
{
public:
    OpusEnc_c();
    virtual ~OpusEnc_c();
public:
    virtual OpusRet_t Open() noexcept override;
    virtual OpusRet_t Set(uint32_t key, void* val) noexcept override;
    virtual OpusRet_t Get(uint32_t key, void* val) noexcept override;
    virtual OpusRet_t Run(OpusData_c& iData, OpusData_c& oData) noexcept override;
    //virtual OpusRet_t Receive(GasfData_c& iData) override;
    //virtual OpusRet_t Generate(GasfData_c& oData) override;
    virtual OpusRet_t Close() noexcept override;
private:
    OpusRet_t RunNoHead(OpusData_c& iData, OpusData_c& oData);
    OpusRet_t RunWithHead(OpusData_c& iData, OpusData_c& oData);
public:
    OpusBasePort_t _opus_bp;
    OpusEncoder* _hd = 0;
    uint32_t _encStackTable[5][2];
    uint32_t _bitRate = 12000;
    uint32_t _frame0p1Ms = 200;
    uint32_t _fs = 0;
    uint32_t _application = OPUS_APPLICATION_AUDIO;
    uint8_t _ch = 0;
    uint8_t _complexity = 0;
    uint8_t _encOutChannels = 0;
    int32_t _encMode = MODE_CELT_ONLY;
    uint32_t _pcmFrameSample = 0;
    uint32_t _pcmFrameByte = 0;
    uint32_t _encodedFrameByte = 0;
    bool _useVbr = false;
    bool _isWithHead = false;
private:
};
