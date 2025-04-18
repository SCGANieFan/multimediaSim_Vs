#if 1
#include"Algo.AudioSamlpes.h"
#include"Algo.AudioCal.OverlapAdd.h"
#include"Algo.AudioCal.WaveFormMatch.h"
#include"AudioSpeedControl.h"

using namespace Algo;
using namespace Audio;
typedef struct {
#if 0
    ALGO_OVERLAP_ADD_CB OverlapAdd;
    ALGO_WAVE_FORM_MATCH_CB WaveFormMatch;
#endif
}FuncList;

typedef struct {
    AlgoBasePorting_t* basePorting;
    AudioInfo info;
    i32 seekSamples;
    i32 overlapSamples;
    i32 constSamples;
    FuncList *funcList;
    AudioSamples tmpBuf;
    AudioSamples iCache;
    f32 speed;
    b1 isBeginning;
    b1 isInited;
}ASC_State;

static FuncList funcList16_g = {
#if 0
    Algo_GetOverlapAdd(sizeof(i16),sizeof(i16)),    
    Get_Algo_WaveFormMatch(sizeof(i16)),
#endif
};

static FuncList funcList24_g = {
#if 0
    Algo_GetOverlapAdd(sizeof(i24),sizeof(i24)),
    Get_Algo_WaveFormMatch(sizeof(i24)),
#endif
};

static FuncList funcList32_g = {
#if 0
    Algo_GetOverlapAdd(sizeof(i32),sizeof(i32)),
    Get_Algo_WaveFormMatch(sizeof(i32)),
#endif
};

STATIC INLINE int32_t AudioSpeedControl_InMinSamlpes(ASC_State* pState)
{
    int32_t samples1 = (pState->seekSamples + pState->overlapSamples + pState->constSamples + pState->overlapSamples);
    int32_t samples2 = (int32_t)(pState->speed * (pState->overlapSamples + pState->constSamples));
    return MAX(samples1, samples2);
}

AudioSpeedControlRet AudioSpeedControl_RunInner(ASC_State* pState, AudioSamples* pIn, AudioSamples* pOut)
{
    int32_t offset = 0;
    if (pOut->GetLeftSamples() < (pState->overlapSamples + pState->constSamples))
        return AUDIO_SPEED_CONTROL_RET_OUT_BUFF_NOT_ENOUGH;
    if (pState->isBeginning == true) {
        pState->isBeginning = false;
        pState->tmpBuf.Append(*pIn, pIn->GetUsedSamples(), pState->overlapSamples);
    }
    int32_t bestLag;
#if 0
    bestLag = pState->asCalculator.WaveFormMatch(
        AS_Calculator::WaveformMatchChoose_e::WAVEFORM_MATCH_SUM,
        *pIn,
        pIn->GetUsedSamples(),
        pState->tmpBuf,
        0,
        pState->seekSamples,
        pState->overlapSamples);
#else
#if 0
    bestLag = pState->funcList->WaveFormMatch(
        Algo_WaveformMatchChoose_e::ALGO_WAVEFORM_MATCH_SUM,
        pIn->GetBufInSample(pIn->GetUsedSamples()),
        pState->tmpBuf.GetBufInSample(0),
        pIn->_info->_channels,
        pState->seekSamples,
        pState->overlapSamples);
#else
    bestLag = 0;
#endif
#endif
#if 0
    pState->asCalculator.OverlapAdd(
        pState->tmpBuf,
        0,
        *pIn,
        pIn->GetUsedSamples() + bestLag,
        pState->overlapSamples);
#else
#if 0
    pState->funcList->OverlapAdd(
        pState->tmpBuf.GetBufInSample(0),
        pState->tmpBuf.GetBufInSample(0),
        pIn->GetBufInSample(pIn->GetUsedSamples() + bestLag),
        pState->tmpBuf._info->_channels,
        0,
        pState->overlapSamples,
        pState->overlapSamples);
#endif
        
#endif
    pOut->Append(
        pState->tmpBuf,
        0,
        pState->overlapSamples);

    offset += bestLag + pState->overlapSamples;

    //copy constSamples from in to out
    pOut->Append(
        *pIn,
        pIn->GetUsedSamples() + offset,
        pState->constSamples);
    offset += pState->constSamples;

    //updata tmpBuf
    pState->tmpBuf.Clear(pState->tmpBuf.GetValidSamples());
    pState->tmpBuf.Append(*pIn, pIn->GetUsedSamples() + offset, pState->overlapSamples);

    pIn->Clear(pState->speed * (pState->overlapSamples + pState->constSamples));
    return AUDIO_SPEED_CONTROL_RET_SUCCESS;
}

