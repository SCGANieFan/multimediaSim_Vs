/*************************************************************
SBC Example PLC ANSI-C Source Code
File: sbcplc.c
*************************************************************/
#include <math.h>
//#include "sbc.h"
#include "sbcplc.h"
#include <stdint.h>
/* Local Function Prototypes */
static float CrossCorrelation(short M, float *x, float *y);
static int PatternMatch(short N, short M, short LHIST, float *y);
static float AmplitudeMatch(short LHIST, short FS, float *y, short bestmatch);

static int plc_ssat16(float x)
{
	int val;
	val = (int)x;

	if(val > 32767)
		val = 32767;

	if(val < -32768)
		val = -32768;

	return val;
}

static int plc_ssat24(float x)
{
	int val;
	val = (int)x;

	if(val > 8388607)
		val = 8388607;

	if(val < -8388608)
		val = -8388608;

	return val;
}
/*****************************************************************************
* Function: InitPLC() *
* Purpose: Perform PLC initialization of memory vectors. *
* Inputs: *plc_state - pointer to PLC state memory *
* Outputs: *plc_state - initialized memory. *
* Date: 03-18-2009
*****************************************************************************/
void a2dp_plc_init(struct PLC_State *plc_state, int size, unsigned int type, float* history, float* rcos)
{
	plc_state->hist = history;
	plc_state->nbf = 0;
	plc_state->bestlag = 0;
	int sz = size;
	if(sz == 0) sz = 480;
	switch (type)
	{
		case 0://sbc
		{
			plc_state->FS = 128;
			plc_state->N = 256;
			plc_state->M = 64;
			plc_state->LHIST = plc_state->N * 3 + plc_state->FS - 1;
			plc_state->SBCRT = 60;
			plc_state->OLAL = 60;
			break;
		}
		case 1://aac
		{
			plc_state->FS = 1024;
			plc_state->N = 1024;
			plc_state->M = 64;
			plc_state->LHIST = plc_state->N + plc_state->FS - 1;
			plc_state->SBCRT = 120;
			plc_state->OLAL = 120;
			break;
		}
		case 2://lc3
		{
			plc_state->FS = sz;
			plc_state->N = sz;
			plc_state->M = 64;
			plc_state->LHIST = plc_state->N * 3 + plc_state->FS - 1;
			plc_state->SBCRT = 120;
			plc_state->OLAL = 120;
			break;
		}
		case 3:
		{
			plc_state->FS = size;//480
			plc_state->N = 128;//256
			plc_state->M = 48;
			plc_state->LHIST = plc_state->N + plc_state->FS - 1;
			plc_state->SBCRT = 10;
			plc_state->OLAL = 10;
			break;
		}
		default:
			break;
	}

	for(int i=0; i<plc_state->OLAL; i++){
		rcos[i] = (float)(cos(3.1415926 * i / plc_state->OLAL) + 1)/2;
	}

	for (int i=0; i < LHIST_MAX; i++)
		plc_state->hist[i] = 0;

	plc_state->g_bad_cnt = 0;
	plc_state->good_cnt = 32768;
	plc_state->sf_old = 1.0f;
    plc_state->tmp_gain = 1.0f;
	plc_state->theta_cnt = 0;
}
void a2dp_plc_init(struct PLC_State *plc_state, int Fs, int search_size, int match_size, int olal, int sbcrt, float *history, float *rcos)
{
	plc_state->hist = history;
	plc_state->nbf = 0;
	plc_state->bestlag = 0;

	plc_state->FS = Fs;
	plc_state->N = search_size;
	plc_state->M = match_size;
	plc_state->LHIST = plc_state->N * 2 + plc_state->FS - 1;
	plc_state->SBCRT = sbcrt;
	plc_state->OLAL = olal;

	for (int i = 0; i < plc_state->OLAL; i++) {
		rcos[i] = (float)(cos(3.1415926 * i / plc_state->OLAL) + 1) / 2;
	}

	for (int i = 0; i < LHIST_MAX; i++)
		plc_state->hist[i] = 0;

	plc_state->g_bad_cnt = 0;
	plc_state->good_cnt = 32768;
	plc_state->sf_old = 1.0f;
	plc_state->tmp_gain = 1.0f;
	plc_state->theta_cnt = 0;
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

void a2dp_plc_bad_frame(struct PLC_State *plc_state, short *ZIRbuf, short *out, float *cos_buf, int len, float *rcos, int stride, int index)
{
	int i;
	float val;
	float sf;
	// static float sf_old;
	plc_state->nbf++;
	sf = 1.0f;
	i = 0;
    //static float cnt = 0;
	//float step = 0.06;
    // static float tmp_gain = 1.0;
	// static int theta_cnt = 0;
	static short OLAL, LHIST, FS, SBCRT, M;

	OLAL = plc_state->OLAL;
	LHIST = plc_state->LHIST;
	FS = plc_state->FS;
	SBCRT = plc_state->SBCRT;
	M = plc_state->M;

	if (plc_state->nbf == 1)
	{
	    //cnt = 1;
		plc_state->sf_old = 1.0f;
        plc_state->tmp_gain = 1.0f;
		plc_state->theta_cnt = 0;
		/* Perform pattern matching to find where to replicate */
		plc_state->bestlag = PatternMatch(plc_state->N, plc_state->M, plc_state->LHIST, plc_state->hist);
		plc_state->bestlag += M; /* the replication begins after the template match */
		/* Compute Scale Factor to Match Amplitude of Substitution Packet to that of Preceding Packet */
		//sf = AmplitudeMatch(plc_state->hist, plc_state->bestlag);

		for (i = 0; i < OLAL; i++)
		{
			val = plc_state->hist[LHIST + i] * rcos[i] + sf*plc_state->hist[plc_state->bestlag + i] * rcos[OLAL - i - 1];
			//if (val > 32767.0) val = 32767.0;
			//if (val < -32768.0) val = -32768.0;
			plc_state->hist[LHIST + i] = val;
		}
		for (; i<OLAL + FS; i++)
		{
			val = sf*plc_state->hist[plc_state->bestlag + i];
			//if (val > 32767.0) val = 32767.0;
			//if (val < -32768.0) val = -32768.0;
			plc_state->hist[LHIST + i] = val;
		}
		for (; i<OLAL + FS + OLAL; i++)
		{
			val = sf*plc_state->hist[plc_state->bestlag + i] * rcos[i - FS - OLAL] + plc_state->hist[plc_state->bestlag + i] * rcos[OLAL - 1 - i + FS + OLAL];
			//if (val > 32767.0) val = 32767.0;
			//if (val < -32768.0) val = -32768.0;
			plc_state->hist[LHIST + i] = val;
		}
		for (; i <OLAL + FS + SBCRT + OLAL; i++)
			plc_state->hist[LHIST + i] = plc_state->hist[plc_state->bestlag + i];
	}
	else
	{
		//sf = 0.2*sf+0.8*sf_old;

		if (plc_state->nbf>28)
		{
			sf = 0;
		}

		for (i = 0; i < OLAL; i++)
		{
			val = plc_state->sf_old*plc_state->hist[plc_state->bestlag + i] * rcos[i] + sf*plc_state->hist[plc_state->bestlag + i] * rcos[OLAL - 1 - i];
			//if (val > 32767.0) val = 32767.0;
			//if (val < -32768.0) val = -32768.0;
			plc_state->hist[LHIST + i] = val;
		}
		for (; i < OLAL + FS; i++)
		{
			val = sf*plc_state->hist[plc_state->bestlag + i];
			//if (val > 32767.0) val = 32767.0;
			//if (val < -32768.0) val = -32768.0;
			plc_state->hist[LHIST + i] = val;
		}
		//plc_state->hist[LHIST + i] = plc_state->hist[plc_state->bestlag + i];
		for (; i <OLAL + FS + SBCRT + OLAL; i++)
			plc_state->hist[LHIST + i] = sf*plc_state->hist[plc_state->bestlag + i];

		plc_state->sf_old = sf;
	}
    /*
    float ratio = 1 - plc_state->nbf * step;
    if(ratio<0){
        ratio = 0;
    }
    */
	for (i = 0; i < FS; i++){
        //plc_state->hist[LHIST + i] = (short)(plc_state->hist[LHIST + i] * ratio);
        if (plc_state->theta_cnt < len*4)
        {
            plc_state->tmp_gain = cos_buf[plc_state->theta_cnt];
            plc_state->theta_cnt++;
        }
        else
            plc_state->tmp_gain = 0;
        //tmp_gain = 1;
        out[i * stride + index] = (short)(plc_ssat16(plc_state->hist[LHIST + i]*plc_state->tmp_gain));
    }
	plc_state->g_bad_cnt = plc_state->theta_cnt;
		//out[i * stride + index] = plc_state->hist[LHIST + i];
	/* shift the history buffer */
	for (i = 0; i < LHIST + OLAL + SBCRT + OLAL; i++)
		plc_state->hist[i] = plc_state->hist[i + FS];
}

void a2dp_plc_bad_frame_smooth(struct PLC_State *plc_state, short *ZIRbuf, short *out, float *rcos, int stride, int index)
{
    int i;
	//float val;
    i = 0;
    plc_state->nbf++;

	static short OLAL, LHIST, FS, SBCRT;

	OLAL = plc_state->OLAL;
	LHIST = plc_state->LHIST;
	FS = plc_state->FS;
	SBCRT = plc_state->SBCRT;

    if (plc_state->nbf == 1){
        for (i = 0; i < OLAL; i++)
        {
            plc_state->hist[LHIST + i] = plc_state->hist[LHIST + i] * rcos[i];
        }

        for (; i <OLAL + FS + SBCRT + OLAL; i++)
        {
            plc_state->hist[LHIST + i] = 0;
        }
    }else{
        for (i = 0; i < FS + OLAL + SBCRT + OLAL; i++){
            plc_state->hist[LHIST + i] = 0;
        }
    }

    for (i = 0; i < FS; i++){
        out[i * stride + index] =  (short)(plc_ssat16(plc_state->hist[LHIST + i]));
    }

    for (i = 0; i < LHIST + OLAL + SBCRT + OLAL; i++)
        plc_state->hist[i] = plc_state->hist[i + FS];
}

void a2dp_plc_bad_frame_24bit(struct PLC_State *plc_state, int *ZIRbuf, int *out, float *cos_buf, int len, float *rcos, int stride, int index)
{
	int i;
	float val;
	float sf;
	// static float sf_old;
	plc_state->nbf++;
	sf = 1.0f;
	i = 0;
    //static float cnt = 0;
	//float step = 0.06;
    // static float tmp_gain = 1.0;
	// static int theta_cnt = 0;
	static short OLAL, LHIST, FS, SBCRT, M;

	OLAL = plc_state->OLAL;
	LHIST = plc_state->LHIST;
	FS = plc_state->FS;
	SBCRT = plc_state->SBCRT;
	M = plc_state->M;

	if (plc_state->nbf == 1)
	{
	    //cnt = 1;
		plc_state->sf_old = 1.0f;
        plc_state->tmp_gain = 1.0f;
		plc_state->theta_cnt = 0;
		/* Perform pattern matching to find where to replicate */
		plc_state->bestlag = PatternMatch(plc_state->N, plc_state->M, plc_state->LHIST, plc_state->hist);
		plc_state->bestlag += M; /* the replication begins after the template match */
		/* Compute Scale Factor to Match Amplitude of Substitution Packet to that of Preceding Packet */
		//sf = AmplitudeMatch(plc_state->hist, plc_state->bestlag);

		for (i = 0; i < OLAL; i++)
		{
			val = plc_state->hist[LHIST + i] * rcos[i] + sf*plc_state->hist[plc_state->bestlag + i] * rcos[OLAL - i - 1];
			//if (val > 32767.0) val = 32767.0;
			//if (val < -32768.0) val = -32768.0;
			plc_state->hist[LHIST + i] = val;
		}
		for (; i<OLAL + FS; i++)
		{
			val = sf*plc_state->hist[plc_state->bestlag + i];
			//if (val > 32767.0) val = 32767.0;
			//if (val < -32768.0) val = -32768.0;
			plc_state->hist[LHIST + i] = val;
		}
		for (; i<OLAL + FS + OLAL; i++)
		{
			val = sf*plc_state->hist[plc_state->bestlag + i] * rcos[i - FS - OLAL] + plc_state->hist[plc_state->bestlag + i] * rcos[OLAL - 1 - i + FS + OLAL];
			//if (val > 32767.0) val = 32767.0;
			//if (val < -32768.0) val = -32768.0;
			plc_state->hist[LHIST + i] = val;
		}
		for (; i <OLAL + FS + SBCRT + OLAL; i++)
			plc_state->hist[LHIST + i] = plc_state->hist[plc_state->bestlag + i];
	}
	else
	{
		//sf = 0.2*sf+0.8*sf_old;

		if (plc_state->nbf>28)
		{
			sf = 0;
		}

		for (i = 0; i < OLAL; i++)
		{
			val = plc_state->sf_old*plc_state->hist[plc_state->bestlag + i] * rcos[i] + sf*plc_state->hist[plc_state->bestlag + i] * rcos[OLAL - 1 - i];
			//if (val > 32767.0) val = 32767.0;
			//if (val < -32768.0) val = -32768.0;
			plc_state->hist[LHIST + i] = val;
		}
		for (; i < OLAL + FS; i++)
		{
			val = sf*plc_state->hist[plc_state->bestlag + i];
			//if (val > 32767.0) val = 32767.0;
			//if (val < -32768.0) val = -32768.0;
			plc_state->hist[LHIST + i] = val;
		}
		//plc_state->hist[LHIST + i] = plc_state->hist[plc_state->bestlag + i];
		for (; i <OLAL + FS + SBCRT + OLAL; i++)
			plc_state->hist[LHIST + i] = sf*plc_state->hist[plc_state->bestlag + i];

		plc_state->sf_old = sf;
	}
    /*
    float ratio = 1 - plc_state->nbf * step;
    if(ratio<0){
        ratio = 0;
    }
    */
	for (i = 0; i < FS; i++){
        //plc_state->hist[LHIST + i] = (short)(plc_state->hist[LHIST + i] * ratio);
        if (plc_state->theta_cnt < len*4)
        {
            plc_state->tmp_gain = cos_buf[plc_state->theta_cnt];
            plc_state->theta_cnt++;
        }
        else
            plc_state->tmp_gain = 0;
        //tmp_gain = 1;
        out[i * stride + index] = plc_ssat24(plc_state->hist[LHIST + i]*plc_state->tmp_gain);
    }
	plc_state->g_bad_cnt = plc_state->theta_cnt;
		//out[i * stride + index] = plc_state->hist[LHIST + i];
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
void a2dp_plc_good_frame(struct PLC_State *plc_state, short *in, short *out, float *cos_buf, int len, float *rcos, int stride, int index)
{
	int i;
	i = 0;
    // static int good_cnt = 32768;

	static short OLAL, LHIST, FS, SBCRT;

	OLAL = plc_state->OLAL;
	LHIST = plc_state->LHIST;
	FS = plc_state->FS;
	SBCRT = plc_state->SBCRT;

	if (plc_state->nbf > 0)
	{
	    plc_state->good_cnt = 0;
		for (i = OLAL + SBCRT; i <OLAL + SBCRT + OLAL; i++)
			plc_state->hist[LHIST + i] = plc_state->hist[LHIST + i] * rcos[i - SBCRT - OLAL] + in[(i - OLAL) * stride + index] * rcos[OLAL - 1 - i + SBCRT + OLAL];

		for (; i < OLAL + FS; i++)
			plc_state->hist[LHIST + i] = in[(i - OLAL) * stride + index];
	}
	else
	{
		/*Copy the output to the history buffer */
		for (i = 0; i < FS; i++)
			plc_state->hist[LHIST + OLAL + i] = in[i * stride + index];
	}
	for (i = 0; i < FS; i++)
	{
		/*
		if (plc_state->nbf > 0 || 896>good_cnt)
		{
			float tmp_gain;
			if (896 > good_cnt)
				//tmp_gain = cos_buf[895 - good_cnt];
				tmp_gain = (cosf(3.1415926*good_cnt / (128 * 7)) + 1) / 2;

			else
				tmp_gain = 1;
			good_cnt++;
			//out[i * stride + index] = plc_state->hist[LHIST + i] * tmp_gain;
			out[i * stride + index] = plc_state->hist[LHIST + i];
		}
		else
		*/
		float tmp_good_gain;
        if(plc_state->g_bad_cnt>len*4){
            plc_state->g_bad_cnt = len * 4;
        }
		if (plc_state->good_cnt < plc_state->g_bad_cnt) {
			tmp_good_gain = cos_buf[plc_state->g_bad_cnt - plc_state->good_cnt-1];
		}
		else {
			tmp_good_gain = 1;
		}

		plc_state->good_cnt++;
		out[i * stride + index] = (short)plc_ssat16(plc_state->hist[LHIST + i]* tmp_good_gain);
	}

	/* shift the history buffer */
	for (i = 0; i < LHIST + OLAL; i++)
		plc_state->hist[i] = plc_state->hist[i + FS];

	plc_state->nbf = 0;
}

void a2dp_plc_good_frame_smooth(struct PLC_State *plc_state, short *in, short *out, float *rcos, int stride, int index)
{
    int i;
    i = 0;

	static short OLAL, LHIST, FS, SBCRT;

	OLAL = plc_state->OLAL;
	LHIST = plc_state->LHIST;
	FS = plc_state->FS;
	SBCRT = plc_state->SBCRT;

    if (plc_state->nbf > 0){
        for (i = OLAL + SBCRT; i <OLAL + SBCRT + OLAL; i++)
            plc_state->hist[LHIST + i] = (short)(plc_state->hist[LHIST + i] * rcos[i - SBCRT - OLAL] + in[(i - OLAL) * stride + index] * rcos[OLAL - 1 - i + SBCRT + OLAL]);

        for (; i < OLAL + FS; i++)
            plc_state->hist[LHIST + i] = in[(i - OLAL) * stride + index];
    }else{
        for (i = 0; i < FS; i++)
            plc_state->hist[LHIST + OLAL + i] = in[i * stride + index];
    }

    for (i = 0; i < FS; i++){
        //out[i * stride + index] = plc_state->hist[LHIST + i];
		 out[i * stride + index] =  (short)(plc_ssat16(plc_state->hist[LHIST + i]));
    }

    for (i = 0; i < LHIST + OLAL; i++){
        plc_state->hist[i] = plc_state->hist[i + FS];
    }
    plc_state->nbf = 0;
}

void a2dp_plc_good_frame_24bit(struct PLC_State *plc_state, int *in, int *out, float *cos_buf, float *rcos, int len, int stride, int index)
{
	int i;
	i = 0;
    // static int good_cnt = 32768;

	static short OLAL, LHIST, FS, SBCRT;

	OLAL = plc_state->OLAL;
	LHIST = plc_state->LHIST;
	FS = plc_state->FS;
	SBCRT = plc_state->SBCRT;

	if (plc_state->nbf > 0)
	{
	    plc_state->good_cnt = 0;
		for (i = OLAL + SBCRT; i <OLAL + SBCRT + OLAL; i++)
			plc_state->hist[LHIST + i] = plc_state->hist[LHIST + i] * rcos[i - SBCRT - OLAL] + in[(i - OLAL) * stride + index] * rcos[OLAL - 1 - i + SBCRT + OLAL];

		for (; i < OLAL + FS; i++)
			plc_state->hist[LHIST + i] = in[(i - OLAL) * stride + index];
	}
	else
	{
		/*Copy the output to the history buffer */
		for (i = 0; i < FS; i++)
			plc_state->hist[LHIST + OLAL + i] = in[i * stride + index];
	}
	for (i = 0; i < FS; i++)
	{
		/*
		if (plc_state->nbf > 0 || 896>good_cnt)
		{
			float tmp_gain;
			if (896 > good_cnt)
				//tmp_gain = cos_buf[895 - good_cnt];
				tmp_gain = (cosf(3.1415926*good_cnt / (128 * 7)) + 1) / 2;

			else
				tmp_gain = 1;
			good_cnt++;
			//out[i * stride + index] = plc_state->hist[LHIST + i] * tmp_gain;
			out[i * stride + index] = plc_state->hist[LHIST + i];
		}
		else
		*/
		float tmp_good_gain;
        if(plc_state->g_bad_cnt>len*4){
            plc_state->g_bad_cnt = len * 4;
        }
		if (plc_state->good_cnt < plc_state->g_bad_cnt) {
			tmp_good_gain = cos_buf[plc_state->g_bad_cnt - plc_state->good_cnt-1];
		}
		else {
			tmp_good_gain = 1;
		}

		plc_state->good_cnt++;
		out[i * stride + index] = (int32_t)plc_ssat24(plc_state->hist[LHIST + i]* tmp_good_gain);
	}

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
static float CrossCorrelation(short M, float *x, float *y)
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
	den = (float)sqrt(x2*y2);
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
static int PatternMatch(short N, short M, short LHIST, float *y)
{
	int n;
	float maxCn;
	float Cn;
	int bestmatch;
	maxCn = -999999.0; /* large negative number */
	bestmatch = 0;
	for (n = 0; n < N; n++)
	{
		Cn = CrossCorrelation(M, &y[LHIST - M] /* x */, &y[n]);
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
static float AmplitudeMatch(short LHIST, short FS, float *y, short bestmatch)
{
	int i;
	float sumx;
	float sumy;
	float sf;
	sumx = 0.0;
	sumy = 0.000001f;
	for (i = 0; i < FS; i++)
	{
		sumx += fabsf(y[LHIST - FS + i]);
		sumy += fabsf(y[bestmatch + i]);
	}
	sf = sumx / sumy;
	/* This is not in the paper, but limit the scaling factor to something
	reasonable to avoid creating artifacts */
	if (sf < 0.75f) sf = 0.75f;
	if (sf > 1.2f) sf = 1.2f;
	return(sf);
}

void cos_generate(float *cos_buf, short len, short packet_len)
{
    if(len > packet_len)
    {
        for (int i = 0; i < packet_len; i++)
        {
            cos_buf[i] = 1;
        }
        for (int i = packet_len; i < len; i++)
        {
    		cos_buf[i] = (float)(cos(3.1415926*(i - packet_len) / (len - packet_len)) + 1) / 2;
			//cos_buf[i] = 1;
        }
    }
    else //len==packet_len
    {
        for (int i = 0; i < len; i++)
        {
            cos_buf[i] = (float)(cos(3.1415926 * i / len) + 1) / 2;
        }
    }
    /*
    for(int i=packet_len; i<packet_len*2; i++){
        cos_buf[i] = (float)(cos(3.1415926*2*i/packet_len+3.1415926)+1)/6;
    }

    for(int i=packet_len*2; i<len; i++){
        cos_buf[i] = (float)(cos(3.1415926*2*i/packet_len+3.1415926)+1)/8;
    }
    */
}




