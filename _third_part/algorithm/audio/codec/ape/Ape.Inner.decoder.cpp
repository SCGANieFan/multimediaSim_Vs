#include"Ape.Inner.decoder.h"


STATIC INLINE APE_RET_t ApeDecoderStartFrame(void* decoder, uint8_t* in, uint32_t inLen, uint32_t* inUsed)
{
	//check
#if 0
	if (inLen < 10)
		return APE_RET_NOT_ENOUGH_INPUT;
#endif
	uint8_t* pIn = in;
	ApeDecoder* pDec = (ApeDecoder*)decoder;
	ApeDescriptor* descriptor = pDec->context.GetDescriptor();
	ApeFrameInfo* frame = &pDec->frame;

	if (pDec->currentframe >= pDec->context.GetHeader()->totalframes)
		return APE_RET_FINISH;
	//nblocks
	frame->nblocks = pDec->context.GetHeader()->blocksperframe;
	if (pDec->currentframe == (pDec->context.GetHeader()->totalframes- 1)) {
		frame->nblocks = pDec->context.GetHeader()->finalframeblocks;
	}
	frame->skip = pDec->firstSkip;
	pDec->bufferRead.SetBufferIn(pIn, inLen);
	if (pDec->isNewFrameStart)
	{
		pDec->isNewFrameStart = false;
		pDec->bufferRead.Read(frame->skip);
	}

	pDec->CRC = pDec->bufferRead.Read(4);
	//ALGO_PRINT("[%d] CRC:%x", pDec->currentframe, pDec->CRC);
	pDec->frameFlags = 0;
	if ((descriptor->fileversion > 3820)) {
		if (pDec->CRC & 0x80000000)
		{
			pDec->frameFlags = pDec->bufferRead.Read(4);
		}
		pDec->CRC &= ~0x80000000;
	}
	//rice
	pDec->riceX.k = 10;
	pDec->riceX.ksum = 16384;//(1 << pDec->riceX.k) * 16;
	pDec->riceY.k = 10;
	pDec->riceY.ksum = 16384;//(1 << pDec->riceY.k) * 16;

	//range coder
	pDec->bufferRead.Read(1);
	pDec->rc.Init(pDec->bufferRead.Read(1));
	//NN filter
	pDec->NNFilter.Reset();
	//prediction
	pDec->prediction.Init();
	pDec->currentframe++;
	*inUsed = pDec->bufferRead.GetBufferUsed();

	return APE_RET_SUCCESS;
}

static APE_RET_t ApeDecoderEndFrame(void* decoder, uint8_t* in, uint32_t inSize, uint32_t* inUsed)
{
	//check
#if 0
	if (decoder == NULL
		|| in == NULL
		|| inSize < 1)
		return APE_RET_INPUT_ERROR;
#endif
	ApeDecoder* pDec = (ApeDecoder*)decoder;
	pDec->bufferRead.SetBufferIn(in, inSize);
	// normalize
	pDec->rc.normalize(&pDec->bufferRead);
	// used to back-pedal the last two bytes out
	// this should never have been a problem because we've outputted and normalized beforehand
	// but stopped doing it as of 3.96 in case it accounted for rare decompression failures
	if (pDec->context.GetFileVersion()<= 3950)
		pDec->bufferRead.Read(-2);
	if (inUsed)
		*inUsed = pDec->bufferRead.GetBufferUsed();
	return APE_RET_SUCCESS; // end of life!
}


static APE_RET_t mono_decoder_process(ApeDecoder* dec, uint16_t* blocksDecoded, void* bufferRead) {

	//0.check
	//
	//1.init	
	int32_t* ch0, * ch1;
	int32_t Y;

	ch0 = dec->ch0;
	ch1 = dec->ch1;
	if (dec->frameFlags & APE_FRAMECODE_STEREO_SILENCE)
	{
		*blocksDecoded = dec->blocksMax;
		memset(ch0, 0, sizeof(*ch0) * (*blocksDecoded));
		memset(ch1, 0, sizeof(*ch1) * (*blocksDecoded));
	}
	else
	{
		*blocksDecoded = 0;
		while (*blocksDecoded < dec->blocksMax)
		{
			//2.entropy decode			
			if (!dec->rc.EntropyDecode(&dec->riceY, &dec->bufferRead, &Y, dec->context.GetFileVersion()))
				return APE_RET_ENTROPY_DECODE_ERROR;

			//3.NN filter
			dec->NNFilter.DoFilter(0, &Y);
			//4.apply filter
			dec->prediction.ApePredictionFilterMono(&Y);
			*ch0++ = Y;
			(*blocksDecoded)++;
		}
		if (dec->context.GetChannels() == 2)
		{
			//APE_FRAMECODE_PSEUDO_STEREO
			memcpy(dec->ch1, dec->ch0, sizeof(*ch0) * (*blocksDecoded));
		}
	}
	return APE_RET_SUCCESS;
}

