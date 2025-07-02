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
/*************************************************************
SBC Example PLC ANSI-C Source Code
File: sbcplc.c
*************************************************************/
#include <math.h>
//#include "sbc.h"
#include "plc_16000.h"
#include "plat_types.h"
/* Local Function Prototypes */
static float CrossCorrelation(short *x, short *y);
static int PatternMatch(short *y);
static float AmplitudeMatch(short *y, short bestmatch);
/* Raised COSine table for OLA */

#if OLAL==16
static float rcos[OLAL] = { 0.99148655f, 0.96623611f, 0.92510857f, 0.86950446f,
0.80131732f, 0.72286918f, 0.63683150f, 0.54613418f,
0.45386582f, 0.36316850f, 0.27713082f, 0.19868268f,
0.13049554f, 0.07489143f, 0.03376389f, 0.00851345f };

#elif OLAL==60
static float rcos[OLAL] = {
	0.999337f,
	0.997350f,
	0.994044f,
	0.989428f,
	0.983514f,
	0.976318f,
	0.967858f,
	0.958158f,
	0.947244f,
	0.935143f,
	0.921888f,
	0.907514f,
	0.892060f,
	0.875566f,
	0.858076f,
	0.839637f,
	0.820297f,
	0.800107f,
	0.779122f,
	0.757396f,
	0.734988f,
	0.711957f,
	0.688364f,
	0.664271f,
	0.639743f,
	0.614844f,
	0.589640f,
	0.564199f,
	0.538588f,
	0.512874f,
	0.487126f,
	0.461412f,
	0.435801f,
	0.410360f,
	0.385156f,
	0.360257f,
	0.335729f,
	0.311636f,
	0.288043f,
	0.265012f,
	0.242604f,
	0.220878f,
	0.199893f,
	0.179703f,
	0.160363f,
	0.141924f,
	0.124434f,
	0.107940f,
	0.0924858f,
	0.0781122f,
	0.0648574f,
	0.0527565f,
	0.0418415f,
	0.0321416f,
	0.0236823f,
	0.0164861f,
	0.0105722f,
	0.00595605f,
	0.00265006f,
	0.000662955f,
};
#else

#endif

unsigned char indices0[] = { 0xad, 0x0, 0x0, 0xc5, 0x0, 0x0, 0x0, 0x0,
0x77, 0x6d, 0xb6, 0xdd, 0xdb, 0x6d, 0xb7,
0x76, 0xdb, 0x6d, 0xdd, 0xb6, 0xdb, 0x77,
0x6d, 0xb6, 0xdd, 0xdb, 0x6d, 0xb7, 0x76,
0xdb, 0x6d, 0xdd, 0xb6, 0xdb, 0x77, 0x6d,
0xb6, 0xdd, 0xdb, 0x6d, 0xb7, 0x76, 0xdb,
0x6d, 0xdd, 0xb6, 0xdb, 0x77, 0x6d, 0xb6,
0xdd, 0xdb, 0x6d, 0xb7, 0x76, 0xdb, 0x6c
};

#define abs(x)	((x)>0?x:-(x))

