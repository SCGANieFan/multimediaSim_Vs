#pragma once
#include "opus_api_private.h"
#include "opus.h"

class OpusEncNormal_c :public OpusCodec_c {
public:
    OpusEncNormal_c() {}
    virtual ~OpusEncNormal_c() {}
private:
    static void int_to_char(opus_uint32 i, unsigned char ch[4]);
    static OpusApiRet_t toOpusFrameDuration(int in, int* out);
    static OpusApiRet_t opus_encoder_run_no_head(OpusEncNormal_c* enc, OpusData_c& iData, OpusData_c& oData);
    static OpusApiRet_t opus_encoder_run_with_head(OpusEncNormal_c* enc, OpusData_c& iData, OpusData_c& oData);
public:
    virtual OpusApiRet_t Open() final;
    virtual OpusApiRet_t Close() final;
    virtual OpusApiRet_t Set(const char* choose, void* val) final;
    virtual OpusApiRet_t Get(const char* choose, void* val) final;
    //virtual OpusApiRet_t Run(short* in, int inSample, unsigned char* out, int* outByte) final;
    virtual OpusApiRet_t Run(OpusData_c& iData, OpusData_c& oData) final;
public:
    OpusApi_BasePort_t* _basePort = 0;
    OpusEncoder* _hd = 0;
    OpusApiRet_t(*_run_cb)(OpusEncNormal_c* encApi, OpusData_c& iData, OpusData_c& oData) = 0;
    uint32_t _bitRate = 12000;
    uint32_t _frame0p1Ms = 200;
    uint32_t _fs = 0;
    uint32_t _application = OPUS_APPLICATION_AUDIO;
    uint8_t _ch = 0;
    uint8_t _complexity = 0;
    uint8_t _encOutChannels = 0;
    int32_t _encMode = OPUS_AUTO;
    uint32_t(*_stackTable)[2] = 0;
    uint32_t _pcmFrameSample = 0;
    uint32_t _pcmFrameByte = 0;
    uint32_t _encodedFrameByte = 0;
    bool _useVbr = false;
    bool _isWithHead = false;
public:
    static uint32_t enc_stack_table[5][2];
};

