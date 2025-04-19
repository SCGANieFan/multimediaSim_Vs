#include"AudioSpeedControlInner.h"


/// Update cross-correlation by accumulating "norm" coefficient by previously calculated value
int32_t calcCrossCorrAccumulate(const int16_t* inQ15, const int16_t* compareQ15, int64_t* normQ30, int32_t accorelationSamples, int32_t channels)
{
    int64_t corrQ30;
    int64_t lnormQ30;
    int32_t i;

    // hint compiler autovectorization that loop length is divisible by 8
    int32_t ilength = (channels * accorelationSamples) & -8;

    // cancel first normalizer tap from previous round
    lnormQ30 = 0;
    for (i = 1; i <= channels; i++)
    {
        lnormQ30 -= (int64_t)inQ15[-i] * inQ15[-i];
    }

    corrQ30 = 0;
    // Same routine for stereo and mono.
    for (i = 0; i < ilength; i += 2)
    {
        corrQ30 += ((int64_t)inQ15[i] * compareQ15[i] + (int64_t)inQ15[i + 1] * compareQ15[i + 1]);
    }

    // update normalizer with last samples of this round
    for (int32_t j = 0; j < channels; j++)
    {
        i--;
        lnormQ30 += (int64_t)inQ15[i] * inQ15[i];
    }
    
    *normQ30 += lnormQ30;

    // Normalize result by dividing by sqrt(norm) - this step is easiest
    // done using floating point operation
    //outQ10
    return (int32_t)((corrQ30 / ((int64_t)sqrt((*normQ30 < 1) ? 1 : *normQ30))) >> 5);
}

int32_t calcCrossCorr(const int16_t* inQ15, const int16_t* compareQ15, int64_t* normQ30,int32_t accorelationSamples,int32_t channels)
{
    int64_t corrQ30;
    uint64_t lnormQ30;
    int32_t i;
    int32_t ilength = (channels * accorelationSamples) & -8;
    corrQ30 = lnormQ30 = 0;
    for (i = 0; i < ilength; i += 2)
    {
        corrQ30 += ((int64_t)inQ15[i] * compareQ15[i] + (int64_t)inQ15[i + 1] * compareQ15[i + 1]);
        lnormQ30 += ((int64_t)inQ15[i] * inQ15[i] + (int64_t)inQ15[i + 1] * inQ15[i + 1]);
    }

    *normQ30 = lnormQ30;

    //out Q10
    return (int32_t)((corrQ30 / ((int64_t)sqrt((*normQ30 < 1) ? 1 : *normQ30))) >> 5);
}

#if 1
// Seeks for the optimal overlap-mixing position. The 'stereo' version of the
// routine
//
// The best position is determined as the position where the two overlapped
// sample sequences are 'most alike', in terms of the highest cross-correlation
// value over the overlapping period

int32_t seekBestOverlapPositionFull(const int16_t* refPos, int16_t* templateBuff, int32_t seekSamples, int32_t accorelationSamples, int32_t channels)
{
    int32_t bestOffs;
    int32_t bestCorrQ10;
    int32_t i;
    int64_t normQ30;
    
    bestOffs = 0;
    bestCorrQ10 = calcCrossCorr(refPos, templateBuff, &normQ30, accorelationSamples, channels);
#if 0
    //bestCorrQ10 = (bestCorrQ10 + 0.1) * 0.75;
    int32_t facQ15 = (1.0f * (((int32_t)1) << 15));
    bestCorrQ10 = ((int64_t)bestCorrQ10 * facQ15) >> 15;
#endif
for (i = 1; i < seekSamples; i++)
{
    int32_t corrQ10;
    corrQ10 = calcCrossCorrAccumulate(refPos + channels * i, templateBuff, &normQ30, accorelationSamples, channels);
#if 0
    int32_t tmpQ15 = ((2 * i - seekSamples) << 15) / seekSamples;//[-1,1]
    //corrQ10 = ((corrQ10 + 0.1) * (1.0 - 0.25 * tmp * tmp));
    corrQ10 = ((corrQ10 + (int32_t)(0.1f * (((int32_t)1) << 15))) * ((((int32_t)1) << 15) - (((tmpQ15 * tmpQ15) >> 15) >> 2))) >> 15;
#endif
    if (corrQ10 > bestCorrQ10)
    {
        bestCorrQ10 = corrQ10;
        bestOffs = i;
    }
}
//adaptNormalizer(pTDStretch);
return bestOffs;
}
#endif

// Seeks for the optimal overlap-mixing position.
int32_t SeekBestLag(int8_t* in, int8_t* templateBuff,int32_t sampleWidth, int32_t seekSamples, int32_t accorelationSamples,int32_t channels)
{
    if (sampleWidth != 2)
        return -1;
#if 0
    int32_t bestLag;
    return bestLag=seekBestOverlapPositionQuick(pTDStretch, in, templateBuff);
#else
    return seekBestOverlapPositionFull((int16_t*)in, (int16_t*)templateBuff, seekSamples, accorelationSamples, channels);
#endif

}