/***************************************************************************
 *
 * Copyright 2015-2019 BES.
 * All rights reserved. All unpublished rights reserved.
 *
 * No part of this work may be used or reproduced in any form or by any
 * means, or stored in a database or retrieval system, without prior written
 * permission of BES.
 *
 * Use of this work is governed by a license granted by BES.
 * This work contains confidential and proprietary information of
 * BES. which is protected by copyright, trade secret,
 * trademark and other intellectual property rights.
 *
 ****************************************************************************/
/////////////////////////////////////////////////////////////////////////
//Packets Loss Concealment                                             //
/////////////////////////////////////////////////////////////////////////
#include "plc_8000.h"
#include "string.h"

#ifdef __arm__

#ifdef __ICCARM__
#include <intrinsics.h>
#else

#include "cmsis.h"
#include "hal_location.h"
#endif

#endif // !WIN32


//quality set
//#define FINE_PLC_PITCH_SEARCH


//for mac using int32 or int64
#define DATA_FOR_FIND_PITCH_SHIFT (3)


#define FIXED_Q (1<<15)
#define FIXED_SQRT_STEP 16

//basic set
/* minimum allowed pitch, 200 Hz */
#define PITCH_MIN 40 

/* maximum allowed pitch, 66 Hz */
#define PITCH_MAX 120 

#define PITCHDIFF (PITCH_MAX - PITCH_MIN)

/* maximum pitch OLA window */
#define POVERLAPMAX (PITCH_MAX >> 2)

/* history buffer length*/
#define HISTORYLEN (PITCH_MAX * 3 + POVERLAPMAX) 

/* 2:1 decimation */
#define NDEC 2 

/* 20 ms correlation length */
#define CORRLEN 160 

/* correlation buffer length */
#define CORRBUFLEN (CORRLEN + PITCH_MAX) 

/* minimum power */
#define CORRMINPOWER (250) 

/* end OLA increment per frame, 4 ms */
#define EOVERLAPINCR (32) 

/* 10 ms at 8 KHz */
#define FRAMESZ 80 

/* attenuation factor per 10 ms frame */
#define ATTENFAC ((short)(0.2*FIXED_Q)) 


/* attenuation per sample */
#define ATTENINCR (ATTENFAC/FRAMESZ)


struct PlcSt_8000_
{
	int LaseOverlen;

	int LaseOverTatollen;

	int LaseEraseCntSample;

	/* consecutive erased samples */
	int OverCntSample;

	/* consecutive erased samples */
	int EraseCntSample;

	/* overlap based on pitch */
	int poverlap;

	/* offset into pitch period */
	int poffset;

	/* pitch estimate */
	int pitch;

	/* current pitch buffer length */
	int pitchblen;

	/* end of pitch buffer */
	short *pitchbufend;

	/* start of pitch buffer */
	short *pitchbufstart;

	/* buffer for cycles of speech */
	short pitchbuf[HISTORYLEN];

	/* saved last quarter wavelength */
	short lastq[POVERLAPMAX];

	/* history buffer */
	short history[HISTORYLEN];

	/* tail of previous pitch estimate */
	short OverBuf[POVERLAPMAX];

	short LastOverbuf[FRAMESZ];
};

static void ScaleSpeech(PlcSt_8000 *lc, short *out, int num);
static void GetFeSpeech(PlcSt_8000 *lc, short *out, int sz);
static void SaveSpeech(PlcSt_8000 *lc, short *s, int num);


static int FindPitch(PlcSt_8000 *lc);

static void FOverLapAdd(short *l, short *r, short *o, int cnt);
static void SOverLapAdd(short *l, short *r, short *o, int totalcnt, int newcnt, int oldcnt);
static void OverLapAddAtEnd(PlcSt_8000 *lc, short *s, short *f, int totalcnt, int newcnt, int oldcnt);

static void CopySample(short *f, short *t, int cnt);
static void Zeros(short *s, int cnt);


