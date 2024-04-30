#include"Algo.AS_Calculator32.h"
b1 AS_Calculator32::OverlapAdd(AudioSamples& dst, i32 dstSample, AudioSamples& src, i32 srcSample, i32 overlapSample)
{
	//check
	i32 fixNum = 15;
	i32 factor;

	i32* pBuf = (i32*)&dst[dstSample];
	i32* pSrc = (i32*)&src[srcSample];

	for (i32 s = 0; s < overlapSample; s++)
	{
		factor = ((i32)s << fixNum) / overlapSample;
		for (i16 ch = 0; ch < dst._info->_channels; ch++)
		{
			*pBuf = ((i64)(*pBuf) * (((i32)1 << fixNum) - factor) + (i64)(*pSrc) * factor) >> fixNum;
			pBuf++;
			pSrc++;
		}
	}
	return true;
}