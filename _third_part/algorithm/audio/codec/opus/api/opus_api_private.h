#pragma once
#include <stdint.h>
#include "opus_api.h"

#ifdef WIN32
#define OPUS_OPEN_ENC 1
#define OPUS_OPEN_DEC 1
#define OPUS_DEBUG 1
#else
#define OPUS_DEBUG 1
#endif

#if OPUS_DEBUG
#define LOG(func,fmt,...) if(func) func("[%s](%d)" fmt,__func__, __LINE__, ##__VA_ARGS__)
#else
#define LOG(func,fmt,...) 
#endif

#define OPUS_ENCODE_TO_ID(x) (((uint32_t)(OpusApi_EncChoose_c)(x))<<16)
#define OPUS_DECODE_TO_ID(x) ((uint32_t)(OpusApi_DecChoose_c)(x))

class OpusApiBase_c {
public:
    OpusApiBase_c() {}
    virtual ~OpusApiBase_c() {}
public:
    void* operator new(size_t size, void* buf) {
        return buf;
    }
    void operator delete(void* buf, size_t size) {
        return;
    }
    void operator delete(void* buf) {
        return;
    }
};

//enc
class OpusEnc_c :public OpusApiBase_c
{
public:
    OpusEnc_c() {}
    virtual ~OpusEnc_c() {}
public:
    virtual OpusApiRet_t Init(OpusApi_BasePort_t* basePort, int fs, int channels, bool isWithHead) {
        return OPUS_API_RET_NOT_SUPPORT;
    }
    virtual OpusApiRet_t Deinit() {
        return OPUS_API_RET_NOT_SUPPORT;
    }
    virtual OpusApiRet_t Set(OpusApi_EncSetChhoose_e choose, void* val) {
        return OPUS_API_RET_NOT_SUPPORT;
    }
    virtual OpusApiRet_t Get(OpusApi_EncGetChhoose_e choose, void* val) {
        return OPUS_API_RET_NOT_SUPPORT;
    }
    virtual OpusApiRet_t Run(short* in, int inSample, unsigned char* out, int* outByte) {
        return OPUS_API_RET_NOT_SUPPORT;
    }
};


class OpusApiEnc_c :public OpusApiBase_c
{
public:
    OpusApiEnc_c() {
        _enc = 0;
        _basePort.malloc_cb = 0;
        _basePort.realloc_cb = 0;
        _basePort.free_cb = 0;
        _basePort.print_cb = 0;
    }
    virtual ~OpusApiEnc_c() {}
public:
    static OpusApiRet_t Create(void** pHd, OpusApi_BasePort_t* basePort, int fs, int channels, bool isWithHead, OpusApi_EncChoose_c choose);
    static OpusApiRet_t Destory(void* hd);
    static OpusApiRet_t Set(void* hd, OpusApi_EncSetChhoose_e choose, void* val);
    static OpusApiRet_t Get(void* hd, OpusApi_EncGetChhoose_e choose, void* val);
    static OpusApiRet_t Run(void* hd, short* in, int inSample, unsigned char* out, int* outByte);
public:
    class OpusEnc_c* _enc = 0;
    OpusApi_BasePort_t _basePort;
};

//dec
class OpusDec_c :public OpusApiBase_c
{
public:
    OpusDec_c() {}
    virtual ~OpusDec_c() {}
public:
    virtual OpusApiRet_t Init(OpusApi_BasePort_t* basePort, int fs, int channels) {
        return OPUS_API_RET_NOT_SUPPORT;
    }
    virtual OpusApiRet_t Deinit() {
        return OPUS_API_RET_NOT_SUPPORT;
    }
    virtual OpusApiRet_t Set(OpusApi_DecSetChhoose_e choose, void* val) {
        return OPUS_API_RET_NOT_SUPPORT;
    }
    virtual OpusApiRet_t Get(OpusApi_DecGetChhoose_e choose, void* val) {
        return OPUS_API_RET_NOT_SUPPORT;
    }
    virtual OpusApiRet_t Run(unsigned char* in, int inByte, short* out, int* outSample, bool isPlc) {
        return OPUS_API_RET_NOT_SUPPORT;
    }
};


class OpusApiDec_c :public OpusApiBase_c
{
public:
    OpusApiDec_c() {
        _basePort.malloc_cb = 0;
        _basePort.realloc_cb = 0;
        _basePort.free_cb = 0;
        _basePort.print_cb = 0;
        _dec = 0;
    }
    virtual ~OpusApiDec_c() {}
public:
    static OpusApiRet_t Create(void** pHd, OpusApi_BasePort_t* basePort, int fs, int channels, OpusApi_DecChoose_c choose);
    static OpusApiRet_t Destory(void* hd);
    static OpusApiRet_t Set(void* hd, OpusApi_DecSetChhoose_e choose, void* val);
    static OpusApiRet_t Get(void* hd, OpusApi_DecGetChhoose_e choose, void* val);
    static OpusApiRet_t Run(void* hd, unsigned char* in, int inByte, short* out, int* outSample, bool isPlc);
public:
    OpusApi_BasePort_t _basePort;
    OpusDec_c* _dec;
};




struct OpusCreater_t* OpusCodecCreaterFind(uint32_t id);
void OpusCodecCreaterRegister(struct OpusCreater_t* opusCreater, uint32_t id, void* (*creater)(OpusApi_BasePort_t* bp));

template<class T>
void OpusCodecCreaterRegister(uint32_t id) {
    struct OpusCreater_t* opusCreater = OpusCodecCreaterFind(id);
    if (!opusCreater) { opusCreater = OpusCodecCreaterFind(0); }
    if (!opusCreater) { return; }
    OpusCodecCreaterRegister(opusCreater, id,
        [](OpusApi_BasePort_t* bp) {
            void* ptr = bp->malloc_cb(sizeof(T));
            new(ptr) T();
            return ptr;
        });
}
void* OpusCodecCreaterDoCreate(uint32_t id, OpusApi_BasePort_t* bp);

void OpusEncoderNoneRegister();
void OpusDecoderNoneRegister();