#if 0
int fix_sqrt(__int64 x,int Fixstep)
{
	int bit = 7;
	int i,z,Sn;
	__int64 Xn,Yn,Xn1,Yn1;
	__int64 ComHalf = 1;
	x = x*64;
	while(x >= (ComHalf<<bit))
		bit = bit + 1;
	bit = bit -1;
	if( bit&0x1)
		bit = bit+1;
	if(x == (ComHalf<<bit))
		z = ComHalf<<(bit/2-3);
	else
	{
		Xn = x + (ComHalf<<bit);
		Yn = x - (ComHalf<<bit);
		if(Yn == 0)
			Sn = 0;
		else if(Yn>0)
			Sn = -1;
		else
			Sn = 1;
		for (i = 1;i<=Fixstep;i++)
		{
			Xn1 = Xn + ((Sn*Yn)>>i);
			Yn1 = Yn + ((Sn*Xn)>>i);
			Xn = Xn1;
			Yn = Yn1;
			if(Yn == 0)
				Sn = 0;
			else if(Yn>0)
				Sn = -1;
			else
				Sn = 1;
		}
		Xn1 = Xn1*1234;
		z = Xn1>>(11+bit/2+2);
		if(z&0x1)
			z = (z>>1) + 1;
		else
			z = z>>1;
	}
	return z;
}
#else

#if !defined(__arm__)
short fix_sqrt(long long  a,int Fixstep)
{


	int rem = 0;
	int root = 0;
	int divisor = 0;
	int i;
	for( i=0; i<16; i++)
	{
		root <<= 1;
		rem = (int)((rem << 2) + (a >> 30));
		a <<= 2;
		divisor = (root<<1) + 1;
		if(divisor <= rem){
			rem -= divisor;
			root++;
		}
	}
	return (short)(root);
}
#else
static inline int fix_sqrt(int op1,int Fixstep)
{
  int result;
  asm volatile("vmov s0, %0" : :"r" (op1));
  asm volatile("vcvt.f32.s32 s0,s0");
  asm volatile("vsqrt.f32 s0,s0");
  asm volatile("vcvt.s32.f32 s0,s0");
  asm volatile("vmov %0, s0" : "=r" (result));
  return(result);
}

#endif
#endif


static void CopySample(short *f, short *t, int cnt)
{
	int i;
	for (i = 0; i < cnt; i++)
	{
		t[i] = f[i];
	}
}

static void Zeros(short *s, int cnt)
{
	int i;
	for (i = 0; i < cnt; i++)
	{
		s[i] = 0;
	}
}

void PlcInit(PlcSt_8000 *lc)
{
	lc->EraseCntSample = 0;
	lc->pitchbufend = &lc->pitchbuf[HISTORYLEN];
	Zeros(lc->history, HISTORYLEN);
}



/*
* Save all samples.
*/
static void SaveSpeech(PlcSt_8000 *lc,short *s, int num)
{
	/* make room for new signal */
	CopySample(&lc->history[num], lc->history, HISTORYLEN - num);
	/* copy in the new frame */
	CopySample(s, &lc->history[HISTORYLEN - num], num);
	/* copy out the delayed frame */
	CopySample(&lc->history[HISTORYLEN - num - POVERLAPMAX], s, num);
}




/*
* A good frame was received and decoded.
* If right after an erasure, do an overlap add with the synthetic signal.
* Add the frame to lc->history buffer.
*/

void AddToHistory(PlcSt_8000 *lc, short *s, int num)
{
	int numsample=num;

	if (lc->EraseCntSample) {

		lc->LaseEraseCntSample = lc->EraseCntSample;

		/*
		* longer erasures require longer overlaps
		* to smooth the transition between the synthetic
		* and real signal.
		*/
		lc->LaseOverTatollen = lc->poverlap + (lc->EraseCntSample / FRAMESZ+1) * EOVERLAPINCR;
		if (lc->LaseOverTatollen > FRAMESZ)
			lc->LaseOverTatollen = FRAMESZ;

		//lc->LaseOverTatollen = FRAMESZ*2;

		lc->LaseOverlen = lc->LaseOverTatollen;

		if (numsample > lc->LaseOverlen)
		{
			numsample = lc->LaseOverlen;
		}

		GetFeSpeech(lc,lc->LastOverbuf, numsample);
		OverLapAddAtEnd(lc,s, lc->LastOverbuf, lc->LaseOverTatollen, numsample, 0);
		lc->LaseOverlen = lc->LaseOverlen - numsample;

		lc->EraseCntSample = 0;

	} else if (lc->LaseOverlen){

		if (numsample > lc->LaseOverlen)
		{
			numsample = lc->LaseOverlen;
		}

		GetFeSpeech(lc,lc->LastOverbuf, numsample);
		OverLapAddAtEnd(lc,s, lc->LastOverbuf, lc->LaseOverTatollen, numsample, lc->LaseOverTatollen-lc->LaseOverlen);
		lc->LaseOverlen = lc->LaseOverlen - numsample;

	}


	SaveSpeech(lc,s,num);
}