EXTERNC{

    int32_t AudioSpeedControl_GetStateSize()
    {
        return sizeof(ASC_State);
    }

    AudioSpeedControlRet AudioSpeedControl_Init(void* pStateIn, AudioSpeedInitParam * param)
    {
        //check
        if (!pStateIn
            || !param)
            return AUDIO_SPEED_CONTROL_RET_INPUT_ERROR;

        if(!param->basePorting
            ||param->seekMs<0
            || param->overlapMs< 0
            || param->constMs< 0
            || param->speed< 0)
            return AUDIO_SPEED_CONTROL_RET_INPUT_ERROR;


        ASC_State* pState = (ASC_State*)pStateIn;
        ALGO_MEM_SET((u8*)pState, 0, sizeof(ASC_State));
        
        pState->basePorting = param->basePorting;
        pState->info.Init(param->fsHz, param->width, param->channels);

        pState->seekSamples = param->seekMs * pState->info._rate / 1000;
        pState->overlapSamples = param->overlapMs * pState->info._rate / 1000;
        pState->constSamples = param->constMs * pState->info._rate / 1000;


        if (pState->info._width == 2) {
            pState->funcList = &funcList16_g;
        }
        else if (pState->info._width == 3) {
            pState->funcList = &funcList24_g;
        }
        else {
            pState->funcList = &funcList32_g;
        }

        BufferSamples bufferSamples;
        bufferSamples._samples = pState->overlapSamples;
        bufferSamples._buf = (u8*)pState->basePorting->Malloc(bufferSamples._samples * pState->info._bytesPerSample);
        pState->tmpBuf.Init(&pState->info, &bufferSamples);
        
        bufferSamples._samples = AudioSpeedControl_InMinSamlpes(pState) * pState->info._bytesPerSample;
        bufferSamples._buf = (u8*)pState->basePorting->Malloc(bufferSamples._samples * pState->info._bytesPerSample);
        pState->iCache.Init(&pState->info, &bufferSamples);

        pState->speed = param->speed;
        pState->isBeginning = true;
        pState->isInited = true;
        return AUDIO_SPEED_CONTROL_RET_SUCCESS;
    }

    AudioSpeedControlRet AudioSpeedControl_Set(void* pStateIn, AudioSpeedControl_SetChhoose_e choose, void** val)
    {
        //check
        if (!pStateIn
            ||choose >= AUDIO_SPEED_CONTROL_SET_CHOOSE_MAX
            || !val)
            return AUDIO_SPEED_CONTROL_RET_INPUT_ERROR;
        ASC_State* pState = (ASC_State*)pStateIn;
        if (pState->isInited == false)
            return AUDIO_SPEED_CONTROL_RET_FAIL;

        switch (choose)
        {
        case AUDIO_SPEED_CONTROL_SET_CHOOSE_SPEEDQ8:
            pState->speed = (f32)(u32)val[0] / (1 << 8);
        default:
            break;
        }
        return AUDIO_SPEED_CONTROL_RET_SUCCESS;
    }

    AudioSpeedControlRet AudioSpeedControl_Get(void* pStateIn, AudioSpeedControl_GetChhoose_e choose, void** val)
    {
        //check
        if (!pStateIn
            || choose >= AUDIO_SPEED_CONTROL_GET_CHOOSE_MAX
            || !val)
            return AUDIO_SPEED_CONTROL_RET_INPUT_ERROR;
        ASC_State* pState = (ASC_State*)pStateIn;
        if (pState->isInited == false)
            return AUDIO_SPEED_CONTROL_RET_FAIL;

        switch (choose)
        {
        case AUDIO_SPEED_CONTROL_GET_CHOOSE_SPEEDQ8:
            *((u32*)(val[0])) = (u32)(pState->speed * (1 << 8));
        default:
            break;
        }
        return AUDIO_SPEED_CONTROL_RET_SUCCESS;
    }


    AudioSpeedControlRet AudioSpeedControl_Run(void* hd, uint8_t * in, int32_t inSize, uint8_t * out, int32_t * outSize)
    {
        //check
        if (!hd
            || !in
            || inSize < 0
            || !out)
            return AUDIO_SPEED_CONTROL_RET_INPUT_ERROR;

        ASC_State* pState = (ASC_State*)hd;
        Buffer buffer;
        AudioSamples pIn;
        buffer._buf = in;
        buffer._max = inSize;
        pIn.Init(&pState->info, &buffer);
        pIn.Append(pIn.GetSamplesMax());

        AudioSamples pOut;
        buffer._buf = out;
        buffer._max = *outSize;
        pOut.Init(&pState->info, &buffer);

        i32 usedSamples;
        AudioSpeedControlRet ret;
        while (1) {
            pState->iCache.AppendFully(pIn, &usedSamples);
            pIn.Used(usedSamples);
            if (!pState->iCache.IsFull()) {
                break;
            }
            ret = AudioSpeedControl_RunInner(pState, &pState->iCache, &pOut);
            if (ret != AUDIO_SPEED_CONTROL_RET_SUCCESS) {
                *outSize = pOut.GetValidSamples() * pState->info._bytesPerSample;
                return ret;
            }
        }
        *outSize = pOut.GetValidSamples() * pState->info._bytesPerSample;
        return AUDIO_SPEED_CONTROL_RET_SUCCESS;
    }

    AudioSpeedControlRet AudioSpeedControl_DeInit(void* pStateIn)
    {
        ASC_State* pState = (ASC_State*)pStateIn;
        pState->basePorting->Free(pState->tmpBuf.GetBufInSample(0));
        pState->basePorting->Free(pState->iCache.GetBufInSample(0));
        return AUDIO_SPEED_CONTROL_RET_SUCCESS;
    }
}
#endif

