#if 1
#include"Algo.AudioSamlpes.h"
#include"Ape.Inner.decoder.h"
#include"Ape.Inner.demuxer.h"
#include"ApeDemux.h"
using namespace Algo;
using namespace Audio;



typedef struct {
	ApeDemuxer_c demux;
	union {
		struct {
			b1 isInited;
		};
		u32 flags;
	};
}ApeDemuxState;


EXTERNC {
	int32_t ApeDemux_GetSize()
	{
		return sizeof(ApeDemuxState);
	}

	int32_t ApeDemux_Init(void* pStateIn)
	{
		//check
		if (!pStateIn)
			return APERET_FAIL;
		ApeDemuxState* pState = (ApeDemuxState*)pStateIn;
		ALGO_MEM_SET(pState, 0, ApeDemux_GetSize());
		ApeDemuxer_c* pDemux = &pState->demux;
		pState->isInited = true;
		return pDemux->Init();
	}

	int32_t ApeDemux_Set(void* pStateIn, ApeDemuxSet_e choose, void* val)
	{
		//check
		if (!pStateIn
			|| choose >= APE_DEMUX_SET_MAX)
			return APERET_FAIL;
		ApeDemuxState* pState = (ApeDemuxState*)pStateIn;
		if (pState->isInited == false)
			return APERET_FAIL;
		ApeDemuxer_c* pDemux = &pState->demux;
		switch (choose)
		{
		case APE_DEMUX_SET_SEEK_TABLE:
		{
			void** pVal = (void**)val;
			pDemux->seekTable.UpdataSeekTable((i32*)pVal[0], (i32)(pVal[1]));
		}
		default:
			break;
		}
		return APERET_SUCCESS;
	}

	int32_t ApeDemux_Get(void* pStateIn, ApeDemuxGet_e choose, void* val)
	{
		//check
		if (!pStateIn
			|| choose >= APE_DEMUX_GET_MAX
			|| !val)
			return APERET_FAIL;
		
		ApeDemuxState* pState = (ApeDemuxState*)pStateIn;
		if (pState->isInited == false)
			return APERET_FAIL;

		ApeDemuxer_c* pDemux = &pState->demux;
		switch (choose)
		{
		case APE_DEMUX_GET_IS_RUN_FINISH:
			*(u32*)val = (u32)pDemux->_isDemuxRunFinish;
			break;
		case APE_DEMUX_GET_HEADE:
			*(void**)val = &pDemux->context._context;
			break;
		case APE_DEMUX_GET_AUDIO_INFO:
		{
			void** pVal = (void**)val;
			*(u32*)pVal[0] = pDemux->context.GetHeader()->samplerate;
			*(u32*)pVal[1] = pDemux->context.GetHeader()->channels;
			*(u32*)pVal[2] = pDemux->context.GetHeader()->bps >> 3; 
			break;
		}
		case APE_DEMUX_GET_SEEK_TABLE:
			*(u32*)((void**)val)[0] = pDemux->context.seektablePos;
			*(u32*)((void**)val)[1] = pDemux->context._context.descriptor.seektablelength;
			break;
		case APE_DEMUX_GET_START_INFO_FROM_POS:
		{
			void** pVal = (void**)val;
			if (!pDemux->seekTable.GetStartInfoFromPos((i32*)pVal[0], (i32*)pVal[1], (i32*)pVal[2]))
				return APERET_FAIL;
			break;
		}
		default:
			break;
		}
		return APERET_SUCCESS;
	}

	int32_t ApeDemux_Run(void* pStateIn, uint8_t* in, int32_t inLen)
	{
		//check
		if (!pStateIn)
			return APERET_FAIL;
		ApeDemuxState* pState = (ApeDemuxState*)pStateIn;
		ApeDemuxer_c* pDemux = &pState->demux;
		if (pState->isInited)
			return pDemux->Run(in, inLen);
	}

	int32_t ApeDemux_DeInit(void* pStateIn)
	{
		if (!pStateIn)
			return APERET_FAIL;
		ApeDemuxState* pState = (ApeDemuxState*)pStateIn;
		pState->demux.DeInit();
		return APERET_SUCCESS;
	}
}

#endif