/*
* Generate the synthetic signal.
* At the beginning of an erasure determine the lc->pitch, and extract
* one lc->pitch period from the tail of the signal. Do an OLA for 1/4
* of the lc->pitch to smooth the signal. Then repeat the extracted signal
* for the length of the erasure. If the erasure continues for more than
* 10 ms, increase the number of periods in the pitchbuffer. At the end
* of an erasure, do an OLA with the start of the first good frame.
* The gain decays as the erasure gets longer.
*/
void Dofe(PlcSt_8000 *lc, short *out, int num)
{
	int SampleNum = num;
	int AvailSampleNum = 0;
	short *OutP = out;


	if (SampleNum>0 && lc->EraseCntSample == 0)
	{
		CopySample(lc->history, lc->pitchbuf, HISTORYLEN); /* get lc->history */
		lc->pitch = FindPitch(lc); /* find lc->pitch */
		lc->poverlap = lc->pitch >> 2; /* OLA 1/4 wavelength */
		/* save original last lc->poverlap samples */
		CopySample(lc->pitchbufend - lc->poverlap, lc->lastq, lc->poverlap);
		lc->poffset = 0; /* create lc->pitch buffer with 1 period */
		lc->pitchblen = lc->pitch;
		lc->pitchbufstart = lc->pitchbufend - lc->pitchblen;
		FOverLapAdd(lc->lastq, lc->pitchbufstart - lc->poverlap,
			lc->pitchbufend - lc->poverlap, lc->poverlap);
		/* update last 1/4 wavelength in lc->history buffer */
		CopySample(lc->pitchbufend - lc->poverlap, &lc->history[HISTORYLEN - lc->poverlap],
			lc->poverlap);

		AvailSampleNum = SampleNum;

		if (AvailSampleNum > FRAMESZ)
		{
			AvailSampleNum = FRAMESZ;
		}

		GetFeSpeech(lc,OutP, AvailSampleNum); /* get synthesized speech */
		OutP = OutP + AvailSampleNum;
		lc->EraseCntSample = lc->EraseCntSample + AvailSampleNum;
		SampleNum = SampleNum - AvailSampleNum;
	}



	if (SampleNum>0 && lc->EraseCntSample > 0 && lc->EraseCntSample <=FRAMESZ)
	{
		AvailSampleNum = SampleNum;

		if (AvailSampleNum > FRAMESZ - lc->EraseCntSample)
		{
			AvailSampleNum = FRAMESZ - lc->EraseCntSample;
		}

		GetFeSpeech(lc,OutP, AvailSampleNum); /* get synthesized speech */
		OutP = OutP + AvailSampleNum;
		lc->EraseCntSample = lc->EraseCntSample + AvailSampleNum;
		SampleNum = SampleNum - AvailSampleNum;

	}



	if (SampleNum > 0 && (lc->EraseCntSample == FRAMESZ))
	{

		int saveoffset = lc->poffset; /* save offset for OLA */
		lc->OverCntSample = 0;
		GetFeSpeech(lc,lc->OverBuf, lc->poverlap); /* continue with old lc->pitchbuf */
		/* add periods to the lc->pitch buffer */
		lc->poffset = saveoffset;
		while (lc->poffset > lc->pitch)
			lc->poffset -= lc->pitch;
		lc->pitchblen += lc->pitch; /* add a period */
		lc->pitchbufstart = lc->pitchbufend - lc->pitchblen;
		FOverLapAdd(lc->lastq, lc->pitchbufstart - lc->poverlap,
			lc->pitchbufend - lc->poverlap, lc->poverlap);

		/* overlap add old lc->pitch buffer with new */

		AvailSampleNum = SampleNum;

		if (AvailSampleNum > FRAMESZ)
		{
			AvailSampleNum = FRAMESZ;
		}



		GetFeSpeech(lc,OutP, AvailSampleNum); /* get synthesized speech */

		if (AvailSampleNum>=lc->poverlap)
		{
			SOverLapAdd(lc->OverBuf, OutP, OutP, lc->poverlap, lc->poverlap, 0);
		}
		else
		{
			SOverLapAdd(lc->OverBuf, OutP, OutP, lc->poverlap, AvailSampleNum, 0);
			lc->OverCntSample = AvailSampleNum;
		}

		ScaleSpeech(lc,OutP, AvailSampleNum);

		OutP = OutP + AvailSampleNum;
		lc->EraseCntSample = lc->EraseCntSample + AvailSampleNum;
		SampleNum = SampleNum - AvailSampleNum;
	}


	if (SampleNum > 0 && (lc->EraseCntSample > FRAMESZ &&lc->EraseCntSample<FRAMESZ + lc->poverlap))
	{


		/* overlap add old lc->pitch buffer with new */

		AvailSampleNum = SampleNum;

		if (AvailSampleNum > FRAMESZ + lc->poverlap)
		{
			AvailSampleNum = FRAMESZ + lc->poverlap;
		}

		GetFeSpeech(lc,OutP, AvailSampleNum); /* get synthesized speech */

		if (AvailSampleNum >= lc->poverlap - lc->OverCntSample)
		{
			SOverLapAdd(lc->OverBuf + lc->OverCntSample, OutP, OutP, lc->poverlap, lc->poverlap - lc->OverCntSample, lc->OverCntSample);
			lc->OverCntSample = 0;
		}
		else
		{
			SOverLapAdd(lc->OverBuf + lc->OverCntSample, OutP, OutP, lc->poverlap, AvailSampleNum, lc->OverCntSample);
			lc->OverCntSample = lc->OverCntSample + AvailSampleNum;
		}

		ScaleSpeech(lc,OutP, AvailSampleNum);

		OutP = OutP + AvailSampleNum;
		lc->EraseCntSample = lc->EraseCntSample + AvailSampleNum;
		SampleNum = SampleNum - AvailSampleNum;
	}


	if (SampleNum > 0 && lc->EraseCntSample >= FRAMESZ + lc->poverlap && lc->EraseCntSample <FRAMESZ * 2)
	{
		AvailSampleNum = SampleNum;

		if (AvailSampleNum > 2*FRAMESZ - lc->EraseCntSample)
		{
			AvailSampleNum = 2*FRAMESZ - lc->EraseCntSample;
		}

		GetFeSpeech(lc,OutP, AvailSampleNum); /* get synthesized speech */
		ScaleSpeech(lc,OutP, AvailSampleNum);

		OutP = OutP + AvailSampleNum;
		lc->EraseCntSample = lc->EraseCntSample + AvailSampleNum;
		SampleNum = SampleNum - AvailSampleNum;
	}



	if (SampleNum > 0 && (lc->EraseCntSample == FRAMESZ * 2))
	{
		/* tail of previous lc->pitch estimate */
		int saveoffset = lc->poffset; /* save offset for OLA */
		lc->OverCntSample = 0;
		GetFeSpeech(lc,lc->OverBuf, lc->poverlap); /* continue with old lc->pitchbuf */
		/* add periods to the lc->pitch buffer */
		lc->poffset = saveoffset;
		while (lc->poffset > lc->pitch)
			lc->poffset -= lc->pitch;
		lc->pitchblen += lc->pitch; /* add a period */
		lc->pitchbufstart = lc->pitchbufend - lc->pitchblen;
		FOverLapAdd(lc->lastq, lc->pitchbufstart - lc->poverlap,
			lc->pitchbufend - lc->poverlap, lc->poverlap);

		/* overlap add old lc->pitch buffer with new */

		AvailSampleNum = SampleNum;

		if (AvailSampleNum > FRAMESZ)
		{
			AvailSampleNum = FRAMESZ;
		}

		GetFeSpeech(lc,OutP, AvailSampleNum); /* get synthesized speech */


		if (AvailSampleNum >= lc->poverlap)
		{
			SOverLapAdd(lc->OverBuf, OutP, OutP, lc->poverlap, lc->poverlap, 0);
		}
		else
		{
			SOverLapAdd(lc->OverBuf, OutP, OutP, lc->poverlap, AvailSampleNum, 0);
			lc->OverCntSample = AvailSampleNum;
		}

		ScaleSpeech(lc,OutP, AvailSampleNum);

		OutP = OutP + AvailSampleNum;
		lc->EraseCntSample = lc->EraseCntSample + AvailSampleNum;
		SampleNum = SampleNum - AvailSampleNum;
	}



	if (SampleNum > 0 && (lc->EraseCntSample > FRAMESZ*2 &&lc->EraseCntSample < FRAMESZ*2 + lc->poverlap))
	{

		/* overlap add old lc->pitch buffer with new */

		AvailSampleNum = SampleNum;

		if (AvailSampleNum > FRAMESZ)
		{
			AvailSampleNum = FRAMESZ;
		}

		GetFeSpeech(lc,OutP, AvailSampleNum); /* get synthesized speech */

		if (AvailSampleNum >= lc->poverlap - lc->OverCntSample)
		{
			SOverLapAdd(lc->OverBuf + lc->OverCntSample, OutP, OutP, lc->poverlap, lc->poverlap - lc->OverCntSample, lc->OverCntSample);
			lc->OverCntSample = 0;
		}
		else
		{
			SOverLapAdd(lc->OverBuf + lc->OverCntSample, OutP, OutP, lc->poverlap, AvailSampleNum, lc->OverCntSample);
			lc->OverCntSample = lc->OverCntSample + AvailSampleNum;
		}

		ScaleSpeech(lc,OutP, AvailSampleNum);

		OutP = OutP + AvailSampleNum;
		lc->EraseCntSample = lc->EraseCntSample + AvailSampleNum;
		SampleNum = SampleNum - AvailSampleNum;
	}

	if (SampleNum > 0 && lc->EraseCntSample >= 2 * FRAMESZ + lc->poverlap && lc->EraseCntSample <FRAMESZ * 6)
	{
		AvailSampleNum = SampleNum;

		if (AvailSampleNum > 6*FRAMESZ - lc->EraseCntSample)
		{
			AvailSampleNum = 6*FRAMESZ - lc->EraseCntSample;
		}

		GetFeSpeech(lc,OutP, AvailSampleNum); /* get synthesized speech */
		ScaleSpeech(lc,OutP, AvailSampleNum);

		OutP = OutP + AvailSampleNum;
		lc->EraseCntSample = lc->EraseCntSample + AvailSampleNum;
		SampleNum = SampleNum - AvailSampleNum;
	}


	if (SampleNum > 0 && (lc->EraseCntSample == FRAMESZ *6))
	{

		lc->OverCntSample = 0;

		GetFeSpeech(lc,lc->OverBuf, lc->poverlap); /* continue with old lc->pitchbuf */

		//Zeros(lc->pitchbuf, HISTORYLEN);
		AvailSampleNum = SampleNum;

		if (AvailSampleNum > FRAMESZ)
		{
			AvailSampleNum = FRAMESZ;
		}

		Zeros(OutP, AvailSampleNum);

		if (AvailSampleNum >= lc->poverlap)
		{
			SOverLapAdd(lc->OverBuf, OutP, OutP, lc->poverlap, lc->poverlap, 0);
		}
		else
		{
			SOverLapAdd(lc->OverBuf, OutP, OutP, lc->poverlap, AvailSampleNum, 0);
			lc->OverCntSample = AvailSampleNum;
		}

		ScaleSpeech(lc,OutP, AvailSampleNum);

		OutP = OutP + AvailSampleNum;
		lc->EraseCntSample = lc->EraseCntSample + AvailSampleNum;
		SampleNum = SampleNum - AvailSampleNum;

		Zeros(lc->pitchbuf, HISTORYLEN);
	}



	if (SampleNum > 0 && (lc->EraseCntSample > FRAMESZ * 6 && lc->EraseCntSample < FRAMESZ * 6 + lc->poverlap))
	{

		/* overlap add old lc->pitch buffer with new */

		AvailSampleNum = SampleNum;

		if (AvailSampleNum > FRAMESZ)
		{
			AvailSampleNum = FRAMESZ;
		}

		GetFeSpeech(lc,OutP, AvailSampleNum); /* get synthesized speech */

		if (AvailSampleNum >= lc->poverlap - lc->OverCntSample)
		{
			SOverLapAdd(lc->OverBuf + lc->OverCntSample, OutP, OutP, lc->poverlap, lc->poverlap - lc->OverCntSample, lc->OverCntSample);
			lc->OverCntSample = 0;
		}
		else
		{
			SOverLapAdd(lc->OverBuf + lc->OverCntSample, OutP, OutP, lc->poverlap, AvailSampleNum, lc->OverCntSample);
			lc->OverCntSample = lc->OverCntSample + AvailSampleNum;
		}

		ScaleSpeech(lc,OutP, AvailSampleNum);

		OutP = OutP + AvailSampleNum;
		lc->EraseCntSample = lc->EraseCntSample + AvailSampleNum;
		SampleNum = SampleNum - AvailSampleNum;
	}


	if (SampleNum > 0 && (lc->EraseCntSample >= FRAMESZ * 6 + lc->poverlap))
	{
		AvailSampleNum = SampleNum;


		GetFeSpeech(lc,OutP, AvailSampleNum); /* get synthesized speech */
		Zeros(OutP, AvailSampleNum);
		ScaleSpeech(lc,OutP, AvailSampleNum);

		OutP = OutP + AvailSampleNum;
		lc->EraseCntSample = lc->EraseCntSample + AvailSampleNum;
		SampleNum = SampleNum - AvailSampleNum;
	}

	SaveSpeech(lc,out, num);
}



