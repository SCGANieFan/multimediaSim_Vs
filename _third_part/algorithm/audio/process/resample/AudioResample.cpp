#if 1
//#include"Algo.AS_Calculator.h"
#include"Algo.AudioCal.Product.h"
#include"Algo.AudioData.h"
#include"AudioResample.h"

using namespace Algo;
using namespace Audio;

#define ONE_Q20	((i64)1 << 20)
#define MASK_Q20 (ONE_Q20 - 1)

typedef struct {
    ALGO_DOT_PRODUCT_ADD_CB ProductAdd;
    ALGO_DOT_PRODUCT_ADD_ACC_CB ProductAddAcc;
}FuncList;


typedef struct {
    BasePorting* basePorting;
    FuncList* funcList;
    int16_t channels;
    int16_t width;
    int16_t bytePerSample;
    int32_t iFs;
    int32_t oFs;
    int32_t KQ20;
    i32 iSamplesAcc;
    i32 oSamplesAcc;
    u8 lastSample[4*8];
    b1 isInited;
}AudioResampleState;

static FuncList funcList16_g = {
    Algo_ProductAdd<i16,i32>,
    Algo_ProductAddAcc<i16,i64>,
};

static FuncList funcList24_g = {
    Algo_ProductAdd<i24,i32>,
    Algo_ProductAddAcc<i24,i64>,
};

static FuncList funcList32_g = {
    Algo_ProductAdd<i32,i32>,
    Algo_ProductAddAcc<i32,i64>,
};

EXTERNC{

    int32_t AudioResample_GetStateSize()
    {
        return sizeof(AudioResampleState);
    }

    AudioResampleRet AudioResample_Init(void* pStateIn, AudioResampleInitParam* param)
    {
        //check
        if (!pStateIn
            || !param)
            return AUDIO_RESAMPLE_RET_INPUT_ERROR;

        if(!param->basePorting)
            return AUDIO_RESAMPLE_RET_INPUT_ERROR;

        AudioResampleState* pState = (AudioResampleState*)pStateIn;
        param->basePorting->MemSet((u8*)pState, 0, sizeof(AudioResampleState));
        
        pState->basePorting = param->basePorting;
        
        if (param->width == 2) {
            pState->funcList = &funcList16_g;
        }
        else if (param->width == 3) {
            pState->funcList = &funcList24_g;
        }
        else {
            pState->funcList = &funcList32_g;
        }


        pState->channels= param->channels;
        pState->width= param->width;
        pState->bytePerSample = pState->channels * pState->width;

        //pState->info.Init(param->iFs, param->width, param->channels);
        pState->iFs = param->iFs;
        pState->oFs = param->oFs;
        pState->KQ20 = (i32)(((i64)pState->iFs << 20) / pState->oFs);

        pState->isInited = true;
        return AUDIO_RESAMPLE_RET_SUCCESS;
    }

    AudioResampleRet AudioResample_Set(void* pStateIn, AudioResample_SetChhoose_e choose, void** val)
    {
        //check
        if (!pStateIn
            ||choose >= AUDIO_RESAMPLE_SET_CHOOSE_MAX
            || !val)
            return AUDIO_RESAMPLE_RET_INPUT_ERROR;
        AudioResampleState* pState = (AudioResampleState*)pStateIn;
        if (pState->isInited == false)
            return AUDIO_RESAMPLE_RET_FAIL;

        switch (choose)
        {
#if 0
        case AUDIO_RESAMPLE_SET_CHOOSE_SPEEDQ8:
            pState->speed = (f32)(u32)val[0] / (1 << 8);
#endif
        default:
            break;
        }
        return AUDIO_RESAMPLE_RET_SUCCESS;
    }

    AudioResampleRet AudioResample_Get(void* pStateIn, AudioResample_GetChhoose_e choose, void** val)
    {
        //check
        if (!pStateIn
            || choose >= AUDIO_RESAMPLE_GET_CHOOSE_MAX
            || !val)
            return AUDIO_RESAMPLE_RET_INPUT_ERROR;
        AudioResampleState* pState = (AudioResampleState*)pStateIn;
        if (pState->isInited == false)
            return AUDIO_RESAMPLE_RET_FAIL;

        switch (choose)
        {
#if 0
        case AUDIO_RESAMPLE_GET_CHOOSE_SPEEDQ8:
            *((u32*)(val[0])) = (u32)(pState->speed * (1 << 8));
#endif
        default:
            break;
        }
        return AUDIO_RESAMPLE_RET_SUCCESS;
    }


    AudioResampleRet AudioResample_Run(void* hd, uint8_t* in, int32_t* inSize, uint8_t* out, int32_t* outSize)
    {
        //check
        if (!hd
            || !in
            || !inSize
            || !out
            || !outSize)
            return AUDIO_RESAMPLE_RET_INPUT_ERROR;
        if (*inSize < 1
            || *outSize < 1)
            return AUDIO_RESAMPLE_RET_INPUT_ERROR;

        AudioResampleState* pState = (AudioResampleState*)hd;
        i32 oSamplesMax = *outSize / (pState->bytePerSample);
        i32 iSamplesMax = *inSize / (pState->bytePerSample);
        i32 oSamples;
        i32 iSamplesAccBases = pState->iSamplesAcc;
        i32 iSamplesFloor;
        i32 iSamplesCeil;
        i64 iSamplesQ20 = (i64)(pState->oSamplesAcc) * pState->KQ20;
        for (oSamples = 0; oSamples < oSamplesMax; oSamples++)
        {
            iSamplesQ20 += pState->KQ20;
            iSamplesFloor = (i32)(iSamplesQ20 >> 20) - iSamplesAccBases - 1;
            iSamplesCeil = (i32)((iSamplesQ20 + MASK_Q20) >> 20) - iSamplesAccBases - 1;
            if (iSamplesCeil >= iSamplesMax)
                break;
            i64 iSamplesFracFloorQ20 = iSamplesQ20 & MASK_Q20;
            i64 iSamplesFracCeilQ20 = ONE_Q20 - (iSamplesQ20 & MASK_Q20);
        
            //produce add
            u8* pInLast;
            if (iSamplesFloor >= 0)
                pInLast = &in[iSamplesFloor * pState->bytePerSample];
            else
                pInLast = pState->lastSample;

            u8* pInNow;
            pInNow = &in[iSamplesCeil * pState->bytePerSample];

            pState->funcList->ProductAdd(&out[oSamples * pState->bytePerSample], pInLast, (u8*)&iSamplesFracCeilQ20, 1, pState->channels, 20);
            pState->funcList->ProductAddAcc(&out[oSamples * pState->bytePerSample], pInNow, (u8*)&iSamplesFracFloorQ20, 1, pState->channels, 20);
            pState->oSamplesAcc++;

        }
        pState->iSamplesAcc += iSamplesCeil;

#if 1
        while (pState->iSamplesAcc >= pState->iFs) {
            pState->iSamplesAcc -= pState->iFs;
            pState->oSamplesAcc -= pState->oFs;
        }
#endif
        * inSize = iSamplesCeil * pState->bytePerSample;
        *outSize = oSamples * pState->bytePerSample;
        pState->basePorting->MemCpy(pState->lastSample, &in[iSamplesFloor * pState->bytePerSample], pState->bytePerSample);
        return AUDIO_RESAMPLE_RET_SUCCESS;
    }

    AudioResampleRet AudioResample_DeInit(void* pStateIn)
    {
#if 0
        AudioResampleState* pState = (AudioResampleState*)pStateIn;
#endif
        return AUDIO_RESAMPLE_RET_SUCCESS;
    }
}
#endif