/*****************************************************************************
* Function: InitPLC() *
* Purpose: Perform PLC initialization of memory vectors. *
* Inputs: *plc_state - pointer to PLC state memory *
* Outputs: *plc_state - initialized memory. *
* Date: 03-18-2009
*****************************************************************************/
void InitPLC(struct PLC_State *plc_state)
{
	int i;
	plc_state->nbf = 0;
	plc_state->bestlag = 0;
	for (i = 0; i < LHIST + SBCRT; i++)
		plc_state->hist[i] = 0;
}
/***********************************************************
* Function: PLC_bad_frame()
*
* Purpose: Perform bad frame processing.
*
* Inputs: *plc_state - pointer to PLC state memory
* *ZIRbuf - pointer to the ZIR response of the SBC decoder
*
* Outputs: *out - pointer to the output samples
*
* Date: 03-18-2009
************************************************************/
void PLC_bad_frame(struct PLC_State *plc_state, short *ZIRbuf, short *out)
{
	int i;
	float val;
	float sf;
	static float sf_old;
	plc_state->nbf++;
	sf = 1.0f;
	i = 0;
	if (plc_state->nbf == 1)
	{
		sf_old = 1.0f;
		/* Perform pattern matching to find where to replicate */
		plc_state->bestlag = PatternMatch(plc_state->hist);
		plc_state->bestlag += M; /* the replication begins after the template match */
		/* Compute Scale Factor to Match Amplitude of Substitution Packet to that of Preceding Packet */
		sf = AmplitudeMatch(plc_state->hist, plc_state->bestlag);

		for (i = 0; i < OLAL; i++)
		{
			val = plc_state->hist[LHIST + i] * rcos[i] + sf*plc_state->hist[plc_state->bestlag + i] * rcos[OLAL - i - 1];
			if (val > 32767.0) val = 32767.0;
			if (val < -32768.0) val = -32768.0;
			plc_state->hist[LHIST + i] = (short)val;
		}
		for (; i<OLAL + FS; i++)
		{
			val = sf*plc_state->hist[plc_state->bestlag + i];
			if (val > 32767.0) val = 32767.0;
			if (val < -32768.0) val = -32768.0;
			plc_state->hist[LHIST + i] = (short)val;
		}
		for (; i<OLAL + FS + OLAL; i++)
		{
			val = sf*plc_state->hist[plc_state->bestlag + i] * rcos[i - FS - OLAL] + plc_state->hist[plc_state->bestlag + i] * rcos[OLAL - 1 - i + FS + OLAL];
			if (val > 32767.0) val = 32767.0;
			if (val < -32768.0) val = -32768.0;
			plc_state->hist[LHIST + i] = (short)val;
		}
		for (; i <OLAL + FS + SBCRT + OLAL; i++)
			plc_state->hist[LHIST + i] = plc_state->hist[plc_state->bestlag + i];
	}
	else
	{
		sf = sf_old;

		if (plc_state->nbf>5)
		{
			sf = 0;
		}

		for (i = 0; i < OLAL; i++)
		{
			val = sf_old*plc_state->hist[plc_state->bestlag + i] * rcos[i] + sf*plc_state->hist[plc_state->bestlag + i] * rcos[OLAL - 1 - i];
			if (val > 32767.0) val = 32767.0;
			if (val < -32768.0) val = -32768.0;
			plc_state->hist[LHIST + i] = (short)val;
		}
		for (; i < OLAL + FS; i++)
		{
			val = sf*plc_state->hist[plc_state->bestlag + i];
			if (val > 32767.0) val = 32767.0;
			if (val < -32768.0) val = -32768.0;
			plc_state->hist[LHIST + i] = (short)val;
		}
		//plc_state->hist[LHIST + i] = plc_state->hist[plc_state->bestlag + i];
		for (; i <OLAL + FS + SBCRT + OLAL; i++)
			plc_state->hist[LHIST + i] = (short)(sf*plc_state->hist[plc_state->bestlag + i]);

		sf_old = sf;
	}
	for (i = 0; i < FS; i++)
		out[i] = plc_state->hist[LHIST + i];
	/* shift the history buffer */
	for (i = 0; i < LHIST + OLAL + SBCRT + OLAL; i++)
		plc_state->hist[i] = plc_state->hist[i + FS];
}
/*****************************************************************************
* Function: PLC_good_frame()
*
* Purpose: Perform good frame processing. Most of the time, this function
* just updates history buffers and passes the input to the output,
* but in the first good frame after frame loss, it must conceal the
* received signal as it reconverges with the true output.
*
* Inputs: *plc_state - pointer to PLC state memory
* *in - pointer to the input vector
*
* Outputs: *out - pointer to the output samples
* Date: 03-18-2009
*****************************************************************************/
void PLC_good_frame(struct PLC_State *plc_state, short *in, short *out)
{
	int i;
	i = 0;
	if (plc_state->nbf > 0)
	{
		for (i = OLAL + SBCRT; i <OLAL + SBCRT + OLAL; i++)
			plc_state->hist[LHIST + i] = (short)(plc_state->hist[LHIST + i] * rcos[i - SBCRT - OLAL] + in[i - OLAL] * rcos[OLAL - 1 - i + SBCRT + OLAL]);

		for (; i < OLAL + FS; i++)
			plc_state->hist[LHIST + i] = in[i - OLAL];
	}
	else
	{
		/*Copy the output to the history buffer */
		for (i = 0; i < FS; i++)
			plc_state->hist[LHIST + OLAL + i] = out[i];
	}
	for (i = 0; i < FS; i++)
		out[i] = plc_state->hist[LHIST + i];
	/* shift the history buffer */
	for (i = 0; i < LHIST + OLAL; i++)
		plc_state->hist[i] = plc_state->hist[i + FS];

	plc_state->nbf = 0;
}
/*****************************************************************************
* Function: CrossCorrelation()
*
* Purpose: Compute the cross correlation according to Eq. (4) of Goodman
* paper, except that the true correlation is used. His formula
* seems to be incorrect.
*
* Inputs: *x - pointer to x input vector
* *y - pointer to y input vector
*
* Outputs: Cn - return value containing the cross-correlation of x and y
*
* Date: 03-18-2009
*****************************************************************************/
static float CrossCorrelation(short *x, short *y)
{
	int m;
	float num;
	float den;
	float Cn;
	float x2, y2;
	num = 0;
	den = 0;
	x2 = 0.0;
	y2 = 0.0;
	for (m = 0; m < M; m++)
	{
		num += ((float)x[m])*y[m];
		x2 += ((float)x[m])*x[m];
		y2 += ((float)y[m])*y[m];
	}
    den = sqrtf(x2*y2);
    //__asm volatile ("VSQRT.F32 %0,%1" : "=t"(den) : "t"(x2*y2));

	Cn = num / den;
	return(Cn);
}
/*****************************************************************************
* Function: PatternMatch()
*
* Purpose: Perform pattern matching to find the match of template with the
* history buffer according to Section B of Goodman paper.
*
* Inputs: *y : pointer to history buffer
*
* Outputs: return(int): the lag corresponding to the best match. The lag is
* with respect to the beginning of the history buffer.
*
* Date: 03-18-2009
*****************************************************************************/
static int PatternMatch(short *y)
{
	int n;
	float maxCn;
	float Cn;
	int bestmatch;
	maxCn = -999999.0; /* large negative number */
	bestmatch = 0;
	for (n = 0; n < N; n++)
	{
		Cn = CrossCorrelation(&y[LHIST - M] /* x */, &y[n]);
		if (Cn > maxCn)
		{
			bestmatch = n;
			maxCn = Cn;
		}
	}
	return(bestmatch);
}
/*****************************************************************************
* Function: AmplitudeMatch()
BLUETOOTH SPECIFICATION Page 122 of 126
Hands-Free Profile 1.6
10 May 2011
*
* Purpose: Perform amplitude matching using mean-absolute-value according
* to Goodman paper.
*
* Inputs: *y : pointer to history buffer
* bestmatch : value of the lag to the best match
*
* Outputs: return(float): scale factor
*
* Date: 03-19-2009
*****************************************************************************/
static float AmplitudeMatch(short *y, short bestmatch)
{
	int i;
	float sumx;
	float sumy;
	float sf;
	sumx = 0.0;
	sumy = 0.000001f;
	for (i = 0; i < FS; i++)
	{
		sumx += abs(y[LHIST - FS + i]);
		sumy += abs(y[bestmatch + i]);
	}
	sf = sumx / sumy;
	/* This is not in the paper, but limit the scaling factor to something
	reasonable to avoid creating artifacts */
	if (sf < 0.75f) sf = 0.75f;
	if (sf > 1.2f) sf = 1.2f;
	return(sf);
}