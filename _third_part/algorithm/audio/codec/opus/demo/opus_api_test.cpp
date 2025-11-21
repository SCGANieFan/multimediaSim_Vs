
#include <string.h>
#include <stdarg.h>
#include <stdint.h>
#include <stdio.h>
#include "hal_trace.h"
#include "heap_api.h"
#include "opus_api.h"
#include "opus_api_test_info.h"

#define LOG_ORI(fmt,...) TRACE(0, fmt, ##__VA_ARGS__)
#define LOG(fmt,...) TRACE(0, "(%s)[%d]" fmt, __func__, __LINE__, ##__VA_ARGS__)

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
    int a = 1;
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

bool OpusEncTest(EncInfo_t *encInfo)
{
    LOG("start, %s-%s", encInfo->type, encInfo->key);
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

    bool haveHead=false;
    OpusApi_BasePort_t opusApiBasePort;
    opusApiBasePort.malloc_cb = opus_malloc;
    opusApiBasePort.realloc_cb = opus_realloc;
    opusApiBasePort.free_cb = opus_free;
    opusApiBasePort.print_cb = opus_print;
    OpusApiRet_t ret = opus_api_create_encoder(&enc, &opusApiBasePort, rate, channels, haveHead, OPUS_API_ENC_CHOOSE_NORMAL);
    if (ret != OPUS_API_RET_SUCCESS) {
        LOG("opus create fail, %s,(%p,%d,%d,%d)", ret2str[-ret], enc, rate, channels, haveHead);
        return false;
    }
    ret = opus_api_encoder_set(enc, OPUS_API_ENC_SET_BIT_RATE, (void*)bitrate);
    if (ret != OPUS_API_RET_SUCCESS) { LOG("opus set fail, %d", ret); return false; }
    ret = opus_api_encoder_set(enc, OPUS_API_ENC_SET_FRAME_DURATION_0P1MS, (void*)frameDMs);
    if (ret != OPUS_API_RET_SUCCESS) { LOG("opus set fail, %d", ret); return false; }
    ret = opus_api_encoder_set(enc, OPUS_API_ENC_SET_USE_VBR, (void*)false);
    if (ret != OPUS_API_RET_SUCCESS) { LOG("opus set fail, %d", ret); return false; }
    uint32_t f = 0;
    for (f = 0; f < frameNum; f++) {
        if(encInfo->is_save_encoded_out){
            if(encInfo->outByte < (f + 1) * encodedFrameByte){
                break;
            }
        }
        if(f%10==0){LOG("[%u] running",f);}
        int16_t* iBuff = (int16_t*)(encInfo->pcm + f * pcmFrameByte);
        int32_t iSample = pcmFrameSample;
        uint8_t* oBuff = 0;
        int32_t oByte = 0;
        if(encInfo->is_save_encoded_out){
            oBuff = encInfo->out + f * encodedFrameByte;
            oByte = encInfo->outByte - f * encodedFrameByte;
        }else{
            oBuff = tmpBuf;
            oByte = sizeof(tmpBuf);
        }
        ret = opus_api_encoder_run(enc, iBuff, iSample, oBuff, &oByte);
        if (ret != OPUS_API_RET_SUCCESS) { LOG("opus run fail, %s", ret2str[-ret]); return false; }
    }
    opus_api_destory_encoder(enc);
    LOG("encoded frameNum:%d", f);
    LOG("end");
    return true;
}

bool OpusDecTest(DecInfo_t *decInfo)
{
    LOG("start, %s-%s", decInfo->type, decInfo->key);
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

    OpusApi_BasePort_t opusApiBasePort;
    opusApiBasePort.malloc_cb = opus_malloc;
    opusApiBasePort.realloc_cb = opus_realloc;
    opusApiBasePort.free_cb = opus_free;
    opusApiBasePort.print_cb = opus_print;
    OpusApiRet_t ret = opus_api_create_decoder(&dec, &opusApiBasePort, rate, channels, OPUS_API_DEC_CHOOSE_NORMAL);
    if (ret != OPUS_API_RET_SUCCESS) {
        LOG("create fail, %s,(%p,%d,%d)", ret2str[-ret], dec, rate, channels);
        return false;
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
            oBuff = (short*)(decInfo->pcm + pcmOutByte);
        }
        else {
            oBuff = (short*)(tmpBuf);
        }
        int32_t oSample = sizeof(tmpBuf);
        ret = opus_api_decoder_run(dec, iBuff, iByte, oBuff, &oSample, false);
        if (ret != OPUS_API_RET_SUCCESS) { LOG("opus run fail, %s", ret2str[ret]); return false; }
        encoedDataUsedByte += iByte + 8;
        if (decInfo->is_save_pcm_out) {
            pcmOutByte += oSample * width * channels;
            if (pcmOutByte > decInfo->pcmByte) { LOG("out pcm buffer size is not enough,%d,%d", pcmOutByte, decInfo->pcmByte); return false; }
        }
        frameNum++;
    }
    opus_api_destory_decoder(dec);
    LOG("frameNum:%d", frameNum);
    LOG("end");
    return true;
}

void OpusCodecTest()
{
#if M_OPUS_TEST_ENC
    extern void register_opus_test_info_enc_16k1ch(); register_opus_test_info_enc_16k1ch();
    extern void register_opus_test_info_enc_16k2ch(); register_opus_test_info_enc_16k2ch();
    extern void register_opus_test_info_enc_48k1ch(); register_opus_test_info_enc_48k1ch();
    extern void register_opus_test_info_enc_48k2ch(); register_opus_test_info_enc_48k2ch();
    OpusEncoderNormalRegister();
#endif
#if M_OPUS_TEST_DEC
    extern void register_opus_test_info_dec_16k1ch(); register_opus_test_info_dec_16k1ch();
    extern void register_opus_test_info_dec_16k2ch(); register_opus_test_info_dec_16k2ch();
    extern void register_opus_test_info_dec_48k1ch(); register_opus_test_info_dec_48k1ch();
    extern void register_opus_test_info_dec_48k2ch(); register_opus_test_info_dec_48k2ch();
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
