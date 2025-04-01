#pragma once
#include"Algo.BasePorting.h"
#ifdef __cplusplus
#define EXTERNC	extern "C"
#else
#define EXTERNC
#endif

#define WAV_MUX_RET_SUCCESS 0
#define WAV_MUX_RET_FAIL -1

typedef struct
{
	AlgoBasePorting_t* basePorting;
#if 0
	int32_t fsHz;
	int16_t channels;
	int16_t width;
#endif
}WavMuxInitParam;


typedef enum {
#if 0
	WAV_MUX_SET_CHOOSE_RATE=0,
	WAV_MUX_SET_CHOOSE_CH,
	WAV_MUX_SET_CHOOSE_WIDTH,
#endif
	WAV_MUX_SET_CHOOSE_BASIC_INFO = 0,//rate ch widht
	WAV_MUX_SET_CHOOSE_MAX,
}WavMux_SetChhoose_e;

typedef enum {
	WAV_MUX_GET_CHOOSE_HEAD_SIZE = 0,
	WAV_MUX_GET_CHOOSE_HEAD,
	WAV_MUX_GET_CHOOSE_MAX,
}WavMux_GetChhoose_e;

EXTERNC int32_t WavMux_GetSize();
EXTERNC int32_t WavMux_Init(void* pStateIn, WavMuxInitParam* paramIns);
EXTERNC int32_t WavMux_Set(void* pStateIn, WavMux_SetChhoose_e choose, void* val);
EXTERNC int32_t WavMux_Get(void* pStateIn, WavMux_GetChhoose_e choose, void* val);
EXTERNC int32_t WavMux_Run(void* pStateIn, uint8_t* in, int32_t inLen, uint8_t* out, int32_t* outLen);
EXTERNC int32_t WavMux_DeInit(void* pStateIn);