/*
* Estimate the lc->pitch.
* l - pointer to first sample in last 20 ms of speech.
* r - points to the sample PITCH_MAX before l
*/
static int FindPitch(PlcSt_8000 *lc)
{
	int i, j;

#ifdef FINE_PLC_PITCH_SEARCH

	int k;

#endif
	int bestmatch;
	int bestcorr;
	int corr; /* correlation */
        long long corr64; /* correlation */
	int energy; /* running energy */
	int scale; /* scale correlation by average power */
	short *rp; /* segment to match */
	short *l = lc->pitchbufend - CORRLEN;
	short *r = lc->pitchbufend - CORRBUFLEN;
    int temp;
	/* coarse search */
	rp = r;
	energy = 0;
	corr = 0;
	for (i = 0; i < CORRLEN; i += NDEC) {
		energy += (rp[i]>>DATA_FOR_FIND_PITCH_SHIFT) * (rp[i]>>DATA_FOR_FIND_PITCH_SHIFT);
		corr += (rp[i]>>DATA_FOR_FIND_PITCH_SHIFT) * (l[i]>>DATA_FOR_FIND_PITCH_SHIFT);
	}
	scale = energy;
	if (scale < (CORRMINPOWER>>DATA_FOR_FIND_PITCH_SHIFT))
		scale = CORRMINPOWER>>DATA_FOR_FIND_PITCH_SHIFT;
	temp = fix_sqrt(scale,FIXED_SQRT_STEP);
	corr = corr / temp;
	bestcorr = corr;
	bestmatch = 0;
	for (j = NDEC; j <= PITCHDIFF; j += NDEC) {
		energy -= (rp[i]>>DATA_FOR_FIND_PITCH_SHIFT) * (rp[i]>>DATA_FOR_FIND_PITCH_SHIFT);
		energy += (rp[CORRLEN]>>DATA_FOR_FIND_PITCH_SHIFT) * (rp[CORRLEN]>>DATA_FOR_FIND_PITCH_SHIFT);
		rp += NDEC;
		corr64 = 0;

		for (i = 0; i < CORRLEN; i += NDEC)
                {
                	corr64 += ((long long)rp[i]) * (l[i]);

                }

		corr = corr64 >> (DATA_FOR_FIND_PITCH_SHIFT*2);
		scale = energy;
		if (scale < (CORRMINPOWER>>DATA_FOR_FIND_PITCH_SHIFT))
			scale = (CORRMINPOWER>>DATA_FOR_FIND_PITCH_SHIFT);
		temp = fix_sqrt(scale,FIXED_SQRT_STEP);
		corr = corr / temp;
		if (corr >= bestcorr) {
			bestcorr = corr;
			bestmatch = j;
		}
	}
	/* fine search */
#ifdef FINE_PLC_PITCH_SEARCH
	j = bestmatch - (NDEC - 1);
	if (j < 0)
		j = 0;
	k = bestmatch + (NDEC - 1);
	if (k > PITCHDIFF)
		k = PITCHDIFF;
	rp = &r[j];
	energy = 0.f;
	corr = 0.f;
	for (i = 0; i < CORRLEN; i++) {
		energy += rp[i] * rp[i];
		corr += rp[i] * l[i];
	}
	scale = energy;
	if (scale < CORRMINPOWER)
		scale = CORRMINPOWER;
	temp = fix_sqrt(scale,FIXED_SQRT_STEP);
	corr = (corr*FIXED_Q) / temp;
	bestcorr = corr;
	bestmatch = j;
	for (j++; j <= k; j++) {
		energy -= rp[0] * rp[0];
		energy += rp[CORRLEN] * rp[CORRLEN];
		rp++;
		corr = 0;
		for (i = 0; i < CORRLEN; i++)
			corr += rp[i] * l[i];
		scale = energy;
		if (scale < CORRMINPOWER)
			scale = CORRMINPOWER;
		temp = fix_sqrt(scale,FIXED_SQRT_STEP);
		corr = (corr*FIXED_Q) / temp;
		if (corr > bestcorr) {
			bestcorr = corr;
			bestmatch = j;
		}
	}
#endif

	return PITCH_MAX - bestmatch;
}

