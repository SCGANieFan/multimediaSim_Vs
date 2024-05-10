#if 1
#include"Algo.AS_Calculator.h"
#include"AudioSpeedControl.h"

typedef struct {
    BasePorting* basePorting;
    AudioInfo info;
    i32 seekSamples;
    i32 overlapSamples;
    i32 constSamples;
    AS_Calculator* asCalculator;
    AudioSamples tmpBuf;
    AudioSamples iCache;
    f32 speed;
    b1 isBeginning;
    b1 isInited;
}ASC_State;


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
    bestLag = pState->asCalculator->WaveFormMatch(
        AS_Calculator::WaveformMatchChoose_e::WAVEFORM_MATCH_SUM,
        *pIn,
        pIn->GetUsedSamples(),
        pState->tmpBuf,
        0,
        pState->seekSamples,
        pState->overlapSamples);

    pState->asCalculator->OverlapAdd(
        pState->tmpBuf,
        0,
        *pIn,
        pIn->GetUsedSamples() + bestLag,
        pState->overlapSamples);

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
        param->basePorting->MemSet((u8*)pState, 0, sizeof(ASC_State));
        
        pState->basePorting = param->basePorting;
        pState->info.Init(param->fsHz, param->width, param->channels);

        pState->seekSamples = param->seekMs * pState->info._rate / 1000;
        pState->overlapSamples = param->overlapMs * pState->info._rate / 1000;
        pState->constSamples = param->constMs * pState->info._rate / 1000;

        pState->asCalculator = AS_Calculator::GetCaculator(param->width);

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
        pState->basePorting->Free(&pState->tmpBuf[0]);
        pState->basePorting->Free(&pState->iCache[0]);
        return AUDIO_SPEED_CONTROL_RET_SUCCESS;
    }
}
#endif
