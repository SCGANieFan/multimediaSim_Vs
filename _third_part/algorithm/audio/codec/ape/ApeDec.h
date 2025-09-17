#pragma once
#if 1
#include"ApeCom.h"


typedef enum {
	APE_DEC_INIT_MODE_DEC_USE_CONTEXT=0,
	APE_DEC_INIT_MODE_DEC,
	APE_DEC_INIT_MODE_MAX,
}ApeDecInitMode_e;

typedef struct
{
	AlgoBasePorting_c* basePorting;
	void* context;
	uint32_t startFrame;
	uint32_t skip;
}ApeDecInitParam_t;


typedef enum {
	APE_DEC_SET_E_HAS_IN_CACHE=0,
	APE_DEC_SET_E_MAX,
}ApeDecSet_e;

typedef enum {
	APE_DEC_GET_E_DEFAULT=0,
	APE_DEC_GET_E_MAX,
}ApeDecGet_e;


EXTERNC int32_t ApeDec_GetSize();
EXTERNC int32_t ApeDec_Init(void* pStateIn, ApeDecInitParam_t* sampleParam);
EXTERNC int32_t ApeDec_Set(void* pStateIn, ApeDecSet_e choose, void* val);
EXTERNC int32_t ApeDec_Get(void* pStateIn, ApeDecGet_e choose, void* val);
EXTERNC int32_t ApeDec_Run(void* pStateIn, uint8_t* in, int32_t inLen, uint8_t* out, int32_t* outLen);
EXTERNC int32_t ApeDec_DeInit(void* pStateIn);

#endif