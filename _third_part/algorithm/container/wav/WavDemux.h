#pragma once
#include"Algo.BasePorting.h"
#ifdef __cplusplus
#define EXTERNC	extern "C"
#else
#define EXTERNC
#endif

#define WAV_DEMUX_RET_SUCCESS 0
#define WAV_DEMUX_RET_FAIL -1

typedef struct
{
	AlgoBasePorting_t* basePorting;
}WavDemuxInitParam;


typedef enum {
	WAV_DEMUX_SET_CHOOSE_MAX,
}WavDemux_SetChhoose_e;

typedef enum {
	WAV_DEMUX_GET_CHOOSE_HAS_HEAD=0,
	WAV_DEMUX_GET_CHOOSE_BASIC_INFO,//rate,ch,width
	WAV_DEMUX_GET_CHOOSE_DATA_POS,
	WAV_DEMUX_GET_CHOOSE_MAX,
}WavDemux_GetChhoose_e;

EXTERNC int32_t WavDemux_GetSize();
EXTERNC int32_t WavDemux_Init(void* pStateIn, WavDemuxInitParam* paramIns);
EXTERNC int32_t WavDemux_Set(void* pStateIn, WavDemux_SetChhoose_e choose, void** val);
EXTERNC int32_t WavDemux_Get(void* pStateIn, WavDemux_GetChhoose_e choose, void** val);
EXTERNC int32_t WavDemux_Run(void* pStateIn, uint8_t* in, int32_t inLen);
EXTERNC int32_t WavDemux_DeInit(void* pStateIn);