static APE_RET_t stereo_decoder_process(ApeDecoder* dec, uint16_t* blocksDecoded, void* bufferRead) {
	//1.init
	int32_t* ch0, * ch1;
	int32_t Y, X;
	int32_t L, R;

	ch0 = dec->ch0;
	ch1 = dec->ch1;
	if (dec->frameFlags & APE_FRAMECODE_STEREO_SILENCE)
	{
		*blocksDecoded = dec->blocksMax;
		memset(ch0, 0, sizeof(*ch0) * (*blocksDecoded));
		memset(ch1, 0, sizeof(*ch1) * (*blocksDecoded));
	}
	else
	{
		*blocksDecoded = 0;
		while (*blocksDecoded < dec->blocksMax)
		{
			//2.entropy decode
			if (!dec->rc.EntropyDecode(&dec->riceY, &dec->bufferRead, &Y, dec->context.GetFileVersion()))
				return APE_RET_ENTROPY_DECODE_ERROR;
			if (!dec->rc.EntropyDecode(&dec->riceX, &dec->bufferRead, &X, dec->context.GetFileVersion()))
				return APE_RET_ENTROPY_DECODE_ERROR;

			//3.NN filter
			dec->NNFilter.DoFilter(0, &Y);
			dec->NNFilter.DoFilter(1, &X);

			//4.apply filter
			dec->prediction.ApePredictionFilterStero(&Y, &X);
		
			//5.Y,X -> L,R
			L = X - (Y / 2);
			R = L + Y;
			*ch0++ = L;
			*ch1++ = R;

			(*blocksDecoded)++;
		}
	}
	return APE_RET_SUCCESS;
}

void ApeDecodeOut(uint8_t* dst, int32_t dstByte, int32_t* src0, int32_t* src1, int32_t srcValidByte, int32_t channel, int32_t loopNum)
{
	int32_t* pSrc0 = (int32_t*)src0;
	int32_t* pSrc1 = (int32_t*)src1;
	int16_t* pDst = (int16_t*)dst;
	i8* pDst8 = (i8*)dst;
	switch (srcValidByte)
	{
	case 1:
		if (channel == 1)
		{
			for (int32_t i = 0; i < loopNum; i++)
			{
				*pDst8++ = (i8)((*pSrc0++));
			}
		}
		else
		{

			for (int32_t i = 0; i < loopNum; i++)
			{
				*pDst8++ = (i8)((*pSrc0++));
				*pDst8++ = (i8)((*pSrc1++));
			}
		}
		break;
	case 2:
		if (channel == 1)
		{
			for (int32_t i = 0; i < loopNum; i++)
			{
				*pDst++ = (int16_t)(*pSrc0++);
			}
		}
		else
		{

			for (int32_t i = 0; i < loopNum; i++)
			{
				*pDst++ = (int16_t)(*pSrc0++);
				*pDst++ = (int16_t)(*pSrc1++);
			}
		}
		break;
	case 3:
		if (channel == 1)
		{
			for (int32_t i = 0; i < loopNum; i++)
			{
				*pDst++ = (int16_t)((*pSrc0++) >> 8);

			}
		}
		else
		{

			for (int32_t i = 0; i < loopNum; i++)
			{
				*pDst++ = (int16_t)((*pSrc0++) >> 8);
				*pDst++ = (int16_t)((*pSrc1++) >> 8);
			}
		}
		break;
	}
}

static APE_RET_t ApeDecodeBlocks(ApeDecoder* decoder, uint8_t* in, uint32_t inSize, uint32_t* inUsed, uint8_t* out, uint32_t* outSize)
{
	//check
	ApeDecoder* pDec = decoder;
	ApeHeader* header= pDec->context.GetHeader();
	APE_RET_t ret;
	pDec->bufferRead.SetBufferIn(in, inSize);
	if ((header->channels == 1)
		|| (pDec->frameFlags & APE_FRAMECODE_PSEUDO_STEREO))
	{
		ret = mono_decoder_process(pDec, &pDec->blocksDecoded, &pDec->bufferRead);
	}
	else
	{
		ret = stereo_decoder_process(pDec, &pDec->blocksDecoded, &pDec->bufferRead);
	}
	
	uint16_t sampleByteIn = header->bps >> 3;
	uint16_t sampleByteOut = header->bps >> 3;
	ApeDecodeOut(out, sampleByteOut, pDec->ch0, pDec->ch1, sampleByteIn, header->channels, pDec->blocksDecoded);
	*inUsed = pDec->bufferRead.GetBufferUsed();
	*outSize = header->channels * sampleByteOut * pDec->blocksDecoded;
	if (ret == APE_RET_ENTROPY_DECODE_ERROR)
	{
		return APE_RET_ENTROPY_DECODE_ERROR;
	}
	return APE_RET_SUCCESS;
}

