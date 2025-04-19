#include"AudioSpeedControlInner.h"

// Processes as many processing frames of the samples 'inputBuffer', store
// the result into 'outputBuffer'


// Overlaps samples in 'midBuffer' with the samples in 'pInput'
void overlapMono(int16_t* out, const int16_t* in, int16_t* bufferTemplate,int32_t overlapLen)
{
    int i;
    int16_t m1, m2;

    m1 = (int16_t)0;
    m2 = (int16_t)overlapLen;

    for (i = 0; i < overlapLen; i++)
    {
        out[i] = (int16_t)(((int32_t)in[i] * m1 + (int32_t)bufferTemplate[i] * m2) / overlapLen);
        m1 += 1;
        m2 -= 1;
    }
}

// Overlaps samples in 'midBuffer' with the samples in 'input'. The 'Stereo' 
// version of the routine.
void overlapStereo(int16_t* out, const int16_t* in, int16_t* bufferTemplate, int32_t overlapLen)
{
    int i;
    int16_t temp;
    int cnt2;

    for (i = 0; i < overlapLen; i++)
    {
        temp = (int16_t)(overlapLen - i);
        cnt2 = 2 * i;
        out[cnt2] = (int16_t)(((int32_t)in[cnt2] * i + (int32_t)bufferTemplate[cnt2] * temp) / overlapLen);
        out[cnt2 + 1] = (int16_t)(((int32_t)in[cnt2 + 1] * i + (int32_t)bufferTemplate[cnt2 + 1] * temp) / overlapLen);
    }
}

// Overlaps samples in 'midBuffer' with the samples in 'input'. The 'Multi'
// version of the routine.
void overlapMulti(int16_t* out, const int16_t* in, int16_t* bufferTemplate, int32_t overlapLen, int32_t channels)
{
    short m1;
    int i = 0;

    for (m1 = 0; m1 < overlapLen; m1++)
    {
        short m2 = (short)(overlapLen - m1);
        for (int c = 0; c < channels; c++) {
            out[i] = (int16_t)(((int32_t)in[i] * m1 + (int32_t)bufferTemplate[i] * m2) / overlapLen);
            i++;
        }
    }
}

// Overlaps samples in 'midBuffer' with the samples in 'pInputBuffer' at position
// of 'ovlPos'.
int32_t overlap(int8_t* out, const int8_t* in, int8_t* bufferTemplate, int32_t overlapLen,int32_t channels,int32_t sampleWidth)
{
    if (sampleWidth != 2)
        return -1;
    if (channels == 1)
    {
        // mono sound.
        overlapMono((int16_t*)out, (int16_t*)in, (int16_t*)bufferTemplate, overlapLen);
    }
    else if (channels == 2)
    {
        // stereo sound
        overlapStereo((int16_t*)out, (int16_t*)in, (int16_t*)bufferTemplate, overlapLen);
    }
    else
    {
        assert(channels > 0);
        overlapMulti((int16_t*)out, (int16_t*)in, (int16_t*)bufferTemplate, overlapLen, channels);
    }
    return 0;
}











