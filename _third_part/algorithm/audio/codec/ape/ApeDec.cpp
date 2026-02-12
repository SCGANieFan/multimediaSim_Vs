#if 1
#include"Algo.AudioSamlpes.h"
#include"Ape.Inner.decoder.h"
#include"Ape.Inner.demuxer.h"
#include"ApeDec.h"
using namespace Algo;
using namespace Audio;


typedef struct {
	ApeDecoder_c dec;
	union {
		struct {
			b1 isInited;
		};
		u32 flags;
	};
}ApeDecState;


EXTERNC {
	int32_t ApeDec_GetSize()
	{
		return sizeof(ApeDecState);
	}

	int32_t ApeDec_Init(void* pStateIn, ApeDecInitParam_t* sampleParam)
	{
		//check
		if (!pStateIn
			|| !sampleParam
			|| !sampleParam->basePorting)
			return APERET_FAIL;
		ApeDecState* pState = (ApeDecState*)pStateIn;
		ALGO_MEM_SET(pState, 0, ApeDec_GetSize());
		ApeDecoder_c* pDec = &pState->dec;
		i32 ret;
		ret = pDec->Init(sampleParam->basePorting, (ApeContext_t*)sampleParam->context, sampleParam->startFrame, sampleParam->skip);
		if (ret == APERET_SUCCESS) {
			pState->isInited = true;
		}
		return ret;
	}

	int32_t ApeDec_Set(void* pStateIn, ApeDecSet_e choose, void* val)
	{
		//check
		if (!pStateIn
			|| choose > APE_DEC_SET_E_MAX)
			return APERET_FAIL;
		ApeDecState* pState = (ApeDecState*)pStateIn;
		if (pState->isInited == false)
			return APERET_FAIL;
		ApeDecoder_c* pDec = &pState->dec;
		switch (choose)
		{
		case APE_DEC_SET_E_HAS_IN_CACHE:
			pDec->haveInCache= (b1)(u32)val; break;
		default:
			break;
		}
		return APERET_SUCCESS;
	}

	int32_t ApeDec_Get(void* pStateIn, ApeDecGet_e choose, void* val)
	{
		//check
		if (!pStateIn
			|| choose >= APE_DEC_GET_E_MAX
			|| !val)
			return APERET_FAIL;
		
		ApeDecState* pState = (ApeDecState*)pStateIn;
		if (pState->isInited == false)
			return APERET_FAIL;
		ApeDecoder_c* pDec = &pState->dec;
		switch (choose)
		{
		default:
			break;
		}
		return APERET_SUCCESS;
	}

	int32_t ApeDec_Run(void* pStateIn, uint8_t* in, int32_t *inLen, uint8_t* out, int32_t* outLen)
	{
#if 1
		//check
		if (!pStateIn
			||!out
			||!outLen)
			return APERET_FAIL;
		ApeDecState* pState = (ApeDecState*)pStateIn;
		ApeDecoder_c* pDec = &pState->dec;
		if (pState->isInited)
			return pDec->Run(in, inLen, out, outLen);
		return APERET_FAIL;
#endif
	}

	int32_t ApeDec_DeInit(void* pStateIn)
	{
		if (!pStateIn)
			return APERET_FAIL;
		ApeDecState* pState = (ApeDecState*)pStateIn;
		pState->dec.DeInitInner();
	}
}

#endif