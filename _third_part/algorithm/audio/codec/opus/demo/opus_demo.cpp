#include <string.h>
#include <stdarg.h>
#include <stdint.h>
#include <stdio.h>
#include "gadf_porting_api.h"
#include "opus_api.h"

//api demo
#define LOG_OPUS_ORI(fmt,...)    GadfPrint(fmt, ##__VA_ARGS__)
#define LOG_OPUS(fmt,...)        GadfPrint("(%s)[%d]" fmt, __func__, __LINE__, ##__VA_ARGS__)

static char heap_pool[80 * 1024];
static void* opus_heap = 0;

static const char* ret2str[] = {
    "success",
    "fail",
    "not support",
};

static void* opus_malloc(int size)
{
    void* buf = GadfHeapMalloc(opus_heap, size);
    LOG_OPUS("%d,%p", size, buf);
    return buf;
}

static void* opus_realloc(void* rmem, int newsize)
{
    void* buf = GadfHheapRealloc(opus_heap, rmem, newsize);
    LOG_OPUS("%d,%p,%p", newsize, rmem, buf);
    return buf;
}

static void opus_free(void* rmem)
{
    LOG_OPUS("%p", rmem);
    GadfHeapFree(opus_heap, rmem);
    return;
}

static void opus_print(const char* fmt, ...)
{
    static char buf[256];
    va_list ap;
    va_start(ap, fmt);
    vsprintf(buf, fmt, ap);
    va_end(ap);
    LOG_OPUS_ORI("%s", buf);
}

static const char* opusEncEnum2Choose[] = {
    "bitrate",              //bits per second after encoded, range (6k --> 510k), default 12k
    "f0p1ms",               //0.1ms, if frame is 20ms, this value should be 200. range (25,50,100,200,400,600), default 200
    "vbr",                  //if 1, use variable bitrate, default 0//recommend 0, range (0 --> 9), default 0
    "cpx",                  //recommend 0, range (0 --> 9), default 0
    "encmode",              //range (-1000,1000,1001,1002), each means AUTO,SILK_ONLY,HYBRID,CELT_ONLY. default -1000
    "outch",                //force encoded as either mono or stereo, range (1, 2), default OPUS_ENC_SET_CHANNELS
    "app",                  //range (2048,2049,2051), each means VOIP,AUDIO,AUDIO. default 2049
    "choose",               //range (OpusApi_EncChoose_e). default none
    "fs",                   //input pcm sample rate, range (8k,12k,16k,24k,48k), default 0
    "ch",                   //input pcm channels, range (1,2), default 0
    "hasHead",              //if true, have 8 byte header, default false
    "stk8k1ch",             //heap used of opus encode running in 8khz 1ch, deafult 31k
    "stk8k2ch",             //heap used of opus encode running in 8khz 2ch, deafult 31k
    "stk12k1ch",            //heap used of opus encode running in 12khz 1ch, deafult 31k
    "stk12k2ch",            //heap used of opus encode running in 12khz 2ch, deafult 31k
    "stk16k1ch",            //heap used of opus encode running in 16khz 1ch, deafult 31k
    "stk16k2ch",            //heap used of opus encode running in 16khz 2ch, deafult 31k
    "stk24k1ch",            //heap used of opus encode running in 24khz 1ch, deafult 31k
    "stk24k2ch",            //heap used of opus encode running in 24khz 2ch, deafult 31k
    "stk48k1ch",            //heap used of opus encode running in 48khz 1ch, deafult 31k
    "stk48k2ch",            //heap used of opus encode running in 48khz 2ch, deafult 31k
};

typedef enum {
    OPUS_ENC_SET_BIT_RATE = 0,
    OPUS_ENC_SET_FRAME_DURATION_0P1MS,
    OPUS_ENC_SET_USE_VBR,
    OPUS_ENC_SET_COMPLEXITY,
    OPUS_ENC_SET_MODE,
    OPUS_ENC_SET_OUT_CH,
    OPUS_ENC_SET_APPLICATION,
    OPUS_ENC_SET_ENC_CHOOSE,
    OPUS_ENC_SET_SAMPLE_RATE,
    OPUS_ENC_SET_CHANNELS,
    OPUS_ENC_SET_HAS_HEAD,
    OPUS_ENC_SET_STACK_8K1CH,
    OPUS_ENC_SET_STACK_8K2CH,
    OPUS_ENC_SET_STACK_12K1CH,
    OPUS_ENC_SET_STACK_12K2CH,
    OPUS_ENC_SET_STACK_16K1CH,
    OPUS_ENC_SET_STACK_16K2CH,
    OPUS_ENC_SET_STACK_24K1CH,
    OPUS_ENC_SET_STACK_24K2CH,
    OPUS_ENC_SET_STACK_48K1CH,
    OPUS_ENC_SET_STACK_48K2CH,
    OPUS_ENC_SET_MAX,
}OpusEncSetChhoose_e;


