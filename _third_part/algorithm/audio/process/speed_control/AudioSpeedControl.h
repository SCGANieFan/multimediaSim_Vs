#pragma once
#include"Algo.BasePorting.h"
#ifndef EXTERNC
#ifndef __cplusplus
#define EXTERNC
#else
#define EXTERNC extern "C"
#endif
#endif

#define AUDIO_SPEED_REF_VAL_SEEK_MS 15
#define AUDIO_SPEED_REF_VAL_OVERLAP_MS 8
#define AUDIO_SPEED_REF_VAL_CONST_MS 24


#define AUDIO_SPEED_CONTROL_RET_SUCCESS 0
#define AUDIO_SPEED_CONTROL_RET_FAIL -1
#define AUDIO_SPEED_CONTROL_RET_INPUT_ERROR -2
#define AUDIO_SPEED_CONTROL_RET_OUT_BUFF_NOT_ENOUGH -3

typedef int32_t AudioSpeedControlRet;

typedef struct{
    AlgoBasePorting_t* basePorting;
    int32_t fsHz;
    int16_t width;
    int16_t channels;
    int16_t seekMs;
    int16_t overlapMs;
    int16_t constMs;
    float speed;
}AudioSpeedInitParam;

typedef enum {
    AUDIO_SPEED_CONTROL_SET_CHOOSE_SPEEDQ8 = 0,
    AUDIO_SPEED_CONTROL_SET_CHOOSE_MAX,
}AudioSpeedControl_SetChhoose_e;

typedef enum {
    AUDIO_SPEED_CONTROL_GET_CHOOSE_SPEEDQ8 = 0,
    AUDIO_SPEED_CONTROL_GET_CHOOSE_MAX,
}AudioSpeedControl_GetChhoose_e;

EXTERNC int32_t AudioSpeedControl_GetStateSize();
EXTERNC AudioSpeedControlRet AudioSpeedControl_Init(void* pStateIn, AudioSpeedInitParam *param);
EXTERNC AudioSpeedControlRet AudioSpeedControl_Set(void* pStateIn, AudioSpeedControl_SetChhoose_e choose, void** val);
EXTERNC AudioSpeedControlRet AudioSpeedControl_Get(void* pStateIn, AudioSpeedControl_GetChhoose_e choose, void** val);
EXTERNC AudioSpeedControlRet AudioSpeedControl_Run(void* hd, uint8_t* in, int32_t inSize, uint8_t* out, int32_t* outSize);
EXTERNC AudioSpeedControlRet AudioSpeedControl_DeInit(void* pStateIn);



//Instructions for use
#if 0

//1. get handel
_hdSize = AudioSpeedControl_GetStateSize();
_hd = _memory.Malloc(_hdSize);

//2. init
AudioSpeedInitParam param;
BasePorting* basePorting = malloc(sizeof(BasePorting));
basePorting->Malloc = (ALGO_Malloc_t)malloc;
basePorting->Realloc = (ALGO_Realloc_t)realloc;
basePorting->Calloc = (ALGO_Calloc_t)calloc;
basePorting->Free = (ALGO_Free_t)free;
basePorting->MemCpy = (ALGO_MemCpy_t)memcpy;
basePorting->MemSet = (ALGO_MemSet_t)memset;
basePorting->MemMove = (ALGO_MemMove_t)memmove;
basePorting->Printf = (ALGO_Printf_t)printf;
param.basePorting = basePorting;
param.fsHz = _rate;
param.width = _width;
param.channels = _ch;
param.seekMs = AUDIO_SPEED_REF_VAL_SEEK_MS;
param.overlapMs = AUDIO_SPEED_REF_VAL_OVERLAP_MS;
param.constMs = AUDIO_SPEED_REF_VAL_CONST_MS;
param.speed = _speed;

AudioSpeedControl_Init(_hd, &param);

//3. set
int32_t speedQ8Set = (1.5f) * (1 << 8)
void* setParam[] = {
    (void*)(uint32_t)(),
};
AudioSpeedControl_Set(_hd, AudioSpeedControl_SetChhoose_e::AUDIO_SPEED_CONTROL_SET_CHOOSE_SPEEDQ8, setParam);

//4. get

int32_t speedQ8Get;
void* getParam[] = {
    (void*)(uint32_t)(&speedQ8),
};
AudioSpeedControl_Get(_hd, AudioSpeedControl_GetChhoose_e::AUDIO_SPEED_CONTROL_GET_CHOOSE_SPEEDQ8, getParam);

//5. run
int32_t outLen = outLenMax;
AudioSpeedControlRet ret = AudioSpeedControl_Run(void* hd, in, inLen, out, &outLen);
if (ret != AUDIO_SPEED_CONTROL_RET_SUCCESS)
    return false;

//6. deinit
AudioSpeedControl_DeInit(_hd);
free(_hd);




#endif