/*
* Get samples from the circular lc->pitch buffer. Update lc->poffset so
* when subsequent frames are erased the signal continues.
*/
static void GetFeSpeech(PlcSt_8000 *lc, short *out, int sz)
{
	while (sz) {
		int cnt = lc->pitchblen - lc->poffset;
		if (cnt > sz)
			cnt = sz;
		CopySample(&lc->pitchbufstart[lc->poffset], out, cnt);
		lc->poffset += cnt;
		if (lc->poffset == lc->pitchblen)
			lc->poffset = 0;
		out += cnt;
		sz -= cnt;
	}
}



static void ScaleSpeech(PlcSt_8000 *lc, short *out, int num)
{

	int i;
	int g;

	g = 1 * FIXED_Q - (lc->EraseCntSample / FRAMESZ-1) * ATTENFAC - (lc->EraseCntSample % FRAMESZ)*ATTENINCR;


	for (i = 0; i < num; i++) {

		if (g<0)
		{
			g = 0;
		}
		out[i] = (short)((out[i] * g) / FIXED_Q);
		g -= ATTENINCR;
	}



	return;

 }

/*
* Overlap add left and right sides
*/

static void FOverLapAdd(short *l, short *r, short *o, int cnt)
{
	int i;
	int incr = FIXED_Q / cnt;
	int lw = FIXED_Q - incr;
	int rw = incr;

	for (i = 0; i < cnt; i++) {
		int t = (lw * l[i]+ rw * r[i])/FIXED_Q;
		if (t > 32767)
			t = 32767;
		else if (t < -32768)
			t = -32768;
		o[i] = t;
		lw -= incr;
		rw += incr;
	}
}



