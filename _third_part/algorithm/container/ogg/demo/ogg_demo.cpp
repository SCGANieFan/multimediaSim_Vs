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
#if 0
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
#endif
    uint32_t id = 0;
    OggApiRet_t ret = OGG_API_RET_SUCCESS;
    OggApiBasePort_t bp;
    bp.malloc_cb = ogg_malloc;
    bp.realloc_cb = ogg_realloc;
    bp.free_cb = ogg_free;
    bp.printf_cb = 0;
    id = ogg_api_muxer_create(&bp);
    if (!id) {LOG_OGG("opus set fail"); return 0; }

    ret |= ogg_api_muxer_set(id, "mode", (void*)"opus");
    ret |= ogg_api_muxer_set(id, "version", (void*)(uint32_t)1);
    ret |= ogg_api_muxer_set(id, "ch", (void*)(uint32_t)channels);
    ret |= ogg_api_muxer_set(id, "preSkip", (void*)(uint32_t)0);
    ret |= ogg_api_muxer_set(id, "fs", (void*)(uint32_t)rate);
    ret |= ogg_api_muxer_set(id, "oGain", (void*)(uint32_t)0);
    ret |= ogg_api_muxer_set(id, "vendor", (void*)(uint32_t)"Lavf60.16.100");
    ret |= ogg_api_muxer_set(id, "comment", (void*)(uint32_t)"encoder=Lavc60.31.102 libopus");
    ret |= ogg_api_muxer_set(id, "pageByte", (void*)(uint32_t)4096);
    if (ret != OGG_API_RET_SUCCESS) { LOG_OGG("ogg set fail, %d", ret); return 0; }

    ret = ogg_api_muxer_open(id);
    if (ret != OGG_API_RET_SUCCESS) { LOG_OGG("ogg open fail, %d", ret); return 0; }

    //_frame_sample = frameDMs * rate / 10000;
    _frame_sample = frameDMs * 48 / 10;
    _frame_sample_acc = 0;
    return (void*)id;
}

bool ogg_demo_muxer_run(void* ogg, uint8_t* data, int32_t *dataByte, uint8_t* oggMuxedData, int32_t* oggMuxedDataByte, bool isEos) {
    uint8_t* iBuf = data;
    int32_t iSize = *dataByte;
    uint8_t* oBuf = oggMuxedData;
    int32_t oSize = *oggMuxedDataByte;
    *oggMuxedDataByte = 0;
    *dataByte = iSize;
    if (isEos) {
        ogg_api_muxer_set((uint32_t)ogg, "eos", (void*)1);
    }
    OggApiRet_t ret = OGG_API_RET_SUCCESS;
    if (iSize) {
        _frame_sample_acc += _frame_sample;
        {
            static uint32_t iSizeAcc = 0;
            static uint32_t ms = 0;
            iSizeAcc += iSize;
            ms += 20;
            LOG_OGG("%d,%d,%dms", _frame_sample_acc,iSizeAcc, ms);
        }
        ogg_api_muxer_set((uint32_t)ogg, "gPos", (void*)(uint32_t)_frame_sample_acc);
        ret = ogg_api_muxer_receive((uint32_t)ogg, iBuf, &iSize);
        if (ret != OGG_API_RET_SUCCESS) return false;
    }
#if 0
    OggPage_t oggPage;
    ret = ogg_api_muxer_generate((uint32_t)ogg, &oggPage);
    if (ret == OGG_API_RET_SUCCESS) {
        if (oSize < oggPage.headLen + oggPage.bodyLen) {
            LOG_OGG("generate fail %d,%d", oSize, oggPage.headLen + oggPage.bodyLen);
            return false;
        }
        LOG_OGG("generate %d", oggPage.headLen + oggPage.bodyLen);
        memcpy(oBuf, oggPage.headData, oggPage.headLen);
        memcpy(oBuf + oggPage.headLen, oggPage.bodyData, oggPage.bodyLen);
        *oggMuxedDataByte = oggPage.headLen + oggPage.bodyLen;
    }
#else
    ret = ogg_api_muxer_generate((uint32_t)ogg, oBuf, &oSize);
    *oggMuxedDataByte = oSize;
#endif
    return true;
}

bool ogg_demo_muxer_deinit(void* ogg) {
    ogg_api_muxer_close((uint32_t)ogg);
    ogg_api_muxer_destory((uint32_t)ogg);
    return true;
}



//for test
#include "gadf.h"
static void OggDemoMuxTest0() {
    ogg_api_register_ogg_muxer();

    void gadf_register_info_opus_16k2ch_ogg_mux(); gadf_register_info_opus_16k2ch_ogg_mux();
    while (1) {
        void* info = gadf_get_register_info();
        if (!info) {
            break;
        }
        gadf_run_by_info(info);
    }

}
static bool t0 = false;
static bool t1 = false;
extern "C" void gaapi_init();
static void OggDemoMuxTest1() {
    gaapi_init();
    GadfThreadStart("reg0", 0,[](void*) {
        t0 = true;
        while (!t1);
        ogg_api_register_ogg_muxer();
        }, 0, 0);
    GadfThreadStart("reg1", 0, [](void*) {
        t1 = true;
        while (!t0);
        ogg_api_register_ogg_demuxer();
        }, 0, 0);
}

EXTERNC void OggDemoMuxTest() {
    OggDemoMuxTest0();
    //OggDemoMuxTest1();
}