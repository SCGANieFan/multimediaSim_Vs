#pragma once
#include "opus_api_private.h"
#include "opus.h"

class OpusDecNormal_c :public OpusDec_c
{
public:
    OpusDecNormal_c() {}
    virtual ~OpusDecNormal_c() {}
public:
    virtual OpusApiRet_t Init(OpusApi_BasePort_t* basePort, int fs, int channels) final;
    virtual OpusApiRet_t Deinit() final;
    virtual OpusApiRet_t Set(OpusApi_DecSetChhoose_e choose, void* val) final;
    virtual OpusApiRet_t Get(OpusApi_DecGetChhoose_e choose, void* val) final;
    virtual OpusApiRet_t Run(unsigned char* in, int inByte, short* out, int* outSample, bool isPlc) final;
public:
    OpusApi_BasePort_t* _basePort = 0;
    OpusDecoder* _hd = 0;
};

