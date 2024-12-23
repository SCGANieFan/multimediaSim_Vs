#if 0
#include"Algo.AudioSamlpes.h"
#include"Algo.AudioCal.Basic.h"
#include"Algo.AudioCal.Product.h"
#include"Algo.AudioCal.OverlapAdd.h"
#include"Algo.AudioCal.WaveFormMatch.h"
#include"Ape.h"

using namespace Algo;
using namespace Audio;

#define APEMIN_FRAME_MS 10
#define APESEEK_MS_DEFAULT 2
#define APEMATCH_MS_DEFAULT 2

typedef enum {
	MUTE_MODE_NOMUTE = 0,
	MUTE_MODE_DOMUTE,
	MUTE_MODE_DOMUTE_ZERO,
}MUTE_MODE;

typedef struct {
	ALGO_PRODUCT_CB product;
	ALGO_OVERLAP_ADD_CB Overlapadd;
	ALGO_WAVE_FORM_MATCH_CB WaveFormMatch;
	ALGO_APPEND_IN_FIXPOINT AppendInFixpoint;
}FuncList;

typedef struct
{
	AlgoBasePorting* basePorting;
	AudioInfo info;
	AudioInfo muteInfo;

	i32 frameSamples;
	i32 lostCount;
	i32 overlapInSamples;
	i32 frameSamplesInner;
	i32 decaySamples;

	FuncList* funcList;
	AudioSamples inHistory;
	AudioSamples infuture;
	AudioSamples fillSignal;
	AudioSamples muteFactor;

	i32 decaySamplesNow;
	i32 fillSignalSampleIndex;
	i32 matchSamples;
	i32 seekSamples;

	b1 isInited;
}MusicPlcState;


static FuncList funcList16_g = {
	Algo_Product<i16,i16>,
	Algo_OverlapAdd<i16>,
	Get_Algo_WaveFormMatch(sizeof(i16)),
	Algo_AppendInFixPoint<i16,i32>,
};

static FuncList funcList24_g = {
	Algo_Product<i24,i24>,
	Algo_OverlapAdd<i24>,
	Get_Algo_WaveFormMatch(sizeof(i24)),
	Algo_AppendInFixPoint<i24,i32>,
};

static FuncList funcList32_g = {
	Algo_Product<i32,i32>,
	Algo_OverlapAdd<i32>,
	Get_Algo_WaveFormMatch(sizeof(i32)),
	Algo_AppendInFixPoint<i32,i32>,
};

STATIC INLINE i32 MusicPlcGetMuteFactorSamples(i32 frameSamples)
{
	return frameSamples;
}

STATIC INLINE i32 MusicPlcGetFillSignalSamples(i32 frameSamples)
{
	return frameSamples;
}

STATIC INLINE i32 MusicPlcGetHistorySamples(i32 overlapInSamples, i32 frameSamples)
{
	return frameSamples + overlapInSamples;
}

STATIC INLINE i32 MusicPlcGetFutureSamples(i32 frameSamples)
{
	return frameSamples;
}

STATIC b1 MusicPlcCheckParam(i32 overlapMs, i32 frameSamples, i32 fsHz, i16 channels, i16 width)
{
	if (overlapMs < 1
		|| frameSamples < 1)
		return false;
	i32 overlapInSamples = overlapMs * fsHz / 1000;
	if (overlapInSamples > frameSamples)
		return false;
	if (channels != 1 && channels != 2 && channels != 4)
		return false;
	if (width != 2 && width != 3 && width != 4)
		return false;

	return true;
}

