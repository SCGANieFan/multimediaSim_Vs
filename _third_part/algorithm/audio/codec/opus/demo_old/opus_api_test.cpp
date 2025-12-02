
#include <string.h>
#include <stdarg.h>
#include <stdint.h>
#include <stdio.h>
#include "hal_trace.h"
#include "heap_api.h"
#include "opus_api.h"
#include "opus_api_test_info.h"

#define LOG_ORI(fmt,...)    TRACE(0, fmt, ##__VA_ARGS__)
#define LOG(fmt,...)        TRACE(0, "(%s)[%d]" fmt, __func__, __LINE__, ##__VA_ARGS__)

#ifdef  WIN32
#define OPUS_TEST_FILE_PATH "./"
void* OpusTestFileOpen(const char*url, const char* mode) {
    return fopen(url, mode);
}
uint32_t OpusTestFileWrite(void* hd, void* buf, uint32_t size) {
    return fwrite(buf, 1, size, (FILE*)hd);
}
void OpusTestFileClose(void* hd) {
    fclose((FILE*)hd);
}
#else
#define OPUS_TEST_FILE_PATH "data/emmc0/"
void* OpusTestFileOpen(const char* url, const char* mode) {
    return 1;
}
uint32_t OpusTestFileWrite(void* hd, void* buf, uint32_t size) {
    return size;
}
void OpusTestFileClose(void* hd) {
}
#endif



static const char* ret2str[] = {
    "success",
    "fail",
    "not support",
};

static uint8_t tmpBuf[10 * 1024];
static char heap_pool[80*1024];
static multi_heap_handle_t heap;

static void* opus_malloc(int size)
{
    void*buf = heap_malloc(heap,size);
    LOG("%d,%p", size,buf);
    return buf;
}

static void* opus_realloc(void* rmem, int newsize)
{
    void *buf = heap_realloc(heap,rmem,newsize);
    LOG("%d,%p,%p", newsize,rmem,buf);
    return buf;
}

static void opus_free(void* rmem)
{
    LOG("%p", rmem);
    heap_free(heap,rmem);
    return;
}

