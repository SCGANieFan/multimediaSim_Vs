#if 1
//#include"Algo.Data.h"
#include"Wav.Inner.Muxer.h"
#include"WavMux.h"

//using namespace Algo;

typedef struct
{
	WavMuxer_c wavMuxer;
	b1 isInited;
}WavMuxState;



EXTERNC {
	int32_t WavMux_GetSize()
	{
		return sizeof(WavMuxState);
	}

	int32_t WavMux_Init(void* pStateIn, WavMuxInitParam* paramIn)
	{
		//check
		if (!pStateIn
			|| !paramIn
			|| !paramIn->basePorting)
			return WAV_MUX_RET_FAIL;

		WavMuxState* pState = (WavMuxState*)pStateIn;
		ALGO_MEM_SET((u8*)pState, 0, WavMux_GetSize());

		if(!pState->wavMuxer.Init(paramIn->basePorting))
			return WAV_MUX_RET_FAIL;
		pState->isInited = true;
		return WAV_MUX_RET_SUCCESS;
	}

	int32_t WavMux_Set(void* pStateIn, WavMux_SetChhoose_e choose, void* val)
	{
		//check
		if (!pStateIn
			|| choose >= WAV_MUX_SET_CHOOSE_MAX
			|| !val)
			return WAV_MUX_RET_FAIL;
		WavMuxState* pWavMux = (WavMuxState*)pStateIn;
		if (pWavMux->isInited == false)
			return WAV_MUX_RET_FAIL;

		switch (choose)
		{
		case WAV_MUX_SET_CHOOSE_BASIC_INFO:
			i32 rate = (i32)((void**)val)[0];
			i16 ch = (i16)((void**)val)[1];
			i16 widht = (i16)((void**)val)[2];
			pWavMux->wavMuxer.SetSampleRate(rate);
			pWavMux->wavMuxer.SetNumChannels(ch);
			pWavMux->wavMuxer.SetWidth(widht);
#if 0
		default:
			break;
#endif
		}
		return WAV_MUX_RET_SUCCESS;
	}

	int32_t WavMux_Get(void* pStateIn, WavMux_GetChhoose_e choose, void* val)
	{
		//check
		if (!pStateIn
			|| choose >= WAV_MUX_GET_CHOOSE_MAX
			|| !val)
			return WAV_MUX_RET_FAIL;
		WavMuxState* pWavMux = (WavMuxState*)pStateIn;
		if (pWavMux->isInited == false)
			return WAV_MUX_RET_FAIL;
		
		switch (choose)
		{
#if 1
		case WAV_MUX_GET_CHOOSE_HEAD_SIZE:
			*(u32*)val = sizeof(WavHead) - 8;
			break;
		case WAV_MUX_GET_CHOOSE_HEAD:
			pWavMux->wavMuxer.GetHead(val);
			break;
#endif
		default:
			break;
		}
		return WAV_MUX_RET_SUCCESS;
	}

	int32_t WavMux_Run(void* pStateIn, uint8_t* in, int32_t inLen, uint8_t* out, int32_t* outLen)
	{
		//check
		if (!pStateIn)
			return WAV_MUX_RET_FAIL;
		if(!((WavMuxState*)pStateIn)->wavMuxer.Process(in, inLen))
			return WAV_MUX_RET_FAIL;
		return WAV_MUX_RET_SUCCESS;
	}

	int32_t WavMux_DeInit(void* pStateIn)
	{
		if (!pStateIn)
			return WAV_MUX_RET_FAIL;
		((WavMuxState*)pStateIn)->wavMuxer.DeInit();
		return WAV_MUX_RET_SUCCESS;
	}
}

#endif