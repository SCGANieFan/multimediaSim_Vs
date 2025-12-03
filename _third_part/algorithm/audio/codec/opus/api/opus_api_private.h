#pragma once
#include <stdint.h>
#include <cstddef>
#include "opus_api.h"

#ifdef WIN32
#define OPUS_OPEN_ENC 1
#define OPUS_OPEN_DEC 1
#endif

#if WIN32
#define LOG(func,fmt,...) if(func) func("<%s>[%s](%d)" fmt, Strrchr_m(__FILE__,'\\') + 1,__func__, __LINE__, ##__VA_ARGS__)
#else
#define LOG(func,fmt,...) if(func) func("<%s>[%s](%d)" fmt, Strrchr_m(__FILE__,'/') + 1,__func__, __LINE__, ##__VA_ARGS__)
#endif

#define OPUS_ENCODE_TO_ID(x) (((uint32_t)(OpusApi_EncChoose_e)(x))<<16)
#define OPUS_DECODE_TO_ID(x) ((uint32_t)(OpusApi_DecChoose_e)(x))

class OpusApiBase_c {
public:
    OpusApiBase_c() {}
    virtual ~OpusApiBase_c() {}
public:
    static void* operator new(size_t size, void* buf);
    static void operator delete(void* buf, size_t size);
    static void operator delete(void* buf, void* place);
    static void operator delete(void* buf);
public:
    static constexpr uint64_t Str2Key(const char* str) {
        uint64_t key = 0;
        for (uint8_t n = 0; n < 8; n++) {
            if (str[n] == '\0') {
                break;
            }
            key = (key << 8) | str[n];
        }
        return key;
    }
    static constexpr char* Strrchr_m(const char* str, char c) {
        int32_t strLen = 0;
        while (str[strLen++] != '\0');
        strLen -= 1;
        char* outStr = (char*)str;
        if (c == '\0') {
            return outStr + strLen + 1;
        }
        for (int32_t n = strLen; n > 0; n--) {
            if (outStr[n] == c) {
                return (outStr + n);
            }
        }
        return outStr;
    }
};

//codec
class OpusData_c
{
public:
    OpusData_c() {}
    ~OpusData_c() {}
public:
    void* _buf = 0;
    uint32_t _len = 0;
    uint32_t _max = 0;
    uint32_t _offset = 0;
    uint32_t _flag = 0;
};

class OpusCodec_c :public OpusApiBase_c
{
public:
    OpusCodec_c() {}
    virtual ~OpusCodec_c() {}
public:
    virtual OpusApiRet_t Open() {
        return OPUS_API_RET_NOT_SUPPORT;
    }
    virtual OpusApiRet_t Close() {
        return OPUS_API_RET_NOT_SUPPORT;
    }
    virtual OpusApiRet_t Set(const char* choose, void* val) {
        return OPUS_API_RET_NOT_SUPPORT;
    }
    virtual OpusApiRet_t Get(const char* choose, void* val) {
        return OPUS_API_RET_NOT_SUPPORT;
    }
    virtual OpusApiRet_t Run(OpusData_c& iData, OpusData_c& oData) {
        return OPUS_API_RET_NOT_SUPPORT;
    }
};
using OpusEnc_c = OpusCodec_c;
using OpusDec_c = OpusCodec_c;

//api
class OpusApi_c :public OpusApiBase_c
{
public:
    OpusApi_c() {
        _basePort.malloc_cb = 0;
        _basePort.realloc_cb = 0;
        _basePort.free_cb = 0;
        _basePort.print_cb = 0;
    }
    virtual ~OpusApi_c() {
    }
protected:
    template<class T>
    static OpusApiRet_t Create(void** pHd, OpusApi_BasePort_t* bp) {
        if (!bp || !bp->print_cb) { return OPUS_API_RET_FAIL; }
        LOG(bp->print_cb, "%s, %p", VERSION, pHd);
        if (!pHd) { return OPUS_API_RET_FAIL; }
        *pHd = 0;
        OpusApi_c* api = (OpusApi_c*)bp->malloc_cb(sizeof(T));
        if (!api) {
            LOG(bp->print_cb, "malloc fail, %d", sizeof(T));
            return OPUS_API_RET_FAIL;
        }
        new(api) T();
        api->_basePort = *bp;
        *pHd = api;
        return OPUS_API_RET_SUCCESS;
    }
    static OpusApiRet_t Destory(void* hd) {
        if (!hd) { return OPUS_API_RET_FAIL; }
        OpusApi_c* api = (OpusApi_c*)hd;
        OpusApi_BasePort_t basePort = api->_basePort;
        LOG(basePort.print_cb, "%p", hd);
        api->~OpusApi_c();
        basePort.free_cb(api);
        return OPUS_API_RET_SUCCESS;
    }
public:
    virtual OpusApiRet_t Open() { return OPUS_API_RET_NOT_SUPPORT; }
    virtual OpusApiRet_t Set(const char* choose, void* val) { return OPUS_API_RET_NOT_SUPPORT; }
    virtual OpusApiRet_t Get(const char* choose, void* val) { return OPUS_API_RET_NOT_SUPPORT; }
    virtual OpusApiRet_t Close() { return OPUS_API_RET_NOT_SUPPORT; }
public:
    OpusApi_BasePort_t _basePort;
    static constexpr const char* VERSION = "1.1.0.1";
};

class OpusApiEnc_c :public OpusApi_c
{
public:
    OpusApiEnc_c();
    virtual ~OpusApiEnc_c();
public:
    static OpusApiRet_t Create(void** pHd, OpusApi_CreateEncParam_t* param);
    static OpusApiRet_t Destory(void* hd);
public:
    virtual OpusApiRet_t Open() final;
    virtual OpusApiRet_t Set(const char* choose, void* val) final;
    virtual OpusApiRet_t Get(const char* choose, void* val) final;
    virtual OpusApiRet_t Close() final;
public:
    OpusApiRet_t Run(unsigned char* pcm, int *pcmByte, unsigned char* encodedFrame, int* encodedFrameByte);
public:
    OpusEnc_c* _enc = 0;
    uint32_t _bitRate = 12000;
    uint32_t _frame0p1Ms = 200;
    uint32_t _fs = 0;
    uint32_t application = 2049;
    uint32_t encStackTable[5][2];
    int32_t _encMode = -1000;
    uint8_t _ch = 0;
    uint8_t _complexity = 0;
    uint8_t _encOutChannels = 0;
    uint8_t _choose = OpusApi_EncChoose_e::OPUS_API_ENC_CHOOSE_NONE;
    bool _useVbr = false;
    bool _isWithHead = false;
};


class OpusApiDec_c :public OpusApi_c
{
public:
    OpusApiDec_c();
    virtual ~OpusApiDec_c();
public:
    static OpusApiRet_t Create(void** pHd, OpusApi_CreateDecParam_t* param);
    static OpusApiRet_t Destory(void* hd);
public:
    virtual OpusApiRet_t Open() final;
    virtual OpusApiRet_t Set(const char* choose, void* val) final;
    virtual OpusApiRet_t Get(const char* choose, void* val) final;
    virtual OpusApiRet_t Close() final;
public:
    OpusApiRet_t Run(unsigned char* encodedOneFrame, int encodedOneFrameByte, unsigned char* decodecPcm, int* decodecPcmByte, bool isPlc);
public:
    OpusDec_c* _dec = 0;
    uint32_t _fs = 0;
    uint8_t _ch = 0;
    uint8_t _choose = OpusApi_DecChoose_e::OPUS_API_DEC_CHOOSE_NONE;
};

//register
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