EXTERNC {
	int32_t Ape_GetStateSize()
	{
		return sizeof(MusicPlcState);
	}

	int32_t Ape_StateInit(void* pMusicPlcStateIn, ApeInitParam* sampleParam)
	{
		//check
		if (!pMusicPlcStateIn
			|| !sampleParam
			|| !sampleParam->basePorting
			|| sampleParam->overlapMs < 0
			|| sampleParam->decayTimeMs < 0)
			return APERET_FAIL;

		if (!MusicPlcCheckParam(sampleParam->overlapMs, sampleParam->frameSamples, sampleParam->fsHz, sampleParam->channels, sampleParam->width))
			return APERET_FAIL;

		//
		MusicPlcState* pPlc = (MusicPlcState*)pMusicPlcStateIn;
		ALGO_MEM_SET((u8*)pPlc, 0, Ape_GetStateSize());
		
		pPlc->basePorting = sampleParam->basePorting;

		pPlc->info.Init(sampleParam->fsHz, sampleParam->width, sampleParam->channels);

		pPlc->info.Init(sampleParam->fsHz, sampleParam->width, sampleParam->channels);
		pPlc->muteInfo.Init(pPlc->info._rate, pPlc->info._width, 1);
		
		pPlc->frameSamples = sampleParam->frameSamples;
		pPlc->overlapInSamples = sampleParam->overlapMs * pPlc->info._rate / 1000;
		pPlc->frameSamplesInner = MAX(pPlc->frameSamples, pPlc->info._rate * APEMIN_FRAME_MS / 1000);
		pPlc->decaySamples = sampleParam->decayTimeMs * pPlc->info._rate / 1000;

		if (pPlc->info._width == 2){
			pPlc->funcList = &funcList16_g;
		}
		else if (pPlc->info._width == 3) {
			pPlc->funcList = &funcList24_g;
		}
		else{
			pPlc->funcList = &funcList32_g;
		}
		BufferSamples bufferSamples;
		
		bufferSamples._samples = MusicPlcGetHistorySamples(pPlc->overlapInSamples, pPlc->frameSamplesInner);
		bufferSamples._buf = (u8*)pPlc->basePorting->Malloc(bufferSamples._samples * pPlc->info._bytesPerSample);
		pPlc->inHistory.Init(&pPlc->info, &bufferSamples);
		pPlc->inHistory.Append(pPlc->inHistory, 0, pPlc->inHistory.GetSamplesMax());

		bufferSamples._samples = MusicPlcGetFutureSamples(pPlc->frameSamples);
		bufferSamples._buf = (u8*)pPlc->basePorting->Malloc(bufferSamples._samples * pPlc->info._bytesPerSample);
		pPlc->infuture.Init(&pPlc->info, &bufferSamples);

		bufferSamples._samples = MusicPlcGetFillSignalSamples(pPlc->frameSamplesInner);
		bufferSamples._buf = (u8*)pPlc->basePorting->Malloc(bufferSamples._samples * pPlc->info._bytesPerSample);
		pPlc->fillSignal.Init(&pPlc->info, &bufferSamples);

		bufferSamples._samples = MusicPlcGetMuteFactorSamples(pPlc->frameSamples);
		bufferSamples._buf = (u8*)pPlc->basePorting->Malloc(bufferSamples._samples * pPlc->muteInfo._bytesPerSample);
		pPlc->muteFactor.Init(&pPlc->muteInfo, &bufferSamples, 14);

		pPlc->fillSignalSampleIndex = 0;
		pPlc->matchSamples = APEMATCH_MS_DEFAULT * pPlc->info._rate / 1000;
		pPlc->matchSamples = pPlc->matchSamples < pPlc->overlapInSamples ? pPlc->matchSamples : pPlc->overlapInSamples;
		pPlc->seekSamples = APESEEK_MS_DEFAULT * pPlc->info._rate / 1000;
		
		pPlc->isInited = true;

		return APERET_SUCCESS;
	}

	int32_t Ape_Set(void* pMusicPlcStateIn, ApeSetChhoose_e choose, void** val)
	{
		//check
		if (!pMusicPlcStateIn
			|| choose >= APESET_CHOOSE_MAX
			|| !val)
			return APERET_FAIL;
		MusicPlcState* pMusicPlc = (MusicPlcState*)pMusicPlcStateIn;
		if (pMusicPlc->isInited == false)
			return APERET_FAIL;

		switch (choose)
		{
#if 0
		case AUDIO_SPEED_CONTROL_SET_CHOOSE_SPEEDQ8:
			pState->speed = (f32)(u32)val[0] / (1 << 8);
#endif
		default:
			break;
		}
		return APERET_SUCCESS;
	}

	int32_t Ape_Get(void* pMusicPlcStateIn, ApeGetChhoose_e choose, void** val)
	{
		//check
		if (!pMusicPlcStateIn
			|| choose >= APEGET_CHOOSE_MAX
			|| !val)
			return APERET_FAIL;
		MusicPlcState* pMusicPlc = (MusicPlcState*)pMusicPlcStateIn;
		if (pMusicPlc->isInited == false)
			return APERET_FAIL;
		
		switch (choose)
		{
#if 0
		case AUDIO_SPEED_CONTROL_SET_CHOOSE_SPEEDQ8:
			pState->speed = (f32)(u32)val[0] / (1 << 8);
#endif
		default:
			break;
		}
		return APERET_SUCCESS;
	}

	int32_t Ape_Run(void* pMusicPlcStateIn, uint8_t* in, int32_t inLen, uint8_t* out, int32_t* outLen, bool isLost)
	{
		//check
		if (!pMusicPlcStateIn
			||!out
			||!outLen)
			return APERET_FAIL;
		MusicPlcState* pMusicPlc = (MusicPlcState*)pMusicPlcStateIn;
		if (isLost == false) {
			if (!in
				|| inLen != (pMusicPlc->frameSamples * pMusicPlc->info._bytesPerSample))
				return APERET_FAIL;
		}
		if (*outLen < pMusicPlc->frameSamples * pMusicPlc->info._bytesPerSample)
			return APERET_FAIL;

		BufferSamples bufferSamples;

		AudioSamples pIn;
		bufferSamples._buf = in;
		bufferSamples._samples = pMusicPlc->frameSamples;
		pIn.Init(&((MusicPlcState*)pMusicPlcStateIn)->info, &bufferSamples);

		AudioSamples pOut;
		bufferSamples._buf = out;
		bufferSamples._samples = pMusicPlc->frameSamples;
		pOut.Init(&((MusicPlcState*)pMusicPlcStateIn)->info, &bufferSamples);

		MUTE_MODE muteMode = MUTE_MODE_NOMUTE;
		if (isLost == false)
		{
			if (pMusicPlc->lostCount == 0)
			{
				//infuture
				if ((pMusicPlc->decaySamplesNow - 1) < 0)
				{
					//quickly Deal
					pOut.Append(pMusicPlc->inHistory, pMusicPlc->inHistory.GetSamplesMax() - pMusicPlc->overlapInSamples, pMusicPlc->overlapInSamples);
					pOut.Append(pIn, 0, pMusicPlc->frameSamples - pMusicPlc->overlapInSamples);

					//shift
					pMusicPlc->inHistory.Clear(pMusicPlc->frameSamples);
					pMusicPlc->inHistory.Append(pIn, 0, pMusicPlc->frameSamples);
					*outLen = pOut.GetValidSamples() * pMusicPlc->info._bytesPerSample;
					return APERET_SUCCESS;
				}
				else
					pMusicPlc->infuture.Append(pIn, 0, pMusicPlc->frameSamples);
			}
			else
			{
				pMusicPlc->lostCount = 0;
				//infuture
				for (i32 i = 0; i < pMusicPlc->overlapInSamples; i++)
				{
					pMusicPlc->infuture.Append(pMusicPlc->fillSignal, pMusicPlc->fillSignalSampleIndex, 1);
					pMusicPlc->fillSignalSampleIndex = (pMusicPlc->fillSignalSampleIndex + 1) % pMusicPlc->fillSignal.GetValidSamples();
				}
#if 1
				pMusicPlc->funcList->Overlapadd(					
					pMusicPlc->infuture.GetBufInSample(0),
					pMusicPlc->infuture.GetBufInSample(0),
					pIn.GetBufInSample(0),
					pMusicPlc->overlapInSamples,
					pMusicPlc->infuture._info->_channels);
#endif
				pMusicPlc->infuture.Append(pIn, pMusicPlc->overlapInSamples, pMusicPlc->frameSamples - pMusicPlc->overlapInSamples);
			}

			//muteFactor
			muteMode = MUTE_MODE_DOMUTE;
			for (i32 i = 0; i < pMusicPlc->frameSamples; i++)
			{
				pMusicPlc->decaySamplesNow--;
				pMusicPlc->decaySamplesNow = MAX(pMusicPlc->decaySamplesNow, 0);
#if 0
				pMusicPlc->asCalculator.AppendInFixPoint(pMusicPlc->muteFactor, pMusicPlc->decaySamples - pMusicPlc->decaySamplesNow, pMusicPlc->decaySamples);
#else
				i32 num = pMusicPlc->decaySamples - pMusicPlc->decaySamplesNow;
				pMusicPlc->funcList->AppendInFixpoint(
					pMusicPlc->muteFactor.GetBufInSample(i),
					(u8*)&num,
					(u8*)&(pMusicPlc->decaySamples),
					1,
					pMusicPlc->muteFactor._info->_channels,
					pMusicPlc->muteFactor.GetFPNum());
				pMusicPlc->muteFactor.Append(1);
#endif
			}
		}
		else
		{
			pMusicPlc->lostCount += 1;
			if (pMusicPlc->lostCount == 1)
			{
#if 1

				i32 bestLag = pMusicPlc->funcList->WaveFormMatch(
					Algo_WaveformMatchChoose_e::ALGO_WAVEFORM_MATCH_SUM,					
					pMusicPlc->inHistory.GetBufInSample(0),
					pMusicPlc->inHistory.GetBufInSample(pMusicPlc->inHistory.GetSamplesMax() - pMusicPlc->overlapInSamples),
					pMusicPlc->inHistory._info->_channels,
					pMusicPlc->seekSamples,
					pMusicPlc->matchSamples);
#else		
				i32 bestLag = 0;
#endif
				//overlap add
#if 1
				pMusicPlc->funcList->Overlapadd(					
					pMusicPlc->infuture.GetBufInSample(pMusicPlc->inHistory.GetSamplesMax() - pMusicPlc->overlapInSamples),
					pMusicPlc->infuture.GetBufInSample(pMusicPlc->inHistory.GetSamplesMax() - pMusicPlc->overlapInSamples),
					pMusicPlc->inHistory.GetBufInSample(bestLag),
					pMusicPlc->overlapInSamples,
					pMusicPlc->inHistory._info->_channels);
#endif
				//fill signal
				pMusicPlc->fillSignal.Clear(pMusicPlc->fillSignal.GetValidSamples());
				pMusicPlc->fillSignal.Append(
					pMusicPlc->inHistory,
					bestLag + pMusicPlc->overlapInSamples,
					pMusicPlc->inHistory.GetSamplesMax() - (bestLag + pMusicPlc->overlapInSamples));
				pMusicPlc->fillSignalSampleIndex = 0;
			}
			//mute
			if ((pMusicPlc->decaySamplesNow + 1) <= pMusicPlc->decaySamples) {
				muteMode = MUTE_MODE_DOMUTE;
				for (i32 i = 0; i < pMusicPlc->frameSamples; i++) {
					pMusicPlc->decaySamplesNow++;
					pMusicPlc->decaySamplesNow = MIN(pMusicPlc->decaySamplesNow, pMusicPlc->decaySamples);
#if 0
					pMusicPlc->asCalculator.AppendInFixPoint(pMusicPlc->muteFactor, pMusicPlc->decaySamples - pMusicPlc->decaySamplesNow, pMusicPlc->decaySamples);
#else
					i32 num = pMusicPlc->decaySamples - pMusicPlc->decaySamplesNow;
					pMusicPlc->funcList->AppendInFixpoint(
						pMusicPlc->muteFactor.GetBufInSample(i),
						(u8*)&num,
						(u8*)&(pMusicPlc->decaySamples),
						1,
						pMusicPlc->muteFactor._info->_channels,
						pMusicPlc->muteFactor.GetFPNum());
					pMusicPlc->muteFactor.Append(1);
#endif
				}
			}
			else {
				muteMode = MUTE_MODE_DOMUTE_ZERO;
			}
			//infuture
			for (i32 i = 0; i < pMusicPlc->infuture.GetSamplesMax(); i++) {
				pMusicPlc->infuture.Append(pMusicPlc->fillSignal, pMusicPlc->fillSignalSampleIndex, 1);
				pMusicPlc->fillSignalSampleIndex = (pMusicPlc->fillSignalSampleIndex + 1) % pMusicPlc->fillSignal.GetValidSamples();
			}
		}
		//muting
#if 1
		if (muteMode != MUTE_MODE_NOMUTE) {
			if (muteMode == MUTE_MODE_DOMUTE) {
				pMusicPlc->funcList->product(
					pMusicPlc->infuture.GetBufInSample(0),
					pMusicPlc->muteFactor.GetBufInSample(0),
					pMusicPlc->frameSamples,
					pMusicPlc->infuture._info->_channels,
					pMusicPlc->muteFactor.GetFPNum());
			}
			else {
				ALGO_MEM_SET(pMusicPlc->infuture.GetBufInSample(0), 0, pMusicPlc->infuture.GetSizeMax());
			}
		}
#endif
		//out
		pOut.Append(pMusicPlc->inHistory, pMusicPlc->inHistory.GetSamplesMax() - pMusicPlc->overlapInSamples, pMusicPlc->overlapInSamples);
		pOut.Append(pMusicPlc->infuture, 0, pMusicPlc->frameSamples - pMusicPlc->overlapInSamples);

		//shift
		pMusicPlc->inHistory.Clear(pMusicPlc->frameSamples);
		pMusicPlc->inHistory.Append(pMusicPlc->infuture, 0, pMusicPlc->frameSamples);
		pMusicPlc->infuture.Clear(pMusicPlc->infuture.GetValidSamples());

		*outLen = pOut.GetValidSamples() * pMusicPlc->info._bytesPerSample;
		pMusicPlc->muteFactor.Clear(pMusicPlc->muteFactor.GetValidSamples());
		return APERET_SUCCESS;
	}

	int32_t Ape_StateDeInit(void* pMusicPlcStateIn)
	{
		if (!pMusicPlcStateIn)
			return APERET_FAIL;
		MusicPlcState* pMusicPlc = (MusicPlcState*)pMusicPlcStateIn;
		pMusicPlc->basePorting->Free(pMusicPlc->inHistory.GetBufInSample(0));
		pMusicPlc->basePorting->Free(pMusicPlc->infuture.GetBufInSample(0));
		pMusicPlc->basePorting->Free(pMusicPlc->fillSignal.GetBufInSample(0));
		pMusicPlc->basePorting->Free(pMusicPlc->muteFactor.GetBufInSample(0));
		return APERET_SUCCESS;
	}
}

#endif