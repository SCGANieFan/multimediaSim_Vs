#pragma once
#include "opus_api_private.h"
#include "opus.h"

class OpusEncNormal_c :public OpusEnc_c {
public:
    OpusEncNormal_c() {}
    virtual ~OpusEncNormal_c() {}
private:
    static void int_to_char(opus_uint32 i, unsigned char ch[4]);
    static OpusApiRet_t toOpusFrameDuration(int in, int* out);
    static OpusApiRet_t opus_encoder_run_no_head(OpusEncNormal_c* enc, short* in, int inSample, unsigned char* out, int* outByte);
    static OpusApiRet_t opus_encoder_run_with_head(OpusEncNormal_c* enc, short* in, int inSample, unsigned char* out, int* outByte);
public:
    virtual OpusApiRet_t Init(OpusApi_BasePort_t* basePort, int fs, int channels, bool isWithHead) final;
    virtual OpusApiRet_t Deinit() final;
    virtual OpusApiRet_t Set(OpusApi_EncSetChhoose_e choose, void* val) final;
    virtual OpusApiRet_t Get(OpusApi_EncGetChhoose_e choose, void* val) final;
    virtual OpusApiRet_t Run(short* in, int inSample, unsigned char* out, int* outByte) final;
public:
    OpusApi_BasePort_t* _basePort;
    OpusEncoder* _hd;
    OpusApiRet_t(*_run_cb)(OpusEncNormal_c* encApi, short* in, int inSample, unsigned char* out, int* outByte);
};