static const char* opusDecEnum2Choose[] = {
    "choose",               //range (OpusApi_DecChoose_e). default none
    "fs",                   //pcm sample rate after decode, range (8k,12k,16k,24k,48k), default 0
    "ch",                   //pcm channels after decode, range (1,2), default 0
};

typedef enum {
    OPUS_DEC_SET_DEC_CHOOSE,
    OPUS_DEC_SET_SAMPLE_RATE,
    OPUS_DEC_SET_CHANNELS,
}OpusDecSetChhoose_e;

void* opus_demo_encoder_init(uint32_t rate, uint16_t channels, uint8_t width, uint32_t bitrate, uint32_t frameDMs, bool haveHead) {

    if (!opus_heap) {
        opus_heap = GadfHheapRegister(heap_pool, sizeof(heap_pool));
    }
    OpusEncoderNormalRegister();
    void* enc = 0;

    LOG_OPUS("(%u,%u,%u),(%u,%u,%u)", rate, channels, width, bitrate, frameDMs, haveHead);
    if (channels > 2
        || width != 2) {
        return 0;
    }
    OpusApi_CreateEncParam_t encParam;
    memset(&encParam, 0, sizeof(OpusApi_CreateEncParam_t));
    encParam.basePort.malloc_cb = opus_malloc;
    encParam.basePort.realloc_cb = opus_realloc;
    encParam.basePort.free_cb = opus_free;
    encParam.basePort.print_cb = opus_print;
    OpusApiRet_t ret = opus_api_create_encoder(&enc, &encParam);
    if (ret != OPUS_API_RET_SUCCESS) {
        LOG_OPUS("opus create fail, %s,(%p,%d,%d,%d)", ret2str[-ret], enc, rate, channels, haveHead);
        return 0;
    }

    ret |= opus_api_encoder_set(enc, opusEncEnum2Choose[OPUS_ENC_SET_BIT_RATE], (void*)bitrate);
    ret |= opus_api_encoder_set(enc, opusEncEnum2Choose[OPUS_ENC_SET_FRAME_DURATION_0P1MS], (void*)frameDMs);
    ret |= opus_api_encoder_set(enc, opusEncEnum2Choose[OPUS_ENC_SET_ENC_CHOOSE], (void*)OPUS_API_ENC_CHOOSE_NORMAL);
    ret |= opus_api_encoder_set(enc, opusEncEnum2Choose[OPUS_ENC_SET_SAMPLE_RATE], (void*)rate);
    ret |= opus_api_encoder_set(enc, opusEncEnum2Choose[OPUS_ENC_SET_CHANNELS], (void*)(uint32_t)channels);
    ret |= opus_api_encoder_set(enc, opusEncEnum2Choose[OPUS_ENC_SET_HAS_HEAD], (void*)haveHead);
    ret |= opus_api_encoder_set(enc, opusEncEnum2Choose[OPUS_ENC_SET_STACK_8K1CH], (void*)(31 * 1024));
    ret |= opus_api_encoder_set(enc, opusEncEnum2Choose[OPUS_ENC_SET_STACK_8K2CH], (void*)(31 * 1024));
    ret |= opus_api_encoder_set(enc, opusEncEnum2Choose[OPUS_ENC_SET_STACK_12K1CH], (void*)(31 * 1024));
    ret |= opus_api_encoder_set(enc, opusEncEnum2Choose[OPUS_ENC_SET_STACK_12K2CH], (void*)(31 * 1024));
    ret |= opus_api_encoder_set(enc, opusEncEnum2Choose[OPUS_ENC_SET_STACK_16K1CH], (void*)(29 * 1024));
    ret |= opus_api_encoder_set(enc, opusEncEnum2Choose[OPUS_ENC_SET_STACK_16K2CH], (void*)(29 * 1024));
    ret |= opus_api_encoder_set(enc, opusEncEnum2Choose[OPUS_ENC_SET_STACK_24K1CH], (void*)(31 * 1024));
    ret |= opus_api_encoder_set(enc, opusEncEnum2Choose[OPUS_ENC_SET_STACK_24K2CH], (void*)(31 * 1024));
    ret |= opus_api_encoder_set(enc, opusEncEnum2Choose[OPUS_ENC_SET_STACK_48K1CH], (void*)(20 * 1024));
    ret |= opus_api_encoder_set(enc, opusEncEnum2Choose[OPUS_ENC_SET_STACK_48K2CH], (void*)(31 * 1024));


    if (ret != OPUS_API_RET_SUCCESS) { LOG_OPUS("opus set fail, %d", ret); return 0; }
    ret = opus_api_open_encoder(enc);
    if (ret != OPUS_API_RET_SUCCESS) { LOG_OPUS("opus open fail, %d", ret); return 0; }
    return enc;
}

