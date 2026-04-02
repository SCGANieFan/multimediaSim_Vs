/***************************************************************************
 *
 * Copyright 2015-2024 BES.
 * All rights reserved. All unpublished rights reserved.
 *
 * No part of this work may be used or reproduced in any form or by any
 * means, or stored in a database or retrieval system, without prior written
 * permission of BES.
 *
 * Use of this work is governed by a license granted by BES.
 * This work contains confidential and proprietary information of
 * BES. which is protected by copyright, trade secret,
 * trademark and other intellectual property rights.
 *
 ****************************************************************************/
#if 1
#include <stdint.h>
typedef void* multi_heap_handle_t;
multi_heap_handle_t heap_register(char *heap_pool, int size);
void* heap_malloc(multi_heap_handle_t heap, int size);
void* heap_realloc(multi_heap_handle_t heap, void* rmem, int newsize);
void heap_free(multi_heap_handle_t heap, void* rmem);
#define TRACE 
#endif

#if 0
#include <string.h>
#include "hal_trace.h"
#include "heap_api.h"
#include "opus_api.h"

#define LOG(fmt,...) TRACE(0, "(%s)[%d]" fmt, __func__, __LINE__, ##__VA_ARGS__)

#define RATE (16000)
#define CHANNEL (1)
#define WIDTH (2)
#define BITRATE (16000)
#define FRAME_0P1MS (200)
#define PCM_MS (360)

#define FRAME_NUM (PCM_MS*10/FRAME_0P1MS)
#define FRAME_SAMPLE (RATE*FRAME_0P1MS/10000)
#define FRAME_SIZE (FRAME_SAMPLE*CHANNEL)
#define FRAME_BYTE (FRAME_SIZE*WIDTH)

#define PCM_SAMPLE (FRAME_NUM*FRAME_SAMPLE)
#define PCM_SIZE (PCM_SAMPLE*CHANNEL)
#define PCM_BYTE (PCM_SIZE*WIDTH)

#define ENCODED_FRAME_BYTE (BITRATE*FRAME_0P1MS/80000)
#define ENCODED_BYTE (FRAME_NUM*ENCODED_FRAME_BYTE)

static uint8_t pcm_in_ori[FRAME_BYTE] = {};
static uint8_t encoded_out[FRAME_BYTE] = {};

static uint8_t encoded_in_ori[ENCODED_BYTE] = {};
static uint8_t pcm_out[FRAME_BYTE] = {};

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
    heap_free(heap,rmem);
    LOG("%p", rmem);
    return;
}

static void opus_print(const char* buf, int len)
{
    TRACE(0, "%s", buf);
}