static int num;
STATIC INLINE i32 Ape_Decode(ApeDecoder* pMusicPlcStateIn, uint8_t* in, int32_t inLen, uint8_t* out, int32_t* outLen)
{
	ApeDecoder* pDec = pMusicPlcStateIn;
	int32_t inOffset = 0;
	int32_t outSizeMax = *outLen;
	int32_t outOffset = 0;
	uint32_t bufferUsed = 0;

	//loop
	while (1)
	{
		//in cache
		i32 used;
		pDec->inCache.ClearUsed();
		pDec->inCache.AppendFully(in + inOffset, inLen - inOffset, &used);
		inOffset += used;
		if (pDec->haveInCache){
			if (pDec->inCache.GetLeftSize() > 0)
				break;
		}
		//start frame
		if (pDec->isFrameStart == true) {
			APE_RET_t startRet = ApeDecoderStartFrame(pDec, pDec->inCache.GetData(), pDec->inCache.GetSize(), &bufferUsed);
			if (startRet != APE_RET_SUCCESS)
				break;
			pDec->isFrameStart = false;
			pDec->inCache.Used(bufferUsed);
		}

		pDec->blocksMax = pDec->frame.nblocks - pDec->blocksUsed;
		pDec->blocksMax = MIN(pDec->blocksMax, APE_BLOCKS_MAX);
		if ((outOffset + pDec->blocksMax * 2 * pDec->context.GetChannels()) >= outSizeMax)
			break;

		uint32_t outLen = 0;
		APE_RET_t ret = ApeDecodeBlocks(pDec, pDec->inCache.GetData(), pDec->inCache.GetSize(), &bufferUsed, out + outOffset, &outLen);
		//printf("[%d] iSize:%d,bufferUsed:%d,outLenSum:%d,blocksUsedNow:%d,ret:%d\n",
		//	num++, pDec->inCache.GetSize(), bufferUsed, outOffset + outLen, pDec->blocksMax, ret);
#if 0
		if (num == 4754)
			int a = 1;
#endif

		if (outLen < 1){
			break;
		}
		int ssize = pDec->inCache.GetSize();
		pDec->inCache.Used(bufferUsed);
		if (pDec->inCache.GetSize() < 0)
			int a1 = 1;
		pDec->blocksUsed += pDec->blocksDecoded;
		outOffset += outLen;

		//end frame
		if (pDec->blocksUsed == pDec->frame.nblocks) {
			APE_RET_t endRet = ApeDecoderEndFrame(pDec, pDec->inCache.GetData(), pDec->inCache.GetSize(), &bufferUsed);
			pDec->inCache.Used(bufferUsed);
			if (endRet != APE_RET_SUCCESS)
			{
				break;
			}
			pDec->blocksUsed = 0;
			pDec->isFrameStart = true;
			if (pDec->currentframe == pDec->context.GetHeader()->totalframes)
			{
				break;;
			}
		}

	}
	*outLen = outOffset;
	return APE_RET_SUCCESS;
}

i32 ApeDecoder::Init(AlgoBasePorting* basePorting, ApeContext_t* contextIn, u32 startFrameNum, u32 skip)
{
	MM.Init(basePorting);
#if 1
	context.InitWithContext(contextIn);
#endif
	//clear NN Filter	
	fSet = context.GetCompressType() / 1000 - 1;
	NNFilter.Init(&MM, fSet, context.GetFileVersion());
	bufferRead.Init();
	Buffer buffer((u8*)MM.Malloc(3 * 1024), 3 * 1024);
	inCache.Init(&buffer);
	isFrameStart = true;
	haveInCache = true;
	isNewFrameStart = true;
	currentframe = startFrameNum - 1;
	firstSkip = skip;
	return APERET_SUCCESS;
}

i32 ApeDecoder::Run(uint8_t* in, int32_t inLen, uint8_t* out, int32_t* outLen)
{
	return Ape_Decode(this, in, inLen, out, outLen);
}

i32 ApeDecoder::DeInitInner()
{
	ALGO_PRINT();
	MM.FreeAll();
	return APERET_SUCCESS;
}
