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
#pragma once
#include <stddef.h>

#ifndef EXTERNC
#ifdef __cplusplus
#define EXTERNC extern "C" 
#else
#define EXTERNC
#endif
#endif


#define OPUS_API_RET_SUCCESS        (0)
#define OPUS_API_RET_FAIL           (-1)
#define OPUS_API_RET_NOT_SUPPORT    (-2)

typedef int OpusApiRet_t;                               //opus api function return value, range(OPUS_API_RET_SUCCESS, OPUS_API_RET_FAIL, OPUS_API_RET_NOT_SUPPORT)

typedef struct {
    void* (*malloc_cb)(int size);                       //dynamic memory alloc function pointer, it can not be null
    void* (*realloc_cb)(void* rmem, int newsize);       //dynamic memory realloc function pointer, it can not be null
    void(*free_cb)(void* buf);                          //dynamic memory free function pointer, it can not be null
    void(*print_cb)(const char* buf, int len);          //log print function pointer, if donot want print, it can be null
}OpusApi_BasePort_t;


typedef struct {
    OpusApi_BasePort_t basePort;
}OpusApi_CreateEncParam_t;

typedef struct {
    OpusApi_BasePort_t basePort;
}OpusApi_CreateDecParam_t;


//enc
EXTERNC OpusApiRet_t opus_api_create_ms_encoder(void** pHd, OpusApi_CreateEncParam_t *param);
EXTERNC OpusApiRet_t opus_api_create_encoder(void** pHd, OpusApi_CreateEncParam_t *param);
EXTERNC OpusApiRet_t opus_api_open_encoder(void* hd);

#if 0
    "bitrate",              //bits per second after encoded, range (6k --> 510k), default 12k
    "f0p1ms",               //0.1ms, if frame is 20ms, this value should be 200. range (25,50,100,200,400,600), default 200
    "vbr",                  //if 1, use variable bitrate, default 0//recommend 0, range (0 --> 9), default 0
    "cpx",                  //recommend 0, range (0 --> 9), default 0
    "encmode",              //range (-1000,1000,1001,1002), each means AUTO,SILK_ONLY,HYBRID,CELT_ONLY. default -1000
    "outch",                //force encoded as either mono or stereo, range (1, 2), default OPUS_ENC_SET_CHANNELS
    "app",                  //range (2048,2049,2051), each means VOIP,AUDIO,AUDIO. default 2049
    "fs",                   //input pcm sample rate, range (8k,12k,16k,24k,48k), default 0
    "ch",                   //input pcm channels, range (1,2), default 0
    "hasHead",              //if true, have 8 byte header, default false
    "signalType",           //range (-1000,3001,3002), each means AUTO,voice,music. default -1000
#endif
EXTERNC OpusApiRet_t opus_api_encoder_set(void* hd, const char* choose, void* val);
EXTERNC OpusApiRet_t opus_api_encoder_get(void* hd, const char* choose, void* val);
/*
pcmByte:            input, the address of a value which meaning the max byte of pcm buffer.
                    output, the address of a value which meaning the used byte of pcm buffer.
encodedFrameByte:   input, the address of a value which meaning the max byte of encodedFrame buffer.
                    output, the address of a value which meaning the valid byte of encodedFrame buffer.
*/
EXTERNC OpusApiRet_t opus_api_encoder_run(void* hd, unsigned char* pcm, int *pcmByte, unsigned char* encodedFrame, int* encodedFrameByte);
EXTERNC OpusApiRet_t opus_api_close_encoder(void* hd);
EXTERNC OpusApiRet_t opus_api_destory_encoder(void* hd);

//dec
EXTERNC OpusApiRet_t opus_api_create_ms_decoder(void** pHd, OpusApi_CreateDecParam_t *param);
EXTERNC OpusApiRet_t opus_api_create_decoder(void** pHd, OpusApi_CreateDecParam_t *param);
EXTERNC OpusApiRet_t opus_api_open_decoder(void* hd);
#if 0
    "fs",                   //pcm sample rate after decode, range (8k,12k,16k,24k,48k), default 0
    "ch",                   //pcm channels after decode, range (1,2), default 0
#endif
EXTERNC OpusApiRet_t opus_api_decoder_set(void* hd, const char* choose, void* val);
EXTERNC OpusApiRet_t opus_api_decoder_get(void* hd, const char* choose, void* val);
/*
decodecPcmByte:     input, the address of a value which meaning the max byte of decodecPcm buffer.
                    output, the address of a value which meaning the valid byte of decodecPcm buffer.
*/
EXTERNC OpusApiRet_t opus_api_decoder_run(void* hd, unsigned char* encodedOneFrame, int encodedOneFrameByte, unsigned char* decodecPcm, int* decodecPcmByte, bool isDoPlc);
EXTERNC OpusApiRet_t opus_api_close_decoder(void* hd);
EXTERNC OpusApiRet_t opus_api_destory_decoder(void* hd);