//void SOverLapAdd(short *l, short *r, short *o, int overnum,int newnum, int oldnum)
static void SOverLapAdd(short *l, short *r, short *o, int totalcnt, int newcnt, int oldcnt)
{
	int i;
	int incr = FIXED_Q / totalcnt;
	int lw = FIXED_Q - incr - oldcnt*incr;
	int rw = incr + oldcnt*incr;

	for (i = 0; i < newcnt; i++) {
		int t = (lw * l[i] + rw * r[i]) / FIXED_Q;
		if (t > 32767)
			t = 32767;
		else if (t < -32768)
			t = -32768;
		o[i] = (short)t;
		lw -= incr;
		rw += incr;
	}
}




/*
* Overlap add the end of the erasure with the start of the first good frame
* Scale the synthetic speech by the gain factor before the OLA.
*/
static void OverLapAddAtEnd(PlcSt_8000 *lc, short *s, short *f, int totalcnt, int newcnt, int oldcnt)
{
	int i;
	int incr;
	int gain;
	int incrg;
	int lw;
	int rw;

	incr = FIXED_Q / totalcnt;


	if (lc->LaseEraseCntSample >= FRAMESZ)
	{
		gain = 1 * FIXED_Q - (lc->LaseEraseCntSample / FRAMESZ - 1) * ATTENFAC - (lc->LaseEraseCntSample % FRAMESZ)*ATTENINCR;
	}
	else
	{
		gain = 1 * FIXED_Q;
	}

	if (gain < 0)
		gain = 0;

	incrg = (incr * gain) / FIXED_Q;
	lw = ((1 * FIXED_Q - incr) * gain) / FIXED_Q - incrg*oldcnt;
	rw = incr + incr*oldcnt;
	for (i = 0; i < newcnt; i++) {
		int t = (lw * f[i] + rw * s[i]) / FIXED_Q;

		if (t > 32767)
			t = 32767;
		else if (t < -32768)
			t = -32768;
		s[i] = (short)t;
		lw -= incrg;
		rw += incr;
	}



}