bool OpusEncTest()
{
    void *enc = 0;
    const int rate = RATE;
    const int channels = CHANNEL;
    const int bitrate=BITRATE;
    const int frameDMs=FRAME_0P1MS;
    const int complexity=0;
    bool haveHead=false;
    OpusApi_CreateEncParam_t param;
    param.basePort.malloc_cb = opus_malloc;
    param.basePort.realloc_cb = opus_realloc;
    param.basePort.free_cb = opus_free;
    param.basePort.print_cb = opus_print;
    LOG("(%d,%d,%d,%d)", rate, channels, bitrate, frameDMs);
    OpusApiRet_t ret = opus_api_create_encoder(&enc, &param);
    if (ret != OPUS_API_RET_SUCCESS) {
        LOG("opus create fail, %d,(%p,%d,%d,%d)", ret, enc, rate, channels, haveHead);
        return false;
    }
    ret |= opus_api_encoder_set(enc, "fs", (void*)rate);
    ret |= opus_api_encoder_set(enc, "ch", (void*)(uint32_t)channels);
    ret |= opus_api_encoder_set(enc, "bitrate", (void*)bitrate);
    ret |= opus_api_encoder_set(enc, "f0p1ms", (void*)frameDMs);
    ret |= opus_api_encoder_set(enc, "vbr", (void*)0);
    ret |= opus_api_encoder_set(enc, "cpx", (void*)complexity);
    ret |= opus_api_encoder_set(enc, "encmode", (void*)1002); //range (-1000,1000,1001,1002), each means AUTO,SILK_ONLY,HYBRID,CELT_ONLY.
    ret |= opus_api_encoder_set(enc, "app", (void*)2049); //range (2048,2049,2051), each means VOIP,AUDIO,RESTRICTED_LOWDELAY.
    ret |= opus_api_encoder_set(enc, "stk48k1ch", (void*)(40 * 1024));
    if (ret != OPUS_API_RET_SUCCESS) { LOG("opus set fail, %d", ret); return false; }
    
    ret = opus_api_open_encoder(enc);
    if (ret != OPUS_API_RET_SUCCESS) { LOG("opus open fail, %d", ret); return false; }

    for(int f=0;f<FRAME_NUM;f++){
        unsigned char* iBuff = (unsigned char*)(pcm_in_ori+f*FRAME_BYTE);
        int iByte = FRAME_BYTE;
        unsigned char* oBuff = encoded_out+f*ENCODED_FRAME_BYTE;
        int oByte = ENCODED_FRAME_BYTE;
        ret = opus_api_encoder_run(enc, iBuff, &iByte, oBuff, &oByte);
        if(ret != OPUS_API_RET_SUCCESS){
            LOG("opus run fail, %d", ret);
            return false;
        }
    }
    opus_api_close_encoder(enc);
    opus_api_destory_encoder(enc);
    LOG("opus encode success");
    return true;
}

bool OpusDecTest()
{
    static float duration = FRAME_0P1MS/10;//ms
    void *dec = 0;
    const int rate = RATE;
    const int channels = CHANNEL;
    const int bitrate=BITRATE;
    const int frameDMs=FRAME_0P1MS;
    const int complexity=0;
    bool haveHead=false;

#if 1
    LOG("(%d,%d,%d,%d)", rate, channels, bitrate, frameDMs);
    OpusApi_CreateDecParam_t param;
    param.basePort.malloc_cb = opus_malloc;
    param.basePort.realloc_cb = opus_realloc;
    param.basePort.free_cb = opus_free;
    param.basePort.print_cb = opus_print;
    OpusApiRet_t ret = opus_api_create_decoder(&dec, &param);
    if (ret != OPUS_API_RET_SUCCESS) {
        LOG("Cannot create decoder: %d\n", ret);
        return false;
    }
    ret |= opus_api_decoder_set(dec, "fs", (void*)rate);
    ret |= opus_api_decoder_set(dec, "ch", (void*)(uint32_t)channels);
    if (ret != OPUS_API_RET_SUCCESS) {
        LOG("set fail"); return false;
    }
    ret = opus_api_open_decoder(dec);
    if (ret != OPUS_API_RET_SUCCESS) {
        LOG("open fail"); return false;
    }

#endif
    for(int f=0;f<FRAME_NUM;f++){
        unsigned char* encodedOneFrame = (uint8_t*)(encoded_in_ori+f*ENCODED_FRAME_BYTE);
        int encodedOneFrameByte = ENCODED_FRAME_BYTE;
        unsigned char* decodecPcm = (unsigned char*)pcm_out;
        int decodecPcmByte = FRAME_BYTE;
        bool isPlc=false;
        ret = opus_api_decoder_run(dec, encodedOneFrame, encodedOneFrameByte, decodecPcm, &decodecPcmByte, false);
        if(ret != OPUS_API_RET_SUCCESS){
            LOG("opus run fail, %d", ret);
            return false;
        }
    }
    opus_api_close_decoder(dec);
    opus_api_destory_decoder(dec);
    LOG("opus decode success");
    return true;
}

void OpusCodecTest()
{
    heap = heap_register(heap_pool,sizeof(heap_pool));
    OpusDecTest();
    OpusEncTest();
}


#endif