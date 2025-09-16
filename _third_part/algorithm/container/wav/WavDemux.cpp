#if 1
#include"Algo.Type.h"
#include"Wav.Inner.Demuxer.h"
#include"WavDemux.h"


typedef struct
{
	WavDemuxer_c demuxer;
	b1 isInited;
}WavDemuxState;


EXTERNC {
	int32_t WavDemux_GetSize()
	{
		return sizeof(WavDemuxState);
	}

	int32_t WavDemux_Init(void* pWavDemuxStateIn, WavDemuxInitParam* paramIn)
	{
		//check
		if (!pWavDemuxStateIn
			|| !paramIn
			|| !paramIn->basePorting)
			return WAV_DEMUX_RET_FAIL;

		WavDemuxState* pState = (WavDemuxState*)pWavDemuxStateIn;
		if(!pState->demuxer.Init(paramIn->basePorting))
			return WAV_DEMUX_RET_FAIL;
		pState->isInited = true;

		return WAV_DEMUX_RET_SUCCESS;
	}

	int32_t WavDemux_Set(void* pWavDemuxStateIn, WavDemux_SetChhoose_e choose, void** val)
	{
		//check
		if (!pWavDemuxStateIn
			|| choose >= WAV_DEMUX_SET_CHOOSE_MAX
			|| !val)
			return WAV_DEMUX_RET_FAIL;
		WavDemuxState* pWavDemux = (WavDemuxState*)pWavDemuxStateIn;
		if (pWavDemux->isInited == false)
			return WAV_DEMUX_RET_FAIL;

#if 0
		switch (choose)
		{
		case AUDIO_SPEED_CONTROL_SET_CHOOSE_SPEEDQ8:
			pState->speed = (f32)(u32)val[0] / (1 << 8);
		default:
			break;
		}
#endif
		return WAV_DEMUX_RET_SUCCESS;
	}

	int32_t WavDemux_Get(void* pWavDemuxStateIn, WavDemux_GetChhoose_e choose, void** val)
	{
		//check
		if (!pWavDemuxStateIn
			|| choose >= WAV_DEMUX_GET_CHOOSE_MAX
			|| !val)
			return WAV_DEMUX_RET_FAIL;
		WavDemuxState* pWavDemux = (WavDemuxState*)pWavDemuxStateIn;
		if (pWavDemux->isInited == false)
			return WAV_DEMUX_RET_FAIL;

		switch (choose)
		{
		case WAV_DEMUX_GET_CHOOSE_HAS_HEAD:
			if(pWavDemux->demuxer.GetIsDemuxFinish())
				*(u32*)val[0] = 1;
			else
				*(u32*)val[0] = 0;
			break;			
		case WAV_DEMUX_GET_CHOOSE_BASIC_INFO:
			*(u32*)val[0] = pWavDemux->demuxer.GetSampleRate();
			*(u32*)val[1] = pWavDemux->demuxer.GetNumChannels();
			*(u32*)val[2] = pWavDemux->demuxer.GetWidth();
			break;
		case WAV_DEMUX_GET_CHOOSE_DATA_POS:
			*(u32*)val = pWavDemux->demuxer.GetDataPos();
			break;
		default:
			break;
		}
		return WAV_DEMUX_RET_SUCCESS;
	}

	int32_t WavDemux_Run(void* pStateIn, uint8_t* in, int32_t inLen)
	{
		//check
		if (!pStateIn)
			return WAV_DEMUX_RET_FAIL;
		return ((WavDemuxState*)pStateIn)->demuxer.Receive(in, inLen)? WAV_DEMUX_RET_SUCCESS : WAV_DEMUX_RET_FAIL;
	}

	int32_t WavDemux_DeInit(void* pStateIn)
	{
		if (!pStateIn)
			return WAV_DEMUX_RET_FAIL;
		((WavDemuxState*)pStateIn)->demuxer.DeInit();
		return WAV_DEMUX_RET_SUCCESS;
	}
}

#endif