bool opus_demo_encoder_run(void* enc, uint8_t* pcm, int32_t *pcmByte, uint8_t* encodedFrame, int32_t* encodedFrameByte) {
    uint32_t iByte = *pcmByte;
    OpusApiRet_t ret = opus_api_encoder_run(enc, pcm, pcmByte, encodedFrame, encodedFrameByte);
    static uint32_t f = 0;
    if (f % 10 == 0) { LOG_OPUS("[%u] running,%d,%d", f, iByte,*encodedFrameByte); }
    f++;
    if (ret != OPUS_API_RET_SUCCESS) {
        LOG_OPUS("opus run fail, %s", ret2str[-ret]); return false;
    }
    return true;
}

bool opus_demo_encoder_deinit(void* enc) {
    opus_api_close_encoder(enc);
    opus_api_destory_encoder(enc);
    return true;
}

void* opus_demo_decoder_init(uint32_t rate, uint16_t channels, uint8_t width) {
    if (!opus_heap) {
        opus_heap = GadfHheapRegister(heap_pool, sizeof(heap_pool));
    }
    OpusDecoderNormalRegister();
    LOG_OPUS("%d,%d", rate, channels);
    if (channels > 2) {
        return 0;
    }
    void* dec = 0;
    OpusApi_CreateDecParam_t param;
    memset(&param, 0, sizeof(OpusApi_CreateDecParam_t));
    param.basePort.malloc_cb = opus_malloc;
    param.basePort.realloc_cb = opus_realloc;
    param.basePort.free_cb = opus_free;
    param.basePort.print_cb = opus_print;
    OpusApiRet_t ret = opus_api_create_decoder(&dec, &param);
    if (ret != OPUS_API_RET_SUCCESS) {
        LOG_OPUS("create fail, %s,(%p,%d,%d)", ret2str[-ret], dec, rate, channels);
        return 0;
    }
    ret |= opus_api_decoder_set(dec, opusDecEnum2Choose[OPUS_DEC_SET_DEC_CHOOSE], (void*)OpusApi_DecChoose_e::OPUS_API_DEC_CHOOSE_NORMAL);
    ret |= opus_api_decoder_set(dec, opusDecEnum2Choose[OPUS_DEC_SET_SAMPLE_RATE], (void*)rate);
    ret |= opus_api_decoder_set(dec, opusDecEnum2Choose[OPUS_DEC_SET_CHANNELS], (void*)(uint32_t)channels);
    if (ret != OPUS_API_RET_SUCCESS) {
        LOG_OPUS("set fail"); return 0;
    }
    ret = opus_api_open_decoder(dec);
    if (ret != OPUS_API_RET_SUCCESS) {
        LOG_OPUS("open fail"); return 0;
    }
    return dec;
}

bool opus_demo_decoder_run(void* dec, uint8_t* encodedOneFrame, uint32_t encodedOneFrameByte, uint8_t* decodecPcm, int32_t* decodecPcmByte, bool isDoPlc) {
    static uint32_t frameNum = 0;
    if (frameNum % 10 == 0) { LOG_OPUS("[%u] running, %d", frameNum, encodedOneFrameByte); }
    ++frameNum;
    OpusApiRet_t ret = opus_api_decoder_run(dec, encodedOneFrame, encodedOneFrameByte, decodecPcm, decodecPcmByte, isDoPlc);
    if (ret != OPUS_API_RET_SUCCESS) {
        LOG_OPUS("opus run fail, %s", ret2str[ret]); return false;
    }
    return true;
}

bool opus_demo_decoder_deinit(void* dec) {
    opus_api_close_decoder(dec);
    opus_api_destory_decoder(dec);
    return true;
}


//for test
#include "gadf.h"
static void OpusCodecTest0() {
    //void gadf_register_info_opus_enc_16k1ch(); gadf_register_info_opus_enc_16k1ch();
    //void gadf_register_info_opus_enc_16k2ch(); gadf_register_info_opus_enc_16k2ch();
    //void gadf_register_info_opus_enc_48k1ch(); gadf_register_info_opus_enc_48k1ch();
    //void gadf_register_info_opus_enc_48k2ch(); gadf_register_info_opus_enc_48k2ch();
    void gadf_register_info_opus_dec_16k1ch(); gadf_register_info_opus_dec_16k1ch();
    //void gadf_register_info_opus_dec_16k2ch(); gadf_register_info_opus_dec_16k2ch();
    //void gadf_register_info_opus_dec_48k1ch(); gadf_register_info_opus_dec_48k1ch();
    //void gadf_register_info_opus_dec_48k2ch(); gadf_register_info_opus_dec_48k2ch();

    while (1) {
        void* info = gadf_get_register_info();
        if (!info) {
            break;
        }
        gadf_run_by_info(info);
    }

}
EXTERNC void OpusCodecTest() {
    OpusCodecTest0();
}