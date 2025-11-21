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
#ifndef __OPUS_API_H__
#define __OPUS_API_H__
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
    void(*print_cb)(const char* fmt, ...);              //log print function pointer, if donot want print, it can be null
}OpusApi_BasePort_t;

typedef enum {
    OPUS_API_ENC_CHOOSE_NONE= 1,                        //now, just support NORMAL
    OPUS_API_ENC_CHOOSE_NORMAL,                         //now, just support NORMAL
    OPUS_API_ENC_CHOOSE_INROM,
    OPUS_API_ENC_CHOOSE_WALKIE_TALKIE,
    OPUS_API_ENC_CHOOSE_MAX,
}OpusApi_EncChoose_c;

typedef enum {
    OPUS_API_DEC_CHOOSE_NONE = 1,                       //now, just support NORMAL
    OPUS_API_DEC_CHOOSE_NORMAL,                         //now, just support NORMAL
    OPUS_API_DEC_CHOOSE_WALKIE_TALKIE,
    OPUS_API_DEC_CHOOSE_MAX,
}OpusApi_DecChoose_c;

typedef enum {
    //OPUS_API_ENC_GET_NO_ONE = 0,
    OPUS_API_ENC_GET_MAX,
}OpusApi_EncGetChhoose_e;                               //reserve


typedef enum {
    OPUS_API_ENC_SET_MODE_AUTO = -1000,
    OPUS_API_ENC_SET_MODE_SILK_ONLY = 1000,
    OPUS_API_ENC_SET_MODE_HYBRID,
    OPUS_API_ENC_SET_MODE_CELT_ONLY,
}OpusApi_EncSetMode_e;

typedef enum {
    OPUS_API_ENC_SET_BIT_RATE = 0,                      //range (6k --> 510k)
    OPUS_API_ENC_SET_FRAME_DURATION_0P1MS,              //0.1ms, for example, if frame is 20ms, this value should be 200. range (25,50,100,200,400,600)
    OPUS_API_ENC_SET_USE_VBR,                           //if 1, use variable bitrate
    OPUS_API_ENC_SET_COMPLEXITY,                        //recommend 0, range (0 --> 9)
    OPUS_API_ENC_SET_MODE,                              //range (OpusApi_EncSetMode_e)
    OPUS_API_ENC_SET_OUT_CH,                            //range (1, 2)
    OPUS_API_ENC_SET_APPLICATION,
}OpusApi_EncSetChhoose_e;

typedef enum {
    //OPUS_API_DEC_GET_NO_ONE= 0,
    OPUS_API_DEC_GET_MAX,
}OpusApi_DecGetChhoose_e;                               //reserve

typedef enum {
    //OPUS_API_DEC_SET_NO_ONE = 0,
    OPUS_API_DEC_SET_MAX,
}OpusApi_DecSetChhoose_e;                               //reserve

//register
EXTERNC void OpusEncoderNormalRegister();
EXTERNC void OpusDecoderNormalRegister();

//enc
EXTERNC OpusApiRet_t opus_api_create_encoder(void** pHd, OpusApi_BasePort_t* basePort, int fs, int channels, bool isWithHead, OpusApi_EncChoose_c choose);
EXTERNC OpusApiRet_t opus_api_destory_encoder(void* hd);
EXTERNC OpusApiRet_t opus_api_encoder_set(void* hd, OpusApi_EncSetChhoose_e choose, void* val);
EXTERNC OpusApiRet_t opus_api_encoder_get(void* hd, OpusApi_EncGetChhoose_e choose, void* val);
EXTERNC OpusApiRet_t opus_api_encoder_run(void* hd, short* in, int inSample, unsigned char* out, int* outByte);

//dec
EXTERNC OpusApiRet_t opus_api_create_decoder(void** pHd, OpusApi_BasePort_t* basePort, int fs, int channels, OpusApi_DecChoose_c choose);
EXTERNC OpusApiRet_t opus_api_destory_decoder(void* hd);
EXTERNC OpusApiRet_t opus_api_decoder_set(void* hd, OpusApi_DecSetChhoose_e choose, void* val);
EXTERNC OpusApiRet_t opus_api_decoder_get(void* hd, OpusApi_DecGetChhoose_e choose, void* val);
EXTERNC OpusApiRet_t opus_api_decoder_run(void* hd, unsigned char* in, int inByte, short* out, int* outSample, bool isPlc);

#endif