#define SUCESSZERO 4
#define NUMOFSAMPLE 120
static short *PcmInBuf;

PlcSt_8000 *speech_plc_8000_init(void* (* speex_alloc_ext)(int))
{
    PlcSt_8000 *plc_st;

    plc_st = (PlcSt_8000 *)speex_alloc_ext(sizeof(PlcSt_8000));
    PcmInBuf = (short *)speex_alloc_ext((NUMOFSAMPLE + SUCESSZERO - 1)*2);
    PlcInit(plc_st);
    return plc_st;
}

int speech_plc_8000(PlcSt_8000 *lc, short *InBuf, int len)
{
    static int ErrorStatusFlag = 1;

     int NumSucess = 0;
     int FirstZeroFlag = 0;
     int FirstNoZeroFlag = 0;
     int FirstZeroIndex = 0;
     int FirstNoZeroIndex = 0;
     int i;

    memcpy(PcmInBuf + (SUCESSZERO-1),InBuf,len);

    for (i = 0; i < (len/2); i++)
    {
        if (PcmInBuf[i] == (short)0x0000)
        {

            NumSucess++;

            if (NumSucess >= SUCESSZERO && FirstZeroFlag == 0)
            {
                FirstZeroIndex = i - (SUCESSZERO-1);
                FirstZeroFlag = 1;
                FirstNoZeroFlag = 0;
                if (ErrorStatusFlag == 1)
                {
                    AddToHistory(lc, &PcmInBuf[FirstNoZeroIndex],FirstZeroIndex  - FirstNoZeroIndex);
                }
                ErrorStatusFlag = 0;
            }

        }
        else
        {
            if (FirstNoZeroFlag == 0)
            {
                if (NumSucess >= SUCESSZERO || ErrorStatusFlag == 0)
                {
                    Dofe(lc, &PcmInBuf[FirstZeroIndex], i - FirstZeroIndex);
                    FirstNoZeroIndex = i;
                }
                else
                {
                    FirstNoZeroIndex = i - NumSucess;
                }

                FirstNoZeroFlag = 1;
            }

            ErrorStatusFlag = 1;
            NumSucess = 0;
            FirstZeroFlag = 0;

        }


    }

    if (ErrorStatusFlag == 1)
    {

        if ((NumSucess == 1 && (PcmInBuf[i] == 0) && (PcmInBuf[i + 1] == 0) && (PcmInBuf[i + 2] == 0))
            || (NumSucess == 2 && (PcmInBuf[i] == 0) && (PcmInBuf[i+1] == 0))
            || (NumSucess == 3 && (PcmInBuf[i] == 0) ))
        {
            AddToHistory(lc, &PcmInBuf[FirstNoZeroIndex], i - FirstNoZeroIndex-NumSucess);
            Dofe(lc, &PcmInBuf[i - NumSucess], NumSucess);
            ErrorStatusFlag = 0;
        }
        else
        {
            AddToHistory(lc, &PcmInBuf[FirstNoZeroIndex], i - FirstNoZeroIndex);
        }


    }
    else
    {
        Dofe(lc, &PcmInBuf[FirstZeroIndex], i - FirstZeroIndex);
    }

    memcpy(InBuf, PcmInBuf , len);
    memcpy(PcmInBuf, PcmInBuf + len/2, (SUCESSZERO - 1)*2);

    return 0;
}

