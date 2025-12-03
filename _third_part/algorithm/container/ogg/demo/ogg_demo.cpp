#include <string.h>
#include <stdarg.h>
#include <stdint.h>
#include <stdio.h>
#include "gadf_porting_api.h"
#include "ogg_api.h"

//api demo
#define LOG_OGG_ORI(fmt,...)    GadfPrint(fmt, ##__VA_ARGS__)
#define LOG_OGG(fmt,...)        GadfPrint("(%s)[%d]" fmt, __func__, __LINE__, ##__VA_ARGS__)

static char heap_pool[80 * 1024];
static void* ogg_heap = 0;


static void* ogg_malloc(uint32_t size)
{
    void* buf = GadfHeapMalloc(ogg_heap, size);
    LOG_OGG("%d,%p", size, buf);
    return buf;
}

static void* ogg_realloc(void* rmem, uint32_t newsize)
{
    void* buf = GadfHheapRealloc(ogg_heap, rmem, newsize);
    LOG_OGG("%d,%p,%p", newsize, rmem, buf);
    return buf;
}

static void ogg_free(void* rmem)
{
    LOG_OGG("%p", rmem);
    GadfHeapFree(ogg_heap, rmem);
    return;
}

static void ogg_print(const char* fmt, ...)
{
    static char buf[256];
    va_list ap;
    va_start(ap, fmt);
    vsprintf(buf, fmt, ap);
    va_end(ap);
    LOG_OGG_ORI("%s", buf);
}


static uint32_t _frame_sample = 0;
static uint32_t _frame_sample_acc = 0;
void* ogg_demo_muxer_init(uint32_t rate, uint16_t channels, uint8_t width, uint32_t frameDMs) {
    if (!ogg_heap) {
        ogg_heap = GadfHheapRegister(heap_pool, sizeof(heap_pool));
    }
    void* _oggMuxer = 0;
    OggMuxerApiParam_t oggMuxerApiParam;
    oggMuxerApiParam.malloc_cb = ogg_malloc;
    oggMuxerApiParam.realloc_cb = ogg_realloc;
    oggMuxerApiParam.free_cb = ogg_free;
    oggMuxerApiParam.printf_cb = 0;
    oggMuxerApiParam.mode = OggMuxerApiMode_e::OGG_MUXER_API_MODE_OPUS;
    oggMuxerApiParam.idParam.version = 1;
    oggMuxerApiParam.idParam.channel = channels;
    oggMuxerApiParam.idParam.preSkip = 0;
    oggMuxerApiParam.idParam.sampleRate = rate;
    oggMuxerApiParam.idParam.outPutGain = 0;
    const char* vendorString = "Lavf60.16.100";
    const char* userComment = "encoder=Lavc60.31.102 libopus";
    oggMuxerApiParam.userComment.vendorString = (uint8_t*)vendorString;
    oggMuxerApiParam.userComment.vendorStringLen = strlen(vendorString);
    oggMuxerApiParam.userComment.userCommentString = (uint8_t*)userComment;
    oggMuxerApiParam.userComment.userCommentStringLen = strlen(userComment);
    oggMuxerApiParam.page_byte_round = 4096;
    OggRet_t ret;
    ret = ogg_muxer_api_create(&oggMuxerApiParam, &_oggMuxer);
    if (ret != OGG_RET_SUCCESS) {channels, LOG_OGG("opus set fail, %d", ret); return 0; }
    //_frame_sample = frameDMs * rate / 10000;
    _frame_sample = frameDMs * 48 / 10;
    _frame_sample_acc = 0;
    return _oggMuxer;
}

bool ogg_demo_muxer_run(void* ogg, uint8_t* data, int32_t *dataByte, uint8_t* oggMuxedData, int32_t* oggMuxedDataByte, bool isEos) {
    uint8_t* iBuf = data;
    int32_t iSize = *dataByte;
    uint8_t* oBuf = oggMuxedData;
    int32_t oSize = *oggMuxedDataByte;
    *oggMuxedDataByte = 0;
    *dataByte = iSize;
    if (isEos) {
        ogg_muxer_api_set(ogg, OggMuxerApiSet_e::OGG_MUXER_API_SET_IS_EOS, (void*)1);
    }
    OggRet_t ret = OGG_RET_SUCCESS;
    if (iSize) {
        _frame_sample_acc += _frame_sample;
        {
            static uint32_t iSizeAcc = 0;
            static uint32_t ms = 0;
            iSizeAcc += iSize;
            ms += 20;
            LOG_OGG("%d,%d,%dms", _frame_sample_acc,iSizeAcc, ms);
        }
        ogg_muxer_api_set(ogg, OggMuxerApiSet_e::OGG_MUXER_API_SET_GRANULEPOS, (void*)(uint32_t)_frame_sample_acc);
        ret = ogg_muxer_api_receive(ogg, iBuf, iSize);
        if (ret != OGG_RET_SUCCESS) return false;
    }
    OggPage_t oggPage;
    ret = ogg_muxer_api_generate(ogg, &oggPage);
    if (ret == OGG_RET_SUCCESS) {
        if (oSize < oggPage.headLen + oggPage.bodyLen) {
            LOG_OGG("generate fail %d,%d", oSize, oggPage.headLen + oggPage.bodyLen);
            return false;
        }
        LOG_OGG("generate %d", oggPage.headLen + oggPage.bodyLen);
        memcpy(oBuf, oggPage.headData, oggPage.headLen);
        memcpy(oBuf + oggPage.headLen, oggPage.bodyData, oggPage.bodyLen);
        *oggMuxedDataByte = oggPage.headLen + oggPage.bodyLen;
    }
    return true;
}

bool ogg_demo_muxer_deinit(void* ogg) {
    if(ogg) ogg_muxer_api_destory(ogg);
    return true;
}



//for test
#include "gadf.h"
static void OggDemoMuxTest0() {
    void gadf_register_info_opus_16k2ch_ogg_mux(); gadf_register_info_opus_16k2ch_ogg_mux();
    while (1) {
        void* info = gadf_get_register_info();
        if (!info) {
            break;
        }
        gadf_run_by_info(info);
    }

}
EXTERNC void OggDemoMuxTest() {
    OggDemoMuxTest0();
}