void opus_print(const char* fmt, ...)
{
    static char buf[256];
    va_list ap;
    va_start(ap, fmt);
    vsprintf(buf, fmt, ap);
    va_end(ap);
    LOG_ORI("%s", buf);
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

bool OpusEncTest(EncInfo_t *encInfo)
{
    LOG("start, %s-%s", encInfo->type, encInfo->key);
    void* fOut = 0;
    if (encInfo->is_save_encoded_out_file) {
        char fileName[128];
        sprintf(fileName, "%s%s_%s.opus", OPUS_TEST_FILE_PATH, encInfo->type, encInfo->key);
        LOG("fopen %s", fileName);
        fOut = OpusTestFileOpen(fileName, "wb");
        if (!fOut) {
            LOG("file open fail"); return false;
        }
    }
    void *enc = 0;
    uint32_t rate = encInfo->rate;
    uint16_t channels = encInfo->ch;
    uint16_t width = encInfo->width;
    uint32_t bitrate = encInfo->bitrate;
    uint32_t frameDMs = encInfo->frame0p1Ms;
    LOG("(%p,%u),(%p,%u),(%u,%u,%u),(%u,%u)", 
        encInfo->pcm, encInfo->pcmByte,
        encInfo->out, encInfo->outByte,
        rate, channels, width, 
        bitrate, frameDMs);
    if (channels > 2 
        || width != 2
        ||!encInfo->pcm
        || !encInfo->out) {
        return false; 
    }
    uint16_t pcmFrameSample = rate * frameDMs / 10000;
    uint16_t pcmFrameByte = pcmFrameSample * channels * width;
    uint16_t frameNum = encInfo->pcmByte / pcmFrameByte;
    uint16_t encodedFrameByte = (bitrate / 8) * encInfo->frame0p1Ms / 10000;
    LOG("frameNm:%u=%u/%u,%u", frameNum, encInfo->pcmByte, pcmFrameByte, encodedFrameByte);

    bool haveHead=true;
    OpusApi_CreateEncParam_t encParam;
    memset(&encParam, 0, sizeof(OpusApi_CreateEncParam_t));
    encParam.basePort.malloc_cb = opus_malloc;
    encParam.basePort.realloc_cb = opus_realloc;
    encParam.basePort.free_cb = opus_free;
    encParam.basePort.print_cb = opus_print;
    OpusApiRet_t ret = opus_api_create_encoder(&enc, &encParam);
    if (ret != OPUS_API_RET_SUCCESS) {
        LOG("opus create fail, %s,(%p,%d,%d,%d)", ret2str[-ret], enc, rate, channels, haveHead);
        return false;
    }

    ret |= opus_api_encoder_set(enc, opusEncEnum2Choose[OPUS_ENC_SET_BIT_RATE], (void*)bitrate);
    ret |= opus_api_encoder_set(enc, opusEncEnum2Choose[OPUS_ENC_SET_FRAME_DURATION_0P1MS], (void*)frameDMs);
    ret |= opus_api_encoder_set(enc, opusEncEnum2Choose[OPUS_ENC_SET_ENC_CHOOSE], (void*)OPUS_API_ENC_CHOOSE_NORMAL);
    ret |= opus_api_encoder_set(enc, opusEncEnum2Choose[OPUS_ENC_SET_SAMPLE_RATE], (void*)rate);
    ret |= opus_api_encoder_set(enc, opusEncEnum2Choose[OPUS_ENC_SET_CHANNELS], (void*)channels);
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


    if (ret != OPUS_API_RET_SUCCESS) { LOG("opus set fail, %d", ret); return false; }
    ret = opus_api_open_encoder(enc);
    if (ret != OPUS_API_RET_SUCCESS) { LOG("opus open fail, %d", ret); return false; }

    uint32_t f = 0;
    for (f = 0; f < frameNum; f++) {
        if(encInfo->is_save_encoded_out_array){
            if(encInfo->outByte < (f + 1) * encodedFrameByte){
                break;
            }
        }
        if(f%10==0){LOG("[%u] running",f);}
        int16_t* iBuff = (int16_t*)(encInfo->pcm + f * pcmFrameByte);
        int32_t iSample = pcmFrameSample;
        uint8_t* oBuff = 0;
        int32_t oByte = 0;
        oBuff = tmpBuf;
        oByte = sizeof(tmpBuf);
        if(encInfo->is_save_encoded_out){
            if(encInfo->is_save_encoded_out_array) {
                oBuff = encInfo->out + f * encodedFrameByte;
                oByte = encInfo->outByte - f * encodedFrameByte;
            }
        }
        ret = opus_api_encoder_run(enc, iBuff, iSample, oBuff, &oByte);
        if (ret != OPUS_API_RET_SUCCESS) { LOG("opus run fail, %s", ret2str[-ret]); return false; }
        if (encInfo->is_save_encoded_out_file) {
            OpusTestFileWrite(fOut, oBuff, oByte);
        }
    }
    opus_api_close_encoder(enc);
    opus_api_destory_encoder(enc);
    if (encInfo->is_save_encoded_out_file) {
        OpusTestFileClose(fOut);
    }
    LOG("encoded frameNum:%d", f);
    LOG("end");
    return true;
}

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

bool OpusDecTest(DecInfo_t *decInfo)
{
    LOG("start, %s-%s", decInfo->type, decInfo->key);
    void* fOut = 0;
    if (decInfo->is_save_pcm_out) {
        char fileName[128];
        sprintf(fileName, "%s%s_%s.opus", OPUS_TEST_FILE_PATH, decInfo->type, decInfo->key);
        fOut = OpusTestFileOpen(fileName, "wb");
        if (!fOut) {
            LOG("file open fail"); return false;
        }
    }
    void *dec = 0;
    int rate = decInfo->rate;
    int channels = decInfo->ch;
    int width = 2;
    LOG("%d,%d,(%p,%u),(%p,%u)",
        rate, channels,
        decInfo->encoedData, decInfo->encoedDataByte,
        decInfo->pcm, decInfo->pcmByte);
    if (channels > 2
        || !decInfo->encoedData
        || !decInfo->pcm) { return false; }
    decInfo->width = width;


    OpusApi_CreateDecParam_t param;
    memset(&param, 0, sizeof(OpusApi_CreateDecParam_t));
    param.basePort.malloc_cb = opus_malloc;
    param.basePort.realloc_cb = opus_realloc;
    param.basePort.free_cb = opus_free;
    param.basePort.print_cb = opus_print;
    OpusApiRet_t ret = opus_api_create_decoder(&dec, &param);
    if (ret != OPUS_API_RET_SUCCESS) {
        LOG("create fail, %s,(%p,%d,%d)", ret2str[-ret], dec, rate, channels);
        return false;
    }
    ret |= opus_api_decoder_set(dec, opusDecEnum2Choose[OPUS_DEC_SET_DEC_CHOOSE], (void*)OpusApi_DecChoose_e::OPUS_API_DEC_CHOOSE_NORMAL);
    ret |= opus_api_decoder_set(dec, opusDecEnum2Choose[OPUS_DEC_SET_SAMPLE_RATE],(void*)rate);
    ret |= opus_api_decoder_set(dec, opusDecEnum2Choose[OPUS_DEC_SET_CHANNELS],(void*)channels);
    if (ret != OPUS_API_RET_SUCCESS) {
        LOG("set fail"); return false;
    }
    ret = opus_api_open_decoder(dec);
    if (ret != OPUS_API_RET_SUCCESS) {
        LOG("open fail"); return false;
    }
    uint32_t encoedDataUsedByte = 0;
    uint32_t pcmOutByte = 0;
    uint32_t frameNum = 0;
    while ((encoedDataUsedByte + 8) < decInfo->encoedDataByte){
        if (frameNum % 10 == 0) { LOG("[%u] running", frameNum); }
        uint8_t* iHead = (uint8_t*)(decInfo->encoedData + encoedDataUsedByte);
        uint32_t iByte = ((uint32_t)iHead[0] << 24)| ((uint32_t)iHead[1] << 16)| ((uint32_t)iHead[2] << 8)| (uint32_t)iHead[3];
        if (decInfo->encoedDataByte - encoedDataUsedByte < iByte + 8) {
            break;
        }
        uint8_t* iBuff = (uint8_t*)(iHead + 8);
        short* oBuff = 0;
        if (decInfo->is_save_pcm_out) {
#if 0
            oBuff = (short*)(decInfo->pcm + pcmOutByte);
#else
            oBuff = (short*)(tmpBuf);
#endif
        }
        else {
            oBuff = (short*)(tmpBuf);
        }
        int32_t oSample = sizeof(tmpBuf);
        ret = opus_api_decoder_run(dec, iBuff, iByte, oBuff, &oSample, false);
        if (ret != OPUS_API_RET_SUCCESS) { LOG("opus run fail, %s", ret2str[ret]); return false; }
        encoedDataUsedByte += iByte + 8;
        if (decInfo->is_save_pcm_out) {
            OpusTestFileWrite(fOut, oBuff, oSample);
            pcmOutByte += oSample * width * channels;
#if 0
            if (pcmOutByte > decInfo->pcmByte) { LOG("out pcm buffer size is not enough,%d,%d", pcmOutByte, decInfo->pcmByte); return false; }
#endif
        }
        frameNum++;
    }
    opus_api_close_decoder(dec);
    opus_api_destory_decoder(dec);
    OpusTestFileClose(fOut);
    LOG("frameNum:%d", frameNum);
    LOG("end");
    return true;
}

void OpusCodecTest()
{
#if M_OPUS_TEST_ENC
    //extern void register_opus_test_info_enc_16k1ch(); register_opus_test_info_enc_16k1ch();
    //extern void register_opus_test_info_enc_16k2ch(); register_opus_test_info_enc_16k2ch();
    //extern void register_opus_test_info_enc_48k1ch(); register_opus_test_info_enc_48k1ch();
    //extern void register_opus_test_info_enc_48k2ch(); register_opus_test_info_enc_48k2ch();
    OpusEncoderNormalRegister();
#endif
#if M_OPUS_TEST_DEC
    //extern void register_opus_test_info_dec_16k1ch(); register_opus_test_info_dec_16k1ch();
    //extern void register_opus_test_info_dec_16k2ch(); register_opus_test_info_dec_16k2ch();
    //extern void register_opus_test_info_dec_48k1ch(); register_opus_test_info_dec_48k1ch();
    //extern void register_opus_test_info_dec_48k2ch(); register_opus_test_info_dec_48k2ch();
    OpusDecoderNormalRegister();
#endif
    heap = heap_register(heap_pool,sizeof(heap_pool));
    while (1) {
        EncInfo_t *info;
        get_opus_test_info("enc", &info);
        if (!info)break;
        OpusEncTest(info);
    }
    while (1) {
        DecInfo_t* info;
        get_opus_test_info("dec", &info);
        if (!info)break;
        OpusDecTest(info);
    }
}
