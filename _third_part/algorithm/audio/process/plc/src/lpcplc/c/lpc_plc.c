#include <stdio.h>
#include <stdlib.h>
#include "lpc_plc.h"
#include "table.h"
//#include "speech_types.h"
//#include <stdint.h>

//#ifndef __arm__
#include "DSP.h"
//#else
//#include "dspfns.h"
//#include "dsp_netsi.h"
//#endif

#define BUFFERSIZE  (LPCO+FRSZ+CODELAY+MAXOS) //(8+320)
#define MIN_UNSTBL (CODELAY/2)         /* don't use these first G722 samples */

static void print_vec(FILE *fd, Word16 *vec, Word16 len, char *str)
{
    fprintf(fd, "%s: \n", str);
    for (Word16 i = 0; i < len - 1; i++) {
        fprintf(fd, "%d, ", vec[i]);
    }
    fprintf(fd, "%d\n", vec[len - 1]);
}

/*-----------------------------------------------------------------------------
* Function: Reset_WB_PLC()
*
* Description: reset the plc state variables
*
* Inputs:  *plc  - pointer to plc state memory
*
* Outputs: *plc  - values reset
*---------------------------------------------------------------------------*/
void Reset_WB_PLC(struct WB_PLC_State *plc)
{
	int	i;

	W16zero((Word16 *)plc, sizeof(struct WB_PLC_State) / 2);
	plc->al[0] = 4096;
	plc->alast[0] = 4096;
	plc->xwd_exp = 31;
	plc->cpplast = i_mult(12, cpp_scale);
	plc->pp = 50;
	plc->ngfae = 9;
	plc->wsz = 1;
#if WMOPS
	move16(); move16(); move16(); move32(); move16(); move16(); move16();
#endif
	for(i = 0; i < PPHL; i++)
	{
		plc->pph[i] = plc->pp;
#if WMOPS
		move16();
#endif
	}

    // for long overlap
    plc->ovlp = 0;
    plc->ovlpidx = 0;
    plc->pup = NULL;
    plc->pdown = NULL;
}
/*-----------------------------------------------------------------------------
* Function: apfilterQ0_Q0()
*
* Description: Performs all-pole filtering with Q0 in and out.
*
* Inputs:  a[]   - prediction coefficients, Q12
*          m     - LPC order
*          x[]   - input signal
*          lg    - size of filtering
*          mem[] - filter memory
*
* Outputs: y[]   - output signal
*          mem[] - filter memory
*---------------------------------------------------------------------------*/
void apfilterQ0_Q0(
	Word16 a[],
	Word16 m,
	Word16 x[],
	Word16 y[],
	Word16 lg,
	Word16 mem[]
	)
{
	Word16 i, j;
	Word32 s;
#if (DMEM)
	Word16 *tmp;
#else
	Word16 tmp[BUFFERSIZE];
#endif
	Word16 *yy;

#if (DMEM)
	/* memory allocation */
	tmp = allocWord16(0, m + lg - 1);
#endif

	/* Copy mem[] to yy[] */

	W16copy(tmp, mem, m);
	yy = &tmp[m];

	/* Do the filtering. */

	for(i = 0; i < lg; i++) {
		s = L_mult0(x[i], a[0]); /* Q12 */
		for(j = 1; j <= m; j++) s = L_msu0(s, a[j], yy[-j]); /* Q12 */
		*yy++ = roundInt(L_shl(s, 4)); /* Q0 */
#ifdef WMOPS
		move16();
#endif
	}
	W16copy(y, &tmp[m], lg);

#if (DMEM)
	/* memory deallocation */
	deallocWord16(tmp, 0, m + lg - 1);
#endif

}


/*-----------------------------------------------------------------------------
* Function: apfilterQ1_Q0()
*
* Description: Performs all-pole filtering with Q1 in and Q0 out.
*
* Inputs:  a[]   - prediction coefficients, Q12
*          m     - LPC order
*          x[]   - input signal
*          lg    - size of filtering
*          mem[] - filter memory
*
* Outputs: y[]   - output signal
*          mem[] - filter memory
*---------------------------------------------------------------------------*/
void apfilterQ1_Q0(
	Word16 a[],
	Word16 m,
	Word16 x[],
	Word16 y[],
	Word16 lg,
	Word16 mem[]
	)
{
	Word16 i;
	Word32 s;

	/* Copy mem[] to y[] */
	W16copy(y - m, mem, m);

	/* Do the filtering. */
	for(i = 0; i < lg; i++)
	{
		s = L_mult0(x[i], a[0]); /* Q13 */
		s = L_msu(s, a[1], y[-1]); /* Q13 */
		s = L_msu(s, a[2], y[-2]); /* Q13 */
		s = L_msu(s, a[3], y[-3]); /* Q13 */
		s = L_msu(s, a[4], y[-4]); /* Q13 */
		s = L_msu(s, a[5], y[-5]); /* Q13 */
		s = L_msu(s, a[6], y[-6]); /* Q13 */
		s = L_msu(s, a[7], y[-7]); /* Q13 */
		s = L_msu(s, a[8], y[-8]); /* Q13 */
		*y++ = roundInt(L_shl(s, 3)); /* Q0 */
#ifdef WMOPS
		move16();
#endif
	}
}

/*-----------------------------------------------------------------------------
 * Function: coarsepitch()
 *
 * Description: COARSE PITCH period extraction.
 *              This function extracts the coarse pitch period based on the
 *              2 kHz decimated weighted speech signal.
 *
 * Inputs:  *xwd  - pointer to 2 kHz decimated weighted speech buffer
 *          cpplast - coarse pitch period of the last frame
 * Outputs: (return value of the function) - extracted coarse pitch period
 *---------------------------------------------------------------------------*/
short   coarsepitch(
short 	*xwd,
short	cpplast)
{
  short    s;       /* Q3 */
  short    a, b;
  short    im;
  short    maxdev, flag, mpflag;
  int      eni, deltae;
  int      cc;
  short    ah,al, bh, bl;
  int      *cor, *energy;
  short    *cor2, *cor2_exp;
  int      a0, a1, a2, a3;
  short    *fp0, *fp1, *fp2, *fp3;
  short    cor2max, cor2max_exp;
  short    cor2m, cor2m_exp;
  short    s0, t0, t1, exp0, exp1, e2, e3,tt;
  short    threshold;
  short	   mplth;		/* Q3 */

  short    i, j, k, n, npeaks, imax, idx[MAXPPD-MINPPD+1];
  short    cpp;

  short plag[HMAXPPD] = { 0, };
  short _cor2[MAXPPD1+1], _cor2_exp[MAXPPD1+1];
  short cor2i[HMAXPPD], cor2i_exp[HMAXPPD];
  int _cor[MAXPPD1+1], _energy[MAXPPD1+1];

  short _energy_man[MAXPPD1+1], _energy_exp[MAXPPD1+1];
  short energyi_man[HMAXPPD], energyi_exp[HMAXPPD];
  short *energy_man, *energy_exp;
  short energym_man, energym_exp;
  short energymax_man, energymax_exp;

  /* COMPUTE CORRELATION & ENERGY OF PREDICTION BASIS VECTOR */

  cor = _cor+1;
  energy = _energy+1;
  cor2 = _cor2+1;
  cor2_exp = _cor2_exp+1;

  energy_man = _energy_man+1;
  energy_exp = _energy_exp+1;

  fp0 = &xwd[MAXPPD1];
  fp1 = &xwd[MAXPPD1-M1];
  a0 = L_mult0(*fp1, *fp1);
  a1 = L_mult0(*fp0++, *fp1++);
  for (i=1;i<PWSZD;i++)
  {
    a0 = L_mac0(a0, *fp1, *fp1);
    a1 = L_mac0(a1, *fp0++, *fp1++);
  }

  cor[M1-1] = a1;
  energy[M1-1] = a0;
  energy_exp[M1-1] = norm_l(energy[M1-1]);
  energy_man[M1-1] = extract_h(L_shl(energy[M1-1], energy_exp[M1-1]));
  s0 = cor2_exp[M1-1] = norm_l(a1);
  t0 = extract_h(L_shl(a1, s0));
  tt = extract_h(L_mult(t0, t0));
#if WMOPS
   move32();move32();move32();move32();move16();
#endif

  if (a1 < 0)
  {
     tt = negate(tt);
  }
  cor2[M1-1] = tt;
#ifdef WMOPS
  move16();
#endif

  fp2 = &xwd[LXD-M1-1];
  fp3 = &xwd[MAXPPD1-M1-1];

  for (i=M1;i<M2;i++) {
    fp0 = &xwd[MAXPPD1];
    fp1 = &xwd[MAXPPD1-1-i];
    a1 = L_mult0(*fp0++,*fp1++);
    for (j=1;j<(LXD-MAXPPD1);j++) a1 = L_mac0(a1,*fp0++,*fp1++);
    cor[i] = a1;
    a0 = L_msu0(a0, *fp2, *fp2);
    a0 = L_mac0(a0, *fp3, *fp3);
    fp2--; fp3--;  /* pointer manipulation here can be avoided in a real implementation */
    energy[i] = a0;
    energy_exp[i] = norm_l(energy[i]);
    energy_man[i] = extract_h(L_shl(energy[i], energy_exp[i]));
    s0 = cor2_exp[i] = norm_l(a1);
    t0 = extract_h(L_shl(a1, s0));
    tt = extract_h(L_mult(t0, t0));
#if WMOPS
    move32();move32();move16();move16();move16();
#endif
    if (a1 < 0)
    {
       tt = negate(tt);
    }
    cor2[i] = tt;
#ifdef WMOPS
    move16();
#endif
  }

  /* FIND POSITIVE CORRELATION PEAKS */
  /* FIND MAXIMUM OF COR*COR/ENERGY AMONG POSITIVE CORRELATION PEAKS */
  npeaks = 0;
  n=MINPPD-1;
#if WMOPS
  move16();move16();
#endif
  while ((sub(n,MAXPPD)<0)&&(sub(npeaks,MAX_NPEAKS)<0))
  {
#ifdef WMOPS
    test();
#endif
    if (cor[n]>0)
    {
      a0   = L_mult(energy_man[n-1],cor2[n]);
      a1   = L_mult(energy_man[n], cor2[n-1]);
#ifdef PLATFORM
	  exp0 = SSAT16_SHIFTL_1(sub(cor2_exp[n], cor2_exp[n-1]));
#else
      exp0 = shl(sub(cor2_exp[n], cor2_exp[n-1]),1);
#endif
      exp0 = add(exp0, energy_exp[n-1]);
      exp0 = sub(exp0, energy_exp[n]);

      if (exp0>=0)
	      a0 = L_shr(a0, exp0);
      if (exp0<0)
	      a1 = L_shl(a1, exp0);

      if (L_sub(a0, a1)>0)
      {
	      a0   = L_mult(energy_man[n+1],cor2[n]);
	      a1   = L_mult(energy_man[n], cor2[n+1]);
#ifdef PLATFORM
		  exp0 = SSAT16_SHIFTL_1(sub(cor2_exp[n], cor2_exp[n + 1]));
#else
	      exp0 = shl(sub(cor2_exp[n], cor2_exp[n+1]),1);
#endif
	      exp0 = add(exp0, energy_exp[n+1]);
	      exp0 = sub(exp0, energy_exp[n]);

	      if (exp0>=0)
	        a0 = L_shr(a0, exp0);
	      if (exp0<0)
	        a1 = L_shl(a1, exp0);

	      if (L_sub(a0, a1)>0)
         {
	        idx[npeaks] = n;
	        npeaks=add(npeaks,1);
#if WMOPS
           move16();
#endif
	      }
      }
    }
	n=add(n, 1);
  }

  /* if there are no positive peaks, repeat allowing negative peaks */
  if (npeaks == 0){
	  n=MINPPD-1;
#if WMOPS
	  move16();
#endif

	  for (i=0;i<MAXPPD1;i++) {
		  cor2[i] = sub(0,cor2[i]);
		  cor[i]  = L_sub(0,cor[i]);
#ifdef WMOPS
        move16();move32();
#endif
	  }

	  while ((sub(n,MAXPPD)<0)&&(sub(npeaks,MAX_NPEAKS)<0))
	  {
#ifdef WMOPS
        test();
#endif
		  if (cor[n]>0)
		  {
			  a0   = L_mult(energy_man[n-1],cor2[n]);
			  a1   = L_mult(energy_man[n], cor2[n-1]);
#ifdef PLATFORM
			  exp0 = SSAT16_SHIFTL_1(sub(cor2_exp[n], cor2_exp[n - 1]));
#else
			  exp0 = shl(sub(cor2_exp[n], cor2_exp[n-1]),1);
#endif
			  exp0 = add(exp0, energy_exp[n-1]);
			  exp0 = sub(exp0, energy_exp[n]);
			  if (exp0>=0)
				  a0 = L_shr(a0, exp0);
			  if (exp0<0)
				  a1 = L_shl(a1, exp0);
			  if (L_sub(a0, a1)>0)
			  {
				  a0   = L_mult(energy_man[n+1],cor2[n]);
				  a1   = L_mult(energy_man[n], cor2[n+1]);
#ifdef PLATFORM
				  exp0 = SSAT16_SHIFTL_1(sub(cor2_exp[n], cor2_exp[n+1]));
#else
				  exp0 = shl(sub(cor2_exp[n], cor2_exp[n+1]),1);
#endif
				  exp0 = add(exp0, energy_exp[n+1]);
				  exp0 = sub(exp0, energy_exp[n]);
				  if (exp0>=0)
					  a0 = L_shr(a0, exp0);
				  if (exp0<0)
					  a1 = L_shl(a1, exp0);
				  if (L_sub(a0, a1)>0)
				  {
					  idx[npeaks] = n;
					  npeaks=add(npeaks, 1);
#if WMOPS
					  move16();
#endif
				  }
			  }
		  }
		  n=add(n,1);
	  }

	  if (npeaks == 0){   /* if there are no positive AND no negative peaks, */

		  return (i_mult(MINPPD, cpp_scale)); /* return minimum pitch period */
	  }
  }

  if (sub(npeaks, 1)==0){   /* if there is exactly one peak, */

    /* return the time lag for this single peak */
    return (i_mult(add(idx[0],1), cpp_scale));
  }


  /* if PROGRAM PROCEEDS TO HERE, THERE ARE 2 OR MORE PEAKS */
  cor2max=(short) 0x8000;
  cor2max_exp= (short) 0;
  energymax_man=1;
  energymax_exp=0;
  imax=0;
#if WMOPS
  move16();move16();move16();move16();move16();
#endif
  for (i=0; i < npeaks; i++) {

    /* FIND INTERPOLATED PEAKS OF cor2[]/energy[] USING QUADRATIC
       INTERPOLATION for cor[] AND LINEAR INTERPOLATION for energy[]. */
    /* first calculate coefficients of quadratic function y(x)=ax^2+bx+c; */
    n=idx[i];
#ifdef PLATFORM
	a0 = L_sub(SSAT_SHIFTR_1(L_add(cor[n + 1], cor[n - 1])), cor[n]);
#else
    a0=L_sub(L_shr(L_add(cor[n+1],cor[n-1]),1),cor[n]);
#endif
    L_Extract(a0, &ah, &al);
#ifdef PLATFORM
	a0 = SSAT_SHIFTR_1(L_sub(cor[n + 1], cor[n - 1]));
#else
    a0=L_shr(L_sub(cor[n+1],cor[n-1]),1);
#endif
    L_Extract(a0, &bh, &bl);
    cc=L_max(cor[n],cor[n]);

    /* INITIALIZE VARIABLES BEforE SEARCHING for INTERPOLATED PEAK */
    im=0;
    cor2m_exp = cor2_exp[n];
    cor2m = cor2[n];
    energym_exp = energy_exp[n];
    energym_man = energy_man[n];
    eni=L_max(energy[n],energy[n]);
#if WMOPS
    move16();move16();move16();move16();move16();move16();
#endif

    /* DERTERMINE WHICH SIDE THE INTERPOLATED PEAK FALLS IN, THEN
       do THE SEARCH IN THE APPROPRIATE RANGE */

    a0	 = L_mult(energy_man[n-1],cor2[n+1]);
    a1 	 = L_mult(energy_man[n+1], cor2[n-1]);
#ifdef PLATFORM
	exp0 = SSAT16_SHIFTL_1(sub(cor2_exp[n + 1], cor2_exp[n - 1]));
#else
    exp0 = shl(sub(cor2_exp[n+1], cor2_exp[n-1]),1);
#endif
    exp0 = add(exp0, energy_exp[n-1]);
    exp0 = sub(exp0, energy_exp[n+1]);
    if (exp0>=0)
      a0 = L_shr(a0, exp0);
    if (exp0<0)
      a1 = L_shl(a1, exp0);

    if (L_sub(a0, a1)>0)
    {	/* if right side */
#ifdef PLATFORM
	  deltae = SSAT_SHIFTR_3(L_sub(energy[n + 1], eni));
#else
      deltae = L_shr(L_sub(energy[n+1], eni), 3);
#endif
      for (k = 0; k < HDECF; k++)
      {
         a0=L_add(L_add(Mpy_32_16(ah,al,x2[k]),Mpy_32_16(bh,bl,x[k])),cc);
         eni = L_add(eni, deltae);
         a1 = L_max(eni, eni);
         exp0 = norm_l(a0);
         s0 = extract_h(L_shl(a0, exp0));
         s0 = extract_h(L_mult(s0, s0));
         e2 = energym_exp;
         t0 = energym_man;
         a2 = L_mult(t0, s0);
         e3 = norm_l(a1);
         t1 = extract_h(L_shl(a1, e3));
         a3 = L_mult(t1, cor2m);
#ifdef PLATFORM
		 exp1 = SSAT16_SHIFTL_1(sub(exp0, cor2m_exp));
#else
         exp1 = shl(sub(exp0, cor2m_exp),1);
#endif
         exp1 = add(exp1, e2);
         exp1 = sub(exp1, e3);

         if (exp1>=0)
            a2 = L_shr(a2, exp1);
         if (exp1<0)
            a3 = L_shl(a3, exp1);

         if (L_sub(a2, a3)>0)
         {
            im = add(k,1);
            cor2m = s0;
            cor2m_exp = exp0;
            energym_exp = e3;
            energym_man = t1;
#if WMOPS
            move16();;move16();move16();move16();
#endif
	      }
      }
    }
    else
    {    /* if interpolated peak is on the left side */
#ifdef PLATFORM
	  deltae = SSAT_SHIFTR_3(L_sub(energy[n - 1], eni));
#else
      deltae = L_shr(L_sub(energy[n-1], eni), 3);
#endif
      for (k = 0; k < HDECF; k++)
      {
	      a0=L_add(L_sub(Mpy_32_16(ah,al,x2[k]),Mpy_32_16(bh,bl,x[k])),cc);
	      eni = L_add(eni, deltae);
         a1 = L_max(eni, eni);

         exp0 = norm_l(a0);
         s0 = extract_h(L_shl(a0, exp0));
         s0 = extract_h(L_mult(s0, s0));
         e2 = energym_exp;
         t0 = energym_man;
         a2 = L_mult(t0, s0);
         e3 = norm_l(a1);
         t1 = extract_h(L_shl(a1, e3));
         a3 = L_mult(t1, cor2m);
#ifdef PLATFORM
		 exp1 = SSAT16_SHIFTL_1(sub(exp0, cor2m_exp));
#else
         exp1 = shl(sub(exp0, cor2m_exp),1);
#endif
         exp1 = add(exp1, e2);
         exp1 = sub(exp1, e3);
         if (exp1>=0)
            a2 = L_shr(a2, exp1);
         if (exp1<0)
            a3 = L_shl(a3, exp1);

         if (L_sub(a2, a3)>0)
         {
            im = negate(add(k,1));
            cor2m = s0;
            cor2m_exp = exp0;
            energym_exp = e3;
            energym_man = t1;
#if WMOPS
            move16();move16();move16();move16();
#endif
         }
      }
    }

    /* SEARCH doNE; ASSIGN cor2[] AND energy[] CORRESPONDING TO
       INTERPOLATED PEAK */
#ifdef PLATFORM
	plag[i] = add(SSAT16_SHIFTL_3(add(idx[i], 1)), im); /* lag of interp. peak */
#else
    plag[i]=add(shl(add(idx[i],1),3),im); /* lag of interp. peak */
#endif
    cor2i[i]=cor2m;
    cor2i_exp[i]=cor2m_exp;
    /* interpolated energy[] of i-th interpolated peak */
    energyi_exp[i] = energym_exp;
    energyi_man[i] = energym_man;
#if WMOPS
    move16();move16();move16();move16();move16();
#endif

    /* SEARCH for GLOBAL MAXIMUM OF INTERPOLATED cor2[]/energy[] peak */
    a0 = L_mult(cor2m,energymax_man);
    a1 = L_mult(cor2max, energyi_man[i]);
#ifdef PLATFORM
	exp0 = SSAT16_SHIFTL_1(sub(cor2m_exp, cor2max_exp));
#else
    exp0 = shl(sub(cor2m_exp, cor2max_exp),1);
#endif
    exp0 = add(exp0, energymax_exp);
    exp0 = sub(exp0, energyi_exp[i]);

    if (exp0 >=0)
      a0 = L_shr(a0, exp0);
    if (exp0<0)
      a1 = L_shl(a1, exp0);

    if (L_sub(a0,a1)>0)
    {
      imax=i;
      cor2max=cor2m;
      cor2max_exp=cor2m_exp;
      energymax_exp = energyi_exp[i];
      energymax_man = energyi_man[i];
#if WMOPS
      move16();move16();move16();move16();move16();
#endif
    }
  }

  cpp=plag[imax];	/* first candidate for coarse pitch period */
  mplth=plag[sub(npeaks,1)]; /* set mplth to the lag of last peak */
#if WMOPS
  move16();move16();
#endif

  /* FIND THE LARGEST PEAK (if THERE IS ANY) AROUND THE LAST PITCH */
#ifdef PLATFORM
  maxdev = SSAT16_SHIFTR_2(cpplast); /* maximum deviation from last pitch */
#else
  maxdev= shr(cpplast,2); /* maximum deviation from last pitch */
#endif
  im = -1;
  cor2m=(short) 0x8000;
  cor2m_exp= (short) 0;
  energym_man = 1;
  energym_exp = 0;
#if WMOPS
  move16();move16();move16();move16();move16();
#endif
  for (i=0;i<npeaks;i++)
  {  /* loop through the peaks before the largest peak */
    if (sub(abs_s(sub(plag[i],cpplast)), maxdev)<=0)
    {
      a0 = L_mult(cor2i[i],energym_man);
      a1 = L_mult(cor2m, energyi_man[i]);
#ifdef PLATFORM
	  exp0 = SSAT16_SHIFTL_1(sub(cor2i_exp[i], cor2m_exp));
#else
      exp0 = shl(sub(cor2i_exp[i], cor2m_exp),1);
#endif
      exp0 = add(exp0, energym_exp);
      exp0 = sub(exp0, energyi_exp[i]);
      if (exp0 >=0)
         a0 = L_shr(a0, exp0);
      if (exp0<0)
         a1 = L_shl(a1, exp0);
      if (L_sub(a0, a1)>0)
      {
         im=i;
         cor2m=cor2i[i];
         cor2m_exp=cor2i_exp[i];
         energym_man = energyi_man[i];
         energym_exp = energyi_exp[i];
#if WMOPS
         move16();move16();move16();move16();move16();
#endif
      }
    }
  } /* if there is no peaks around last pitch, then im is still -1 */

  /* NOW SEE if WE SHOULD PICK ANY ALTERNATICE PEAK. */
  /* FIRST, SEARCH FIRST HALF OF PITCH RANGE, SEE if ANY QUALifIED PEAK
     HAS LARGE ENOUGH PEAKS AT EVERY MULTIPLE OF ITS LAG */
  i=0;
#if WMOPS
  move16();
#endif
#ifdef PLATFORM
  while (sub(SSAT16_SHIFTL_1(plag[i]), mplth)<0)
#else
  while (sub(shl(plag[i],1), mplth)<0)
#endif
  {

    /* DETERMINE THE APPROPRIATE THRESHOLD for THIS PEAK */
     t1 = sub(i,im);
    if (t1!=0)
    {  /* if not around last pitch, */
      threshold = TH1;    /* use a higher threshold */
#if WMOPS
      move16();
#endif
    }
    if (t1==0)
    {        /* if around last pitch */
      threshold = TH2;    /* use a lower threshold */
#if WMOPS
      move16();
#endif
    }

    /* if THRESHOLD EXCEEDED, TEST PEAKS AT MULTIPLES OF THIS LAG */
    a0 = L_mult(cor2i[i],energymax_man);
    t1 = extract_h(L_mult(energyi_man[i], threshold));
    a1 = L_mult(cor2max, t1);
#ifdef PLATFORM
	exp0 = SSAT16_SHIFTL_1(sub(cor2i_exp[i], cor2max_exp));
#else
    exp0 = shl(sub(cor2i_exp[i], cor2max_exp),1);
#endif
    exp0 = add(exp0, energymax_exp);
    exp0 = sub(exp0, energyi_exp[i]);
    if (exp0 >=0) a0 = L_shr(a0, exp0);

    if (exp0 <0) a1 = L_shl(a1, exp0);

    if (L_sub(a0, a1)>0)
    {
      flag=1;
      j=add(i,1);
      k=0;
#if WMOPS
      move16();move16();
#endif
#ifdef PLATFORM
	  s = SSAT16_SHIFTL_1(plag[i]); /* initialize s to twice the current lag */
#else
      s=shl(plag[i],1); /* initialize s to twice the current lag */
#endif
      while (sub(s,mplth)<=0)
      { /* loop thru all multiple lag <= mplth */
         mpflag=0;   /* initialize multiple pitch flag to 0 */
#if WMOPS
         move16();
#endif
         t0 = mult_r(s,MPDTH);
         a=sub(s, t0);   /* multiple pitch range lower bound */
         b=add(s, t0);   /* multiple pitch range upper bound */
         for (;j<npeaks;j++)
         { /* loop thru peaks with larger lags */
            if (sub(plag[j],b)>0) { /* if range exceeded, */
	            break;          /* break the innermost loop */

            }       /* if didn't break, then plag[j] <= b */
            if (sub(plag[j],a)>0)
            { /* if current peak lag within range, */
               /* then check if peak value large enough */
               a0 = L_mult(cor2i[j],energymax_man);
               tt = sub(k,4);
               if (tt<0)
               {
                  t1 = MPTH[k];
#if WMOPS
                  move16();
#endif
               }
               if (tt>=0)
               {
                  t1 = MPTH4;
#if WMOPS
                  move16();
#endif
               }
               t1 = extract_h(L_mult(t1, energyi_man[j]));
               a1 = L_mult(cor2max, t1);
#ifdef PLATFORM
			   exp0 = SSAT16_SHIFTL_1(sub(cor2i_exp[j], cor2max_exp));
#else
               exp0 = shl(sub(cor2i_exp[j], cor2max_exp),1);
#endif
               exp0 = add(exp0, energymax_exp);
               exp0 = sub(exp0, energyi_exp[j]);
               if (exp0 >=0)
                  a0 = L_shr(a0, exp0);
               if (exp0<0)
                  a1 = L_shl(a1, exp0);
               if (L_sub(a0,a1)>0)
               {
                  mpflag=1; /* if peak large enough, set mpflag, */
#if WMOPS
                  move16();
#endif
                  break; /* and break the innermost loop */
               }
            }
         }
         /* if no qualified peak found at this multiple lag */
         if (mpflag == 0)
         {
            flag=0;     /* disqualify the lag plag[i] */
#if WMOPS
            move16();
#endif
            break;      /* and break the while (s-mplth<=0) loop */
         }
         k=add(k,1);
         s = add(s, plag[i]); /* update s to the next multiple pitch lag */
      }

      /* if there is a qualified peak at every multiple of plag[i], */
      if (sub(flag,1)==0)
      {
         cpp = plag[i]; /* accept this as final coarse pitch period */
#if WMOPS
         move16();
#endif
		   return cpp;         /* return to calling function */
      }
   }
   i=add(i,1);
   if (sub(i,npeaks)==0)
      break;      /* to avoid out of array bound error */
  }

  /* if PROGRAM PROCEEDS TO HERE, NONE OF THE PEAKS WITH LAGS < 0.5*mplth
     QUALifIES AS THE FINAL COARSE PITCH PERIOD. IN THIS CASE, CHECK if
     THERE IS ANY PEAK LARGE ENOUGH AROUND LAST COARSE PITCH PERIOD.
     if SO, USE ITS LAG AS THE FINAL COARSE PITCH PERIOD. */
  if (add(im,1)!=0)
  {   /* if there is at least one peak around last coarse pitch period */
     tt=sub(im, imax);
    if (tt==0)
    { /* if this peak is also the global maximum, */
      return cpp;   /* return first pitch candidate at global maximum */
    }
    if (tt<0)
    { /* if lag of this peak < lag of global maximum, */
      a0 = L_mult(cor2m,energymax_man);
      t1 = extract_h(L_mult(energym_man, LPTH2));
      a1 = L_mult(cor2max, t1);
#ifdef PLATFORM
	  exp0 = SSAT16_SHIFTL_1(sub(cor2m_exp, cor2max_exp));
#else
      exp0 = shl(sub(cor2m_exp, cor2max_exp),1);
#endif
      exp0 = add(exp0, energymax_exp);
      exp0 = sub(exp0, energym_exp);
      if (exp0 >=0)
         a0 = L_shr(a0, exp0);
      if (exp0<0)
         a1 = L_shl(a1, exp0);
      if (L_sub(a0,a1)>0)
      {
	      if (sub(plag[im], i_mult(HMAXPPD,cpp_scale))>0)
         {
            cpp = plag[im];
#if WMOPS
            move16();
#endif

	  	      return cpp;
         }
	      for (k=2; k<=5;k++)
         { /* check if current candidate pitch */
            s=mult(plag[imax],invk[k-2]); /* is a sub-multiple of */
            t0 = mult_r(s,SMDTH);
            a=sub(s, t0);  		/* the time lag of */
            b=add(s, t0);       /* the global maximum peak */
#ifdef WMOPS
            test();
#endif
            if (sub(plag[im],a)>0 && sub(plag[im],b)<0)
            {     /* if so, */
               cpp = plag[im];		/* accept this lag */
#if WMOPS
               move16();
#endif
	            return cpp;         /* and return as pitch */
            }
         }
      }
    }
    else
    {           /* if lag of this peak > lag of global max, */
      a0 = L_mult(cor2m,energymax_man);
      t1 = extract_h(L_mult(energym_man, LPTH1));
      a1 = L_mult(cor2max, t1);
#ifdef PLATFORM
	  exp0 = SSAT16_SHIFTL_1(sub(cor2m_exp, cor2max_exp));
#else
      exp0 = shl(sub(cor2m_exp, cor2max_exp),1);
#endif
      exp0 = add(exp0, energymax_exp);
      exp0 = sub(exp0, energym_exp);
      if (exp0 >=0)
         a0 = L_shr(a0, exp0);
      if (exp0<0)
         a1 = L_shl(a1, exp0);
      if (L_sub(a0,a1)>0)
      {
#if WMOPS
         move16();
#endif
         cpp = plag[im];	/* accept its lag */
		   return cpp;
      }
    }
  }

  /* if PROGRAM PROCEEDS TO HERE, WE HAVE NO CHOICE BUT TO ACCEPT THE
     LAG OF THE GLOBAL MAXIMUM */
  return cpp;

}

/*-----------------------------------------------------------------------------
 * Function: Autocorr()
 *
 * Description: Calculates autocorrelation.
 *
 * Inputs:  x[]      - Input signal
 *          window[] - LPC Analysis window
 *          l_window - window length
 *          m        - LPC order
 *
 * Outputs: r[]      - Autocorrelations
 *---------------------------------------------------------------------------*/
void Autocorr(
int	  r[],
short	x[],
const short	window[],
short l_window,
short m)
{
  short i, j, norm;
#if (DMEM)
  short *y;
#else
  short y[WINSZ];
#endif
  int sum;
  short lw;

#if (DMEM)
  /* memory allocation */
  y = allocshort(0, l_window-1);
#endif

  /* Windowing of signal */

  for(i=0; i<l_window; i++)
  {
    y[i] = mult_r(x[i], window[i]);
#ifdef WMOPS
    move16();
#endif
  }

  /* Compute r[0] and test for overflow */

  do {
    __set_Overflow(0); //Overflow = 0;
    sum = 1;                   /* Avoid case of all zeros */
#if WMOPS
    move16(); move16();
#endif
    for(i=0; i<l_window; i++)
      sum = L_mac0(sum, y[i], y[i]);

    /* if overflow divide y[] by 4 */

    if (__get_Overflow()/*Overflow*/) {

      for(i=0; i<l_window; i++)
      {
#ifdef PLATFORM
		 y[i] = SSAT16_SHIFTR_2(y[i]);
#else
         y[i] = shr(y[i], 2);
#endif
#ifdef WMOPS
         move16();
#endif
      }
    }

  } while (__get_Overflow()/*Overflow*/);

  /* Normalization of r[0] */

  norm = norm_l(sum);
  r[0]  = L_shl(sum, norm);
#ifdef WMOPS
  move16();
#endif
  /* r[1] to r[m] */

  for (i = 1; i <= m; i++)
  {
    sum=L_mult0(y[0],y[i]);
    lw = sub(l_window, i);
    for(j=1; j<lw; j++) sum = L_mac0(sum, y[j], y[j+i]);

    r[i] = L_shl(sum, norm);
#ifdef WMOPS
    move16();
#endif
  }

#if (DMEM)
  /* dememory allocation */
  deallocshort(y, 0, l_window-1);
#endif

}


/*-----------------------------------------------------------------------------
 * Function: Spectral_Smoothing()
 *
 * Description: Performs spectral smoothing on the autocorrelation coefficients.
 *
 * Inputs:  m       - LPC order
 *          r[]     - Autocorrelations
 *          lag_h[] - SST coefficients  (msb)
 *          lag_l[] - SST coefficients  (lsb)
 *
 * Outputs: r[]     - Autocorrelations
 *---------------------------------------------------------------------------*/
void Spectral_Smoothing(
  short m,
  int r[],
  const short lag_h[],
  const short lag_l[]
)
{
  short 	i;
  short	hi, lo;

#if WMOPS
  move16(); /* for loading of lag_h[i-1] pointer */
  move16(); /* for loading of lag_l[i-1] pointer */
#endif
  for(i=1; i<=m; i++)
  {
  	 L_Extract(r[i], &hi, &lo);
    r[i] = Mpy_32(hi, lo, lag_h[i-1], lag_l[i-1]);
#ifdef WMOPS
    move16();
#endif
  }
}
/*___________________________________________________________________________
 |                                                                           |
 |      LEVINSON-DURBIN algorithm in double precision                        |
 |      ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~                        |
 |---------------------------------------------------------------------------|
 |                                                                           |
 | Algorithm                                                                 |
 |                                                                           |
 |       R[i]    autocorrelations.                                           |
 |       A[i]    filter coefficients.                                        |
 |       K       reflection coefficients.                                    |
 |       Alpha   prediction gain.                                            |
 |                                                                           |
 |       Initialization:                                                     |
 |               A[0] = 1                                                    |
 |               K    = -R[1]/R[0]                                           |
 |               A[1] = K                                                    |
 |               Alpha = R[0] * (1-K**2]                                     |
 |                                                                           |
 |       do for  i = 2 to m                                                  |
 |                                                                           |
 |            S =  SUM ( R[j]*A[i-j] ,j=1,i-1 ) +  R[i]                      |
 |                                                                           |
 |            K = -S / Alpha                                                 |
 |                                                                           |
 |            An[j] = A[j] + K*A[i-j]   for j=1 to i-1                       |
 |                                      where   An[i] = new A[i]             |
 |            An[i]=K                                                        |
 |                                                                           |
 |            Alpha=Alpha * (1-K**2)                                         |
 |                                                                           |
 |       END                                                                 |
 |                                                                           |
 | Remarks on the dynamics of the calculations.                              |
 |                                                                           |
 |       The numbers used are in double precision in the following format :  |
 |       A = AH <<16 + AL<<1.  AH and AL are 16 bit signed integers.         |
 |       Since the LSB's also contain a sign bit, this format does not       |
 |       correspond to standard 32 bit integers.  We use this format since   |
 |       it allows fast execution of multiplications and divisions.          |
 |                                                                           |
 |       "DPF" will refer to this special format in the following text.      |
 |       See oper_32b.c                                                      |
 |                                                                           |
 |       The R[i] were normalized in routine AUTO (hence, R[i] < 1.0).       |
 |       The K[i] and Alpha are theoretically < 1.0.                         |
 |       The A[i], for a sampling frequency of 8 kHz, are in practice        |
 |       always inferior to 16.0.                                            |
 |                                                                           |
 |       These characteristics allow straigthforward fixed-point             |
 |       implementation.  We choose to represent the parameters as           |
 |       follows :                                                           |
 |                                                                           |
 |               R[i]    Q31   +- .99..                                      |
 |               K[i]    Q31   +- .99..                                      |
 |               Alpha   Normalized -> mantissa in Q31 plus exponent         |
 |               A[i]    Q27   +- 15.999..                                   |
 |                                                                           |
 |       The additions are performed in 32 bit.  for the summation used      |
 |       to calculate the K[i], we multiply numbers in Q31 by numbers        |
 |       in Q27, with the result of the multiplications in Q27,              |
 |       resulting in a dynamic of +- 16.  This is sufficient to avoid       |
 |       overflow, since the final result of the summation is                |
 |       necessarily < 1.0 as both the K[i] and Alpha are                    |
 |       theoretically < 1.0.                                                |
 |___________________________________________________________________________|
*/


/* Last A(z) for case of unstable filter */

#define	M	16

void Levinson(
  Word32 Rhl[],     /* (i)       : Rh[M+1] Vector of autocorrelations (msb) */
  Word16 A[],       /* (o) Q12   : A[M]    LPC coefficients                 */
  Word16 old_A[],   /* (i/o) Q12 : old_A[M+1] old LPC coefficients          */
  Word16 m	        /* (i)       : LPC order				    */
)
{
 Word16 i, j;
 Word16 hi, lo;
 Word16 Kh, Kl;                /* reflection coefficient; hi and lo           */
 Word16 alp_h, alp_l, alp_exp; /* Prediction gain; hi lo and exponent         */
 Word32 t0, t1, t2;            /* temporary variable                          */
 Word16  *pAh, *pAl;
 Word16 unstable;

#if (DMEM)
 Word16 *Ah, *Al;      /* LPC coef. in double prec.                   */
 Word16 *Anh, *Anl;    /* LPC coef.for next iteration in double prec. */
#else
 Word16 Ah[M+1], Al[M+1];      /* LPC coef. in double prec.                   */
 Word16 Anh[M+1], Anl[M+1];    /* LPC coef.for next iteration in double prec. */
#endif

/* K = A[1] = -R[1] / R[0] */

#if (DMEM)
  /* memory allocation */
  Ah  = allocWord16(0, m);
  Al  = allocWord16(0, m);
  Anh = allocWord16(0, m);
  Anl = allocWord16(0, m);
#endif

  t1  = Rhl[1]; 				        /* R[1] in Q31      */
  unstable = 0;
#if WMOPS
  move32();move16();
#endif

  t2  = L_abs(t1);                      /* abs R[1]         */
  L_Extract(Rhl[0], &alp_h, &alp_l);	 /* borrow alp for Rhl[0] */
  t0  = Div_32(t2, alp_h, alp_l);       /* R[1]/R[0] in Q31 */
  if (t1 > 0) t0= L_negate(t0);         /* -R[1]/R[0]       */
  L_Extract(t0, &Kh, &Kl);              /* K in DPF         */
#ifdef PLATFORM
  t0 = SSAT_SHIFTR_4(t0);                     /* A[1] in Q27      */
#else
  t0 = L_shr(t0,4);                     /* A[1] in Q27      */
#endif
  L_Extract(t0, &Ah[1], &Al[1]);        /* A[1] in DPF      */

/*  Alpha = R[0] * (1-K**2) */

  t0 = Mpy_32(Kh ,Kl, Kh, Kl);          /* K*K      in Q31 */
  t0 = L_abs(t0);                       /* Some case <0 !! */
  t0 = L_sub( (Word32)0x7fffffffL, t0 );/* 1 - K*K  in Q31 */
  L_Extract(t0, &hi, &lo);              /* DPF format      */
  t0 = Mpy_32(alp_h , alp_l, hi, lo);   /* Alpha in Q31    */

/* Normalize Alpha */

  alp_exp = norm_l(t0);
  t0 = L_shl(t0, alp_exp);
  L_Extract(t0, &alp_h, &alp_l);         /* DPF format    */

/*--------------------------------------*
 * ITERATIONS  I=2 to m                 *
 *--------------------------------------*/

  for (i= 2; i<=m; i++)
  {

    /* t0 = SUM ( R[j]*A[i-j] ,j=1,i-1 ) +  R[i] */

    L_Extract(Rhl[1], &hi, &lo);
     pAh = Ah+i-1;
     pAl = Al+i-1;
     t0 = Mpy_32(hi, lo, *pAh--, *pAl--);
     for(j=2; j<i; j++) {
	      L_Extract(Rhl[j], &hi, &lo);
         t0 = L_add(t0, Mpy_32(hi, lo, *pAh--, *pAl--));
	}

    t0 = L_shl(t0,4);                  /* result in Q27 -> convert to Q31 */
                                       /* No overflow possible            */
    t1 = Rhl[i];
    t0 = L_add(t0, t1);                /* add R[i] in Q31                 */

    /* K = -t0 / Alpha */

    t1 = L_abs(t0);
    t2 = Div_32(t1, alp_h, alp_l);     /* abs(t0)/Alpha                   */
    if(t0 > 0) t2= L_negate(t2);       /* K =-t0/Alpha                    */
    t2 = L_shl(t2, alp_exp);           /* denormalize; compare to Alpha   */
    L_Extract(t2, &Kh, &Kl);           /* K in DPF                        */

    /* Test for unstable filter.*/
    if (sub(abs_s(Kh), 32750) > 0)
         unstable = 1;

    /*------------------------------------------*
     *  Compute new LPC coeff. -> An[i]         *
     *  An[j]= A[j] + K*A[i-j]     , j=1 to i-1 *
     *  An[i]= K                                *
     *------------------------------------------*/

    pAh = Ah+i-1;
    pAl = Al+i-1;
    for(j=1; j<i; j++)
    {
      t0 = Mpy_32(Kh, Kl, *pAh--, *pAl--);
      t0 = L_add(t0, L_Comp(Ah[j], Al[j]));
      L_Extract(t0, &Anh[j], &Anl[j]);
    }
#ifdef PLATFORM
	t2 = SSAT_SHIFTR_4(t2);                  /* t2 = K in Q31 ->convert to Q27  */
#else
    t2 = L_shr(t2, 4);                  /* t2 = K in Q31 ->convert to Q27  */
#endif
    L_Extract(t2, &Anh[i], &Anl[i]);    /* An[i] in Q27                    */

    /*  Alpha = Alpha * (1-K**2) */

    t0 = Mpy_32(Kh ,Kl, Kh, Kl);          /* K*K      in Q31 */
    t0 = L_abs(t0);                       /* Some case <0 !! */
    t0 = L_sub( (Word32)0x7fffffffL, t0 );/* 1 - K*K  in Q31 */
    L_Extract(t0, &hi, &lo);              /* DPF format      */
    t0 = Mpy_32(alp_h , alp_l, hi, lo);   /* Alpha in Q31    */

    /* Normalize Alpha */

    j = norm_l(t0);
    t0 = L_shl(t0, j);
    L_Extract(t0, &alp_h, &alp_l);         /* DPF format    */
    alp_exp = add(alp_exp, j);             /* Add normalization to alp_exp */

    /* A[j] = An[j] */

    for(j=1; j<=i; j++)
    {
      Ah[j] =Anh[j];
      Al[j] =Anl[j];
#if WMOPS
      move16();move16();
#endif
    }
  }
  /* Test for unstable filter. if unstable keep old A(z) */
  if (unstable)
  {
      for(j=0; j<=m; j++)
      {
        A[j] = old_A[j];
#ifdef WMOPS
        move16();
#endif
      }

#if (DMEM)
      /* memory deallocation */
      deallocWord16(Ah,  0, m);
      deallocWord16(Al,  0, m);
      deallocWord16(Anh, 0, m);
      deallocWord16(Anl, 0, m);
#endif

      return;
  }

  /* Truncate A[i] in Q27 to Q12 with rounding */

  A[0] = 4096;
#if WMOPS
  move16();
#endif
  for(i=1; i<=m; i++)
  {
    t0   = L_Comp(Ah[i], Al[i]);
    old_A[i] = A[i] = roundInt(L_shl(t0, 1));
#if WMOPS
    move16();move16();
#endif
  }

#if (DMEM)
  /* memory deallocation */
  deallocWord16(Ah,  0, m);
  deallocWord16(Al,  0, m);
  deallocWord16(Anh, 0, m);
  deallocWord16(Anl, 0, m);
#endif

  return;
}

/*-----------------------------------------------------------------------------
 * Function: azfilterQ0_Q1()
 *
 * Description: Performs all-zero filtering with Q0 in and Q1 out.
 *
 * Inputs:  a[]   - prediction coefficients, Q12
 *          m     - LPC order
 *          x[]   - input signal
 *          lg    - size of filtering
 *
 * Outputs: y[]   - output signal
 *---------------------------------------------------------------------------*/
void azfilterQ0_Q1(
  Word16 a[],
  Word16 m,
  Word16 x[],
  Word16 y[],
  Word16 lg
)
{
  Word16 i;
  Word32 s;

   for (i = 0; i < lg; i++)
   {
      s = L_mult0(x[i], a[0]); /* Q12 */
      s = L_mac0(s, a[1], x[i-1]);
      s = L_mac0(s, a[2], x[i-2]);
      s = L_mac0(s, a[3], x[i-3]);
      s = L_mac0(s, a[4], x[i-4]);
      s = L_mac0(s, a[5], x[i-5]);
      s = L_mac0(s, a[6], x[i-6]);
      s = L_mac0(s, a[7], x[i-7]);
      s = L_mac0(s, a[8], x[i-8]);

      y[i] = roundInt(L_shl(s, 5)); /* Q1 */
#ifdef WMOPS
      move16();
#endif
   }
}

/*-----------------------------------------------------------------------------
 * Function: decim()
 *
 * Description: DECIMate the weighted speech signal from 16 kHz to 2 kHz.
 *              The 1 kHz anti-aliasing low-pass filtering is performed with
 *              a 60th-order minimum-phase FIR filter. The decimation filtering
 *              operation is performed only once every 8 samples -- only when
 *              an output 2 kHz signal sample is needed.
 *
 * Inputs:  *xw   - pointer to 16 kHz weighted speech buffer
 *          *xwd  - pointer to 2 kHz decimated weighted speech buffer
 *          *cstate - data structure containing the states of G.722 PLC
 * Outputs: *xwd  - pointer to 2 kHz decimated weighted speech buffer
 *---------------------------------------------------------------------------*/
void decim(
Word16 	*xw,
Word16	*xwd,
struct WB_PLC_State *cstate)
{
   Word32    a0, a1;
   Word32    *lp0;
   Word16    exp, new_exp;
   Word16    *fp0;
   Word16    exp0, exp1;
   Word16    i, j;
   Word16    *fp;
#if (DMEM)
   Word32 *lxwd;
#else
   Word32 lxwd[FRSZD];
#endif

   a1 = L_mult(1,1);

   /* load decimation filter memory to beginning part of xw[] array */
   W16copy(xw, cstate->dfm, DFO);

#if (DMEM)
   lxwd = allocWord32(0, FRSZD-1);
#endif

   /* low-pass filtering of xw[] every 8th sample, save output to lxwd[] */
   lp0 = lxwd;
   for(i=7;i<FRSZ;i=i+8)
   {
      fp = &xw[DFO+i];
      a0 = L_mult0(bdf[0],*fp--);
      for (j=0;j<DFO-1;j++)
         a0 = L_mac0(a0,bdf[j+1],*fp--);
      *lp0++=a0;
#if WMOPS
      move32();
#endif
      a0 = L_abs(a0);
      a1 = L_max(a0,a1);
   }
   /* update decimation filter state memory */
   W16copy(cstate->dfm, xw+FRSZ, DFO);

   /* setup local xwd[] */
   lp0 = lxwd;
   new_exp = sub(norm_l(a1), 3);
   exp = sub(cstate->xwd_exp, new_exp);

   if (exp < 0)
   {
      new_exp = cstate->xwd_exp;
#if WMOPS
      move16();
#endif
   }
   exp = s_max(exp, 0);

   for (i=0;i<XDOFF;i++)
   {
      xwd[i] = shr(cstate->xwd[i], exp);
#ifdef WMOPS
      move16();
#endif
   }

   fp0 = &xwd[XDOFF];
   for (i=0;i<FRSZD;i++)
   {
      fp0[i] = roundInt(L_shl(lp0[i],new_exp));
#ifdef WMOPS
      move16();
#endif
   }

#if (DMEM)
   /* memory deallocation */
   deallocWord32(lxwd, 0, FRSZD-1);
#endif

   /* update xwd() memory */
   exp0 = 1;
#if WMOPS
   move16();
#endif
   for (i=0;i<XDOFF;i++)
   {
      exp1 = abs_s(xwd[FRSZD+i]);
      exp0 = s_max(exp0, exp1);
   }
   exp0 = sub(norm_s(exp0),3);
   exp = sub(exp0, exp);

   for (i=0;i<XDOFF-FRSZD;i++)
   {
      cstate->xwd[i] = shl(cstate->xwd[i+FRSZD], exp);
#ifdef WMOPS
      move16();
#endif
   }
   for (;i<XDOFF;i++)
   {
      cstate->xwd[i] = shl(xwd[FRSZD+i],exp0);
#ifdef WMOPS
      move16();
#endif
   }

   cstate->xwd_exp = add(new_exp, exp0);
#ifdef WMOPS
   move16();
#endif
}


/*-----------------------------------------------------------------------------
 * Function: prfn()
 *
 * Description: Pitch period ReFiNement.
 *              This function performs a refinement search around the coarse
 *              pitch period with the 16 kHz time resolution based on the
 *              16 kHz G.722/PLC output speech signal.
 *
 * Inputs:  *xq - pointer to 16 kHz output speech buffer
 *          cpp - coarse pitch period in 16 kHz time resolution
 * Outputs: (return value of the function) - refined pitch period
 *          *ptfe - Pitch Tap for Frame Erasure (scaling factor for PWE)
 *          *cormax - CORrelation MAXimum during pitch refinement search
 *          *energymax32 - ENERGY MAXimum during pitch refinement search
 *          *ppt - Pitch Predictor Tap for calculating long-term ringing
 *          *wsz - adaptive Window SiZe chosen for pitch refinement
 *          *sflag - Shift FLAG for xq[]
 *---------------------------------------------------------------------------*/
Word16  prfn(
Word16  *ptfe,      /* (o) Q14 pitch tap */
Word32  *cormax,
Word32  *energymax32,
Word16  *ppt,
Word16  *wsz,
Word16  *sflag,
Word16  *xq,        /* (i) quantized signal from last sub-frame */
Word16  cpp)        /* (i) pitch period from last subframe */
{
	Word32	a0, a1, amy;
	Word16	*xqp, *pp;
	Word16   *x, *y, *fp0, *fp1;
	Word16	s, t, lasts;
	Word16	cor2max, cor2max_exp;
	Word16	energymax, energymax_exp;
	Word16	ener, ener_exp;
	Word16	cor2, cor2_exp;
	Word32	energy, cor;
	Word32   cormax_local, energymax32_local;
	Word16 	lb, ub, ppfe, tt;
	Word16	i, k;
   Word16   shift;

#if (DMEM)
	Word16 *xqbuf;
#else
	Word16 xqbuf[xqoff];
#endif

#if (DMEM)
	/* memory allocation */
	xqbuf = allocWord16(0, xqoff-1);
#endif

	*wsz = s_min(cpp,WML);
	lb = sub(cpp, 3);
	lb = s_max(lb, MINPP); /* lower bound of pitch period search range */
	ub = add(cpp, 3);
	ub = s_min(ub, MAXPP); /* upper bound of pitch period search range */

	/* CHECK if THE SIGNAL SEGMENT CAUSES OVERFLOW */
	energy = 0;
#if WMOPS
   move16();
#endif
	fp1 = &xq[sub(sub(xqoff, (*wsz)), lb)];
	for (k=0;k<*wsz;k++)
   {
#ifdef PLATFORM
	    t = SSAT16_SHIFTR_3(*fp1++);
#else
	    t = shr(*fp1++, 3);
#endif
	    energy = L_mac0(energy,t,t);
	}
   shift = norm_l(energy);
   shift = sub(6, shift);
   if (shift > 0)
   {
      /* memory allocation */
      xqp = xqbuf;
#ifdef PLATFORM
	  shift = SSAT16_SHIFTR_1(add(shift, 1));
#else
      shift = shr(add(shift, 1), 1);
#endif
      *sflag = shift;
#if WMOPS
      move16();
#endif
      for (i=0;i<xqoff;i++)
      {
         xqbuf[i] = shr(xq[i],shift);
#ifdef WMOPS
         move16();
#endif
      }
	} else {
	    xqp = xq;
		 *sflag = 0;
#if WMOPS
       move16();
#endif
	}

    /* HANDLE THE FIRST CANDIDATE OUT OF THE LOOP */
	x = &xqp[sub(xqoff, (*wsz))];				/* target vector for the search */
	fp0 = x;
	fp1 = &xqp[sub(sub(xqoff, (*wsz)),lb)];
   energy=L_mult0(*fp1,*fp1);
   cor = L_mult0(*fp0++,*fp1++);
	for (k=1;k<*wsz;k++) {
		energy = L_mac0(energy,*fp1,*fp1);
		cor = L_mac0(cor,*fp0++,*fp1++);
	}

	cor = L_max(cor, -1);
  	cormax_local=L_max(cor,cor);
	cor2max_exp = norm_l(cor);
	s = extract_h(L_shl(cor, cor2max_exp));
#ifdef PLATFORM
	cor2max_exp = SSAT16_SHIFTL_1(cor2max_exp);
#else
	cor2max_exp = shl(cor2max_exp, 1);
#endif
	cor2max = extract_h(L_mult(s, s));
	energymax32_local = L_max(energy,energy);
	energymax_exp = norm_l(energy);
	energymax = extract_h(L_shl(energy, energymax_exp));
  	ppfe=lb;
#if WMOPS
	move16();
#endif

    /* NOW SEARCH THE REST OF CANDIDATES for MAXIMUM PITCH PREDICTION GAIN */
   pp = &xqp[sub(sub(xqoff, (*wsz)), (Word16)(lb+1))];
	for (k=lb+1;k<=ub;k++) {
		fp0 = x;
		fp1 = pp--;
		lasts = *fp1;
      cor = L_mult0(*fp0++,*fp1++);
#if WMOPS
      move16();
#endif
		for (i=1;i<(*wsz);i++)
      {
			cor = L_mac0(cor,*fp0++,*fp1++);
		}

		energy = L_msu0(energy,*fp1,*fp1);
		energy = L_mac0(energy,lasts,lasts);

		cor2_exp = norm_l(cor);
		s = extract_h(L_shl(cor, cor2_exp));
#ifdef PLATFORM
		cor2_exp = SSAT16_SHIFTL_1(cor2_exp);
#else
		cor2_exp = shl(cor2_exp, 1);
#endif
		cor2 = extract_h(L_mult(s, s));
		ener_exp = norm_l(energy);
		ener = extract_h(L_shl(energy, ener_exp));

#if WMOPS
      test(); /* for the conditions in the if below */
#endif
		if ((cor > 0) && (ener>0)) {
			a0 = L_mult(cor2, energymax);
			a1 = L_mult(cor2max, ener);
			s = add(cor2_exp, energymax_exp);
			t = add(cor2max_exp, ener_exp);

         tt = sub(s, t);
			if (tt>=0) a0 = L_shr(a0, tt);
         if (tt<0)  a1 = L_shl(a1, tt);

			if (L_sub(a0, a1)>0) {
				cormax_local=L_max(cor,cor);
				cor2max = cor2; cor2max_exp = cor2_exp;
				energymax = ener; energymax_exp = ener_exp;
				energymax32_local = L_max(energy,energy);
				ppfe=k;
#if WMOPS
				move16();move16();move16();move16();move16();
#endif
			}
		}
	}

    /* USE THE RATIO OF AVERAGE MAGNITUDE AS THE SCALING FACTOR (PITCH TAP) */
	y = &xqp[sub(sub(xqoff, (*wsz)), ppfe)];/* candidate vector at delay ppfe */
	amy = L_mult(0,0);
	fp0 = y;
	for (k=0;k<(*wsz);k++)
   {
      s = abs_s(*fp0++);
      amy = L_add(amy, (Word32)s);
	}

	if (amy == 0)
   {
      t = 0;
#if WMOPS
      move16();
#endif
   }
	else {
		fp0 = x;
		a1 = L_mult(0,0);
		for (k=0;k<(*wsz);k++) {
         s = abs_s(*fp0++);
         a1 = L_add(a1,(Word32)s);
		}

		/* t = (a1/amy); */
		ub = sub(norm_l(a1),1);
		lb = norm_l(amy);
		t = extract_h(L_shl(a1,ub));
		s = extract_h(L_shl(amy,lb));
		t = div_s(t, s);
		lb = sub(sub(lb,ub),1);		/* 15-14=1 */
		t = shl(t, lb);
		if (cormax_local < 0) t = negate(t);
	}

    /* LIMIT THE RANGE OF PITCH TAP to [-1, 1] */
	t = s_min(t,UPBOUND);
	t = s_max(t, DWNBOUND);
	*ptfe = t;
#if WMOPS
	move16();
#endif

	t = mult(768, *ptfe); /* Q10 * Q14 -> Q9 */
	t = s_max(t, 0);
	*ppt = t;
#if WMOPS
	move16();
#endif

#if (DMEM)
	/* memory deallocation */
	deallocWord16(xqbuf, 0, xqoff-1);
#endif

	*energymax32 = energymax32_local;
	*cormax = cormax_local;
#if WMOPS
	move32();move32();
#endif

	return ppfe;
}
/*-----------------------------------------------------------------------------
* Function: merit()
*
* Description: This function calculates the figure of MERIT which determines
*              the mixing ratio of periodically extrapolated waveform and
*              filtered white Gaussian noise.
*
* Inputs:  *xq - pointer to 16 kHz output speech buffer
*          wsz - adaptive Window SiZe chosen for pitch refinement
*          cormax - CORrelation MAXimum during pitch refinement search
*          energymax32 - ENERGY MAXimum during pitch refinement search
*          level - long-term average logarithmic signal level
*          sflag - Shift FLAG for xq[]
* Outputs: (return value of the function) - calculated figure of merit
*---------------------------------------------------------------------------*/
Word16  merit(
	Word16  *xq,                /* (i) quantized signal from last sub-frame */
	Word16  wsz,
	Word32  cormax,
	Word32  energymax32,
	Word16  sflag)              /* (i) signaling for if energymax were scaled */
{
	Word32	a0, a1;
	Word16	*xqp;
	Word16  *fp0;
	Word16	s, t;
	Word16	cor2max, cor2max_exp;
	Word16	energymax, energymax_exp;
	Word32	sige, rese, r1;
	Word32	sigel;	/* Q25 */
	Word16	sigel_exp, sigel_frct, rho1;
	Word16	pg;
	Word16	nlg;	   /* Q8 */
	Word16 	lb, ub;
	Word16	i, k;

#if (DMEM)
	Word16 *xqbuf;
#else
	Word16 xqbuf[XQOFF];
#endif

#if (DMEM)
	/* memory allocation */
	xqbuf = allocWord16(0, XQOFF - 1);
#endif

	if(sflag > 0)
	{
		xqp = xqbuf;
		for(i = 0; i < XQOFF; i++)
		{
			xqbuf[i] = shr(xq[i], sflag);
#ifdef WMOPS
			move16();
#endif
		}
	}
	else
	{
		xqp = xq;
	}

		/* CALCULATE LOG-GAIN, PITCH PREDICTION GAIN, & FIRST NORMALIZED AUTOCORR */
	sige = 0;
	r1 = 0;
	fp0 = &xqp[XQOFF - wsz];
#if WMOPS
	move16(); move16();
#endif
	for(k = 0; k < wsz; k++)
	{
		sige = L_mac0(sige, fp0[0], fp0[0]); /* prediction target SIGnal Energy */
		r1 = L_mac0(r1, fp0[0], fp0[-1]);    /* first autocorrelation coeff. */
		fp0++;
	}

	cor2max_exp = norm_l(cormax);
	s = extract_h(L_shl(cormax, cor2max_exp));
#ifdef PLATFORM
	cor2max_exp = SSAT16_SHIFTL_1(cor2max_exp);
#else
	cor2max_exp = shl(cor2max_exp, 1);
#endif
	cor2max = extract_h(L_mult(s, s));
	energymax_exp = norm_l(energymax32);
	energymax = extract_h(L_shl(energymax32, energymax_exp));

	if(sige != 0) {
		/* calculate base-2 logarithm of signal energy */
		Log2(sige, &sigel_exp, &sigel_frct);
		s = sigel_exp;
		//move16();
		if (sub(sflag, 1) == 0)
			s = add(s, 2);
		sigel = L_shl(L_Comp(s, sigel_frct), 9);	/* Q25 */

		if(energymax32 != 0) {

			/* calcualte pitch prediction residual energy "rese" */
			/* rese = sige-cormax*cormax/energymax; */
			/* t = cormax/energymax32; */
			a1 = L_abs(cormax);
			ub = sub(norm_l(a1), 1);
			lb = norm_l(energymax32);
			t = extract_h(L_shl(a1, ub));
			s = extract_h(L_shl(energymax32, lb));
			t = div_s(t, s);
			lb = sub(sub(lb, ub), 1);	/* Q14 */
			t = shl(t, lb);

			if (cormax < 0) t = negate(t);
			L_Extract(cormax, &ub, &lb);
			a0 = L_shl(Mpy_32_16(ub, lb, t), 1);	/* cormax*cormax/energymax */
			rese = L_sub(sige, a0);

			if(rese != 0) {

				/* calcualte pitch prediction gain */
				/* 10*log10(sige/rese) = 3.0103*(log2(sige)-log2(rese)) */
				Log2(rese, &s, &t);
				a0 = L_Comp(sigel_exp, sigel_frct);
				a1 = L_Comp(s, t);
				a0 = L_sub(a0, a1); /* Q16 */
				L_Extract(a0, &s, &t);
				a0 = Mpy_32_16(s, t, 24660);	/* 3.0103/4 Q15, a0 is Q14 */
				pg = roundInt(L_shl(a0, 11));		/* Q9 */
#ifdef PLATFORM
			}else pg = SSAT16_SHIFTL_9(20);	/* Q9 */
#else
			} else pg = shl(20, 9);	/* Q9 */
#endif
		} else
		{
			pg = 0;
#if WMOPS
			move16();
#endif
		}

			/* first normalized autocorrelation coefficient rho1 = r1/sige; */
		a1 = L_abs(r1);
		ub = sub(norm_l(a1), 1);
		lb = norm_l(sige);
		t = extract_h(L_shl(a1, ub));
		s = extract_h(L_shl(sige, lb));
		t = div_s(t, s);
		lb = sub(lb, ub);	/* Q15 */
		rho1 = shl(t, lb);

		if (r1 < 0) rho1 = negate(rho1);
	} else{
		sigel = 0;
		pg = 0;
		rho1 = 0;
#if WMOPS
		move16(); move16(); move16();
#endif
	}

		/* calculate the figure of merit: merit = nlg + pg + 12*rho1 */
#ifdef PLATFORM
	a0 = SSAT_SHIFTR_1(L_sub(sigel, (Word32)0x1b000000));		/* Q24 */
#else
	a0 = L_shr(L_sub(sigel, (Word32)0x1b000000), 1);		/* Q24 */
#endif
	nlg = roundInt(a0);		/* nlg = normalized logarithmic gain in Q8 format */
	a0 = L_mac(a0, pg, 16384);		/* 1.0 Q14, pg Q9 -> Q24 */
	a0 = L_mac(a0, rho1, 3072);		/* 12. Q8, rho1 Q15 -> Q24 */

#if (DMEM)
	/* memory deallocation */
	deallocWord16(xqbuf, 0, XQOFF - 1);
#endif

    // suppress compiler warnings
    (void) nlg;
    (void) energymax;
    (void) cor2max;

	return roundInt(a0);		/* Q8 merit */
}
/*-----------------------------------------------------------------------------
* Function: getlag()
*
* Description: Find the lag that maximizes the cross correlation between the
*              signal in x and the signal in esb.  Use a window of length
*              lsw and search +-delta samples.  The function getlag assumes
*              input vectors are properly shifted outside for optimal
*              saturation/precision tradeoff
*
* Inputs:  *x    - pointer to the fixed signal buffer
*          *esb  - pointer to extrapolated signal buffer.  It contains enough
*                  signal to search +-delta samples.
*          lsw   - lag search window length
*          delta - search for the maximum lag within a range of +-delta samps
*
* Outputs: (Word16) - the lag.
*          *pemax - value contains the energy of esb at the returned lag.
*          *pc2max- value contains the correlation^2 at the returned lag.
*---------------------------------------------------------------------------*/
Word16 getlag(Word16 *x, Word16 *esb, Word16 lsw, Word16 delta, Word16 *pemax, Word16 *pc2max)
{
	Word16 *p_y, *p_y_lsw, *p_y1, *p_x;
	Word16 c2max_man, c2max_exp;
	Word16 emax_man, emax_exp;
	Word16 e_man, e_exp;
	Word16 c2_man, c2_exp;
	Word32 e, c, a0, a1;
	Word16 i, lag, lagmax, s, t, tt;

	/* EXTRACT FIRST GOOD FRAME(S)  */
	/* AND FIND ITS TIME LAG RELATIVE TO EXTRAPOLATED WAVEforM */

	p_y = esb;
	p_x = x;

	e = L_mult0(*p_y, *p_y);
	c = L_mult0(*p_x++, *p_y++);
	for (i = 0; i<lsw - 1; i++){
		e = L_mac0(e, *p_y, *p_y);       /*  energy of y()*/
		c = L_mac0(c, *p_x++, *p_y++);   /*  correlation at first lag of MAXOS */
	}

	c2max_exp = norm_l(c);
	s = extract_h(L_shl(c, c2max_exp));
#ifdef PLATFORM
	c2max_exp = SSAT16_SHIFTL_1(c2max_exp);
#else
	c2max_exp = shl(c2max_exp, 1);
#endif
	c2max_man = extract_h(L_mult(s, s));
	if (c < 0)                           /* if correlation is negative, */
		c2max_man = sub(0, c2max_man);   /* get correlation square with negative sign */

	emax_exp = norm_l(e);
	emax_man = extract_h(L_shl(e, emax_exp));

	lagmax = delta;
	p_y = esb;
	p_y_lsw = p_y + lsw;
#if WMOPS
	move16();
#endif
	for (lag = delta - 1; lag >= -delta; lag--){
		e = L_msu0(e, *p_y, *p_y);
		e = L_mac0(e, *p_y_lsw, *p_y_lsw);
		p_y++; p_y_lsw++;

		p_y1 = p_y;
		p_x = x;

		c = L_mult0(*p_x++, *p_y1++);
		for (i = 0; i<lsw - 1; i++){
			c = L_mac0(c, *p_x++, *p_y1++);	/*  correlation at lag */
		}

		c2_exp = norm_l(c);
		s = extract_h(L_shl(c, c2_exp));
#ifdef PLATFORM
		c2_exp = SSAT16_SHIFTL_1(c2_exp);
#else
		c2_exp = shl(c2_exp, 1);
#endif
		c2_man = extract_h(L_mult(s, s));
		if (c < 0)							/* if correlation is negative, */
			c2_man = sub(0, c2_man);		/* get correlation square with negative sign */
		e_exp = norm_l(e);
		e_man = extract_h(L_shl(e, e_exp));

		a0 = L_mult(c2_man, emax_man);
		a1 = L_mult(c2max_man, e_man);
		s = add(c2_exp, emax_exp);
		t = add(c2max_exp, e_exp);

		tt = sub(s, t);
		if (tt >= 0)
			a0 = L_shr(a0, tt);
		if (tt <0)
			a1 = L_shl(a1, tt);

		if (L_sub(a0, a1) > 0) {
			c2max_man = c2_man; c2max_exp = c2_exp;
			emax_man = e_man; emax_exp = e_exp;
			lagmax = lag;
#if WMOPS
			move16(); move16();; move16(); move16();; move16();
#endif
		}

	}

	pemax[0] = emax_man; pemax[1] = emax_exp;
	pc2max[0] = c2max_man; pc2max[1] = c2max_exp;
#if WMOPS
	move16();; move16(); move16();; move16();
#endif

	return(lagmax);
}

/*-----------------------------------------------------------------------------
 * Function: testrpc()
 *
 * Description: Test to determine if the last good frame before erasure or the
 *              first good frame after erasure is completely unvoiced or
 *              noise.  If either one is, return a flag=0 indicating that
 *              rephasing and time warping should not be done.  Otherwise
 *              return a flag=1 indicating that rephasing and time warping
 *              can be done.
 *
 * Inputs:  merit - figure of merit for the last good frame
 *          *inbuf- pointer to buffer containing the first good frame speech
 *                  of length FRSZ/2 (8kHz sampling).
 *
 * Outputs: return() - flag = 0 : last good frame or first good frame is
 *                                unvoiced.
 *                   - flag = 1 : last good frame and first good frame are
 *                                not unvoiced.
 *---------------------------------------------------------------------------*/
int testrpc(short merit, short *inbuf)
{
   Word32   r0, r1;
   int      i;
   int      rpcflag;
   Word32   energy;
   Word16   shift, t;

	/* CHECK IF THE SIGNAL SEGMENT CAUSES OVERFLOW */
   t = shr(inbuf[0], 3);
   energy = L_mult0(t,t);
	for (i=1;i<FRSZ;i++)
   {
	    t = shr(inbuf[i], 3);
	    energy = L_mac0(energy,t,t);
	}
   shift = norm_l(energy);
   shift = sub(6, shift);
   if (shift > 0)
   {
      shift = shr(add(shift, 1), 1);
      for (i=0;i<FRSZ;i++)
      {
         inbuf[i] = shr(inbuf[i], shift);
#ifdef WMOPS
         move16();
#endif
      }
   }
   shift = s_max(shift, 0);

   /* IF THE LAST GOOD FRAME IS UNVOICED, DO NOT DO WAVEFORM MATCHING */
   if (sub(merit, 256*MLO)<=0)
   {
      rpcflag=0;
#if WMOPS
      move16();
#endif
   }
   else
   {
      /* % IF FIRST FUTURE GOOD FRAME LOOKS LIKE UNVOICED, SKIP WAVEFORM MATCHING */
      r0 = L_mult0(inbuf[FRSZ-1], inbuf[FRSZ-1]);
      r1 = L_mult(0,0);
      for (i=0; i<FRSZ-1; i++)
      {
         r0 = L_mac0(r0, inbuf[i], inbuf[i]);     /* % r0 = energy of 1st future good frame  */
         r1 = L_mac0(r1, inbuf[i], inbuf[i+1]);   /* % r1 = 1st unnormalized autocorrelation */
      }
      r0 = L_shr(r0, 3);
      r0 = L_sub(r1,r0);
      if (r0<0)               /* % if 1st normalized autocorrelation coefficient < 0.1,  */
         rpcflag=0;           /* % don't do future waveform matching; signal it by wmflag=0 */
      if (r0>=0)              /* % otherwise, */
         rpcflag=1;           /* % do future waveform matching; signal it by setting wmflag=0 */

#if WMOPS
      move16();
#endif
   }
   return(rpcflag);
}

/*-----------------------------------------------------------------------------
 * Function: resample()
 *
 * Description: Low complexity resampler.  The input buffer is stretched or
 *              shrunk by "delta" samples.  The resampling is done by a
 *              sample shift overlap-add process.  The resulting signal is
 *              placed in the output buffer.  In the case of stretching
 *              (delta > 0), any extra samples beyond FRSZ are not computed.
 *              The extra samples to the "left" are the ones thrown out.
 *
 * Inputs:  *in   - pointer to 16kHz input buffer
 *          *out  - pointer to buffer for output
 *          delta - number of samples to stretch (+) or shrink (-)
 *
 * Outputs: *out  - "resampled" signal.
 *---------------------------------------------------------------------------*/
void resample(Word16 *in, Word16 *out, Word16 delta)
{
   Word16   olalen;
   Word16   i;
   Word16   iola = 0;
   Word16   ad;      /* Add/Drop */
   Word16   inlen;
   Word16   skip;
   Word16   outi;
   Word16   oldi = 0;

   Word16 spad16;
   Word32 nspad32;
   Word16 Qspad;
   Word16 Qinlen;
   Word16 Qdelta;
   Word16 temp, temp2;
   Word16 tempQ;
   const Word16 *pup = NULL;
   const Word16 *pdwn = NULL;
   Word16 olandx;
   Word32 a0;
   const Word16 *ola3_8[6] = {ola3, ola4, ola5, ola6, ola7, ola8};

   /* Compute the number of samples in the input that will be used */
   /* Also, compute the samples per add/drop needed, and the ola length */
   if (delta !=0)
   {
      ad = 1;  /* add */
      inlen = (FRSZ - MIN_UNSTBL);
      skip = 0;
#if WMOPS
      move16();move16();move16();
#endif
      if (delta < 0) /* indicates a drop */
      {
         delta = sub(0, delta);
         ad = -1; /* drop */
#if WMOPS
      move16();
#endif
      }
      if (sub(ad,1)==0)
      {
         if (sub(delta,MIN_UNSTBL)>0)
         {
            skip  = sub(delta, MIN_UNSTBL);
            inlen = sub(inlen, skip);
         }
      }

      /* spad = ((Float)inlen)/delta; */
      Qdelta = norm_s(delta);
      Qinlen = norm_s(inlen);
      temp2  = shl(delta, Qdelta);
      temp   = shl(inlen, Qinlen);
      if (sub(temp,temp2)>=0)
      {
#ifdef PLATFORM
		 temp = SSAT16_SHIFTR_1(temp);
#else
         temp  = shr(temp, 1);
#endif
         Qinlen = sub(Qinlen, 1);
      }
      spad16 = div_s(temp, temp2);
      Qspad  = sub(Qinlen, Qdelta);
      Qspad  = add(Qspad, 15);

      if (add(ad,1)==0)    /* compute olalen for stretching */
      {
         /* olalen = (int)((inlen-delta)/delta);*/  /* floor */
         temp = sub(inlen, delta);
         tempQ= norm_s(temp);
         temp = shl(temp, tempQ);
         if (sub(temp,temp2)>=0)
         {
#ifdef PLATFORM
			temp = SSAT16_SHIFTR_1(temp);
#else
            temp = shr(temp, 1);
#endif
            tempQ = sub(tempQ, 1);
         }
         olalen = div_s(temp, temp2);
         temp   = sub(tempQ, Qdelta);
         temp   = add(temp, 15);
         olalen = shr(olalen, temp);
      }
      else                 /* compute olalen for shrinking */
      {
         temp = shr(spad16, Qspad);
         temp2 = sub(spad16, shl(temp, Qspad));
         if (temp2!=0)
            olalen = add(1, temp);
         if (temp2==0)
         {
            olalen = temp;
#if WMOPS
            move16();
#endif
         }
      }
      if (sub(olalen,8)>0) /* limit the olalen to 8 */
      {
         olalen=8;
#if WMOPS
         move16();
#endif
      }

      outi = 0;

      spad16  = add(spad16, 1);
      nspad32 = L_shl(skip, Qspad);
      olandx  = sub(olalen, 3);

#if WMOPS
      move16();
#endif
      /* Do the actual sample shift OLA resampling */
      for(i=skip; i<FRSZ-MIN_UNSTBL;i++) /* loop through the input */
      {
#if WMOPS
         test();    /* for the 2nd test in the if */
#endif
         /* time to add/delete a sample ? */
         if ((L_sub(i, L_shr(nspad32, Qspad))==0)&&(sub(i,FRSZ-MIN_UNSTBL-1))!=0)
         {
            nspad32 = L_add(nspad32, spad16); /* increment for next time */
            iola = 0;
            oldi = i;
            i = sub(i, ad);
            pup = ola3_8[olandx];
            pdwn = &pup[olalen-1];
#if WMOPS
            move16();move16();
#endif
         }
         if (sub(iola,olalen)<0)  /* we are in the middle of an ola */
         {
            iola = add(iola, 1);
 			   a0 = L_mult(in[i],*pup++);
			   a0 = L_mac(a0, in[oldi++], *pdwn--);
		  	   out[outi++] = roundInt(a0);
#ifdef WMOPS
            move16();
#endif
         }
         else     /* not in an ola, so just copy the sample */
         {
            out[outi++] = in[i];
#if WMOPS
            move16();
#endif
         }
      }
   }
   else     /* delta=0, so just copy the samples => no warping */
   {
      W16copy(out, in, FRSZ-MIN_UNSTBL);
   }
}

/*-----------------------------------------------------------------------------
 * Function: extractbuf()
 *
 * Description: Extract an extrapolated signal from the output history buffer.
 *
 * Inputs:  *xq   - pointer to the history buffer
 *          *esb  - pointer to buffer for output
 *          D     - Distance from end of last frame to start of esb()
 *          L     - number of samples to generate from xq
 *          pp    - pitch period to be used for PWE.
 *
 * Outputs: *esb  - extrapolated signal.
 *---------------------------------------------------------------------------*/
void extractbuf(Word16 *xq, Word16 *esb, Word16 D, Word16 L, Word16 pp)
{
   Word16 pos;
   int    n;
   Word16 ovs;

#if WMOPS
   move16();
#endif
   pos=0;      /* initialize position to first sample of current frame */
   if (D<0)    /* start with known samples back in xq */
   {
      W16copy(esb, xq+XQOFF+D, -D); /* copy the old samples into esb */
      if (sub(add(L,D),pp) <= 0)   /* if # of remaining samples in esb() <= pitch period */
      {
         W16copy(esb-D, xq+XQOFF-pp, L+D);      /* fill rest of esb() in 1 shot */
      }
      else
      {
         W16copy(esb-D, xq+XQOFF-pp, pp);     /* copy one pitch cycle first, */
         n= sub(pp, D);

         for (;n<L;n++)
         {
            esb[n] = esb[n-pp];           /*  then fill the rest of esb() buffer */
#if WMOPS
            move16();
#endif
         }
      }
   }
   else        /* starting point is some point in the future */
   {
      while (sub(pos,D) < 0)
      {
         pos=add(pos, pp);    /* increment pos by one pitch period at a time  */
      }                 /*  until position fly pass D */
      ovs=sub(pos, D);        /*  overshoot = last position - D */
      if (sub(ovs,L) >= 0)     /* if pos landed on esb(L) or beyond, */
      {
         W16copy(esb, xq+XQOFF-ovs, L); /* copy entire esb() in one shot*/
      }
      else              /* if pos landed inside the esb() buffer */
      {
         if (ovs > 0)   /* if pos landed beyong esb(1),*/
         {
            W16copy(esb, xq+XQOFF-ovs, ovs);    /* copy samples before landing position */
         }
         if (sub(L,ovs) <= pp)   /* if # of remaining samples in esb() <= pitch period */
         {
            W16copy(esb+ovs, xq+XQOFF-pp, L-ovs);      /* fill rest of esb() in 1 shot */
         }
         else
         {
            W16copy(esb+ovs, xq+XQOFF-pp, pp);     /* copy one pitch cycle first, */
            n=add(ovs, pp);

            for (;n<L;n++)
            {
               esb[n] = esb[n-pp];           /*  then fill the rest of esb() buffer */
#if WMOPS
               move16();
#endif
            }
         }
      }
   }
}

/*-----------------------------------------------------------------------------
 * Function: refinelag()
 *
 * Description: Refine the estimated lag using only the data within the OLA
 *              window.  Estimate the position of the OLA window by the current
 *              value of the lag.
 *
 * Inputs:  *xq   - pointer to the history buffer
 *          pp    - pitch period to be used for PWE.
 *          *inbuf- pointer to the first good frame 16kHz data
 *          estlag- current lag estimate
 *
 * Outputs: (int) - the refined lag.
 *---------------------------------------------------------------------------*/
#define RSR    4    /* refine search range 16kHz */
int refinelag( short *xq, short pp, short *inbuf, short estlag)
{
#if DMEM
   Word16 *esb;
   Word16 *y;
#else
   Word16 esb[OLALG+2*RSR];
   Word16 y[OLALG];			/* scaled inbuf[MIN_UNSTBL...MIN_UNSTBL+OLALG-1] */
#endif

   Word16 L, D, lagmax;
   Word16 e[2], c2[2];

   Word16 *p_y, *p_in, *p_esb;
   Word16 i, t, shift_y, shift_esb;
   Word32 e_esb, e_y;
   Word16 spola; /* start position of ola */

#if DMEM
   esb = allocWord16(0, OLALG+2*RSR-1);
   y = allocWord16(0, OLALG-1);
#endif
   /* EXTRAPOLATE WAVEforM TO FUTURE GOOD FRAME(S) +- del SAMPLES */
   L=add(OLALG,RSR<<1);      /* Length of extrapolated signal buffer for waveform matching */

   spola = add(estlag, (FRSZ-MIN_UNSTBL));
   spola = s_min(spola,FRSZ);
   spola = sub(FRSZ, spola);

   D=sub(sub(spola,estlag),RSR);      /* Distance from end of last frame to start of esb() */
   extractbuf(xq, esb, D, L, pp);

   /* scale input */
   p_in = inbuf+spola;
#ifdef PLATFORM
   t = SSAT16_SHIFTR_3(*p_in++);
#else
   t = shr(*p_in++, 3);
#endif
   e_y = L_mult0(t,t);
   for(i=1;i<OLALG;i++){
#ifdef PLATFORM
	   t = SSAT16_SHIFTR_3(*p_in++);
#else
	   t = shr(*p_in++, 3);
#endif
	   e_y = L_mac0(e_y, t, t);
   }
   shift_y = norm_l(e_y);
   shift_y = sub(6, shift_y);
   t = add(shift_y, 2);
   if(shift_y >= 0)
#ifdef PLATFORM
	   shift_y = SSAT16_SHIFTR_1(t);
#else
	   shift_y = shr(t, 1);
#endif
   if (shift_y<0)
   {
	   shift_y = 0;
#if WMOPS
	   move16();
#endif
   }
   p_in = inbuf+spola;
   p_y  = y;

   for(i=0;i<OLALG;i++)
   {
	   *p_y++ = shr(*p_in++, shift_y);
#ifdef WMOPS
      move16();
#endif
   }

   /* scale esb */
   p_esb = esb;
#ifdef PLATFORM
   t = SSAT16_SHIFTR_3(*p_esb++);
#else
   t = shr(*p_esb++, 3);
#endif
   e_esb = L_mult0(t,t);
   for(i=1;i<OLALG+2*RSR;i++){
#ifdef PLATFORM
	   t = SSAT16_SHIFTR_3(*p_esb++);
#else
	   t = shr(*p_esb++, 3);
#endif
	   e_esb = L_mac0(e_esb, t, t);
   }
   shift_esb = norm_l(e_esb);
   shift_esb = sub(6, shift_esb);
   t = add(shift_esb, 2);
   if(shift_esb >= 0)
#ifdef PLATFORM
	   shift_esb = SSAT16_SHIFTR_1(t);
#else
	   shift_esb = shr(t, 1);
#endif
   if (shift_esb<0)
   {
	   shift_esb = 0;
#if WMOPS
	   move16();
#endif
   }
   p_esb = esb;

   for(i=0;i<OLALG+2*RSR;i++){
	   *p_esb = shr(*p_esb, shift_esb);
#ifdef WMOPS
      move16();
#endif
	   p_esb++;
   }

   /* refine the lag */
   lagmax = getlag(y, esb, OLALG, RSR, e, c2);

#if DMEM
   deallocWord16(esb, 0, OLALG+2*RSR-1);
   deallocWord16(y, 0, OLALG-1);
#endif

   /* new lag is the old lag added to the new lag */
   return (add(lagmax, estlag));
}
/*-----------------------------------------------------------------------------
 * Function: ppchange()
 *
 * Description: Compute the lag offset between an extrapolated signal based on
 *              the output history buffer, and the first good frame.
 *
 * Inputs:  *xq   - pointer to the history buffer
 *          pp    - pitch period to be used for PWE.
 *          *inbuf- pointer to the first good frame 16kHz data
 *          estlag- current lag estimate
 *
 * Outputs: (int) - the refined lag.
 *---------------------------------------------------------------------------*/
#define REF 4 /* 16 kHz */
int ppchange( short *xq, short pp, short *inbuf)
{
#if DMEM
   Word16 *esb;
   Word16 *esb4k;
   Word16 *in4k;
#else
   Word16    esb[(FRSZ+2*MAXOS)];     /* extrapolated signal buffer */
   Word16    esb4k[(FRSZ+2*MAXOS)/4];
   Word16    in4k[FRSZ/4];
   Word16    in8k[FRSZ / 2];
#endif
   Word16    LSW, L, D;
   Word16      i, n;


   Word16      lagmax, refinement;
   Word16      del;
   Word16 emax_fx[2], cor2max_fx[2];
   Word32   ee;
   Word32 energy;
   Word16   t,tt;
   Word16   shift;
   Word16 ee_exp, ee_man;
   Word16 *ps;

#if DMEM
   esb = allocWord16(0, (FRSZ+2*MAXOS)-1);
   esb4k = allocWord16(0, ((FRSZ+2*MAXOS)/4)-1);
   in4k = allocWord16(0, (FRSZ/4)-1);
#endif
#ifdef PLATFORM
   del = SSAT16_SHIFTR_1(add(pp, 1));
#else
   del = shr(add(pp, 1), 1);
#endif
   del = add(del, 3);
#ifdef PLATFORM
   t = SSAT16_SHIFTL_2(SSAT16_SHIFTR_2(del));
#else
   t = shl(shr(del,2),2);
#endif
   if ( sub(t,del)!=0)
   {
      del=t;
#if WMOPS
      move16();
#endif
   }
   del = s_min(del, MAXOS);

   /* Set the Lag Search Window */
#ifdef PLATFORM
   LSW = add(pp, SSAT16_SHIFTL_1(pp));
   LSW = SSAT16_SHIFTR_1(add(LSW, 1));
#else
   LSW = add(pp, shl(pp, 1));
   LSW = shr(add(LSW, 1), 1);
#endif
   LSW = s_max(LSW, 80);
   LSW = s_min(LSW, FRSZ);
#ifdef PLATFORM
   LSW = SSAT16_SHIFTR_1(LSW);  /* now at 8k */
#else
   LSW = shr(LSW, 1);  /* now at 8k */
#endif

   /* EXTRAPOLATE WAVEforM TO FUTURE GOOD FRAME(S) +- del SAMPLES */
   /* Length of extrapolated signal buffer for waveform matching */
#ifdef PLATFORM
   L = SSAT16_SHIFTL_1(add(LSW, del));
#else
   L = shl(add(LSW, del), 1);
#endif

   /* Distance from end of last frame to start of esb() */
   D = sub(LBO, del);

   extractbuf(xq, esb, D, L, pp);

	/* CHECK if THE SIGNAL SEGMENT CAUSES OVERFLOW */
#ifdef PLATFORM
   t = SSAT16_SHIFTR_3(esb[0]);
#else
   t = shr(esb[0], 3);
#endif
   energy = L_mult0(t,t);
   for (i=2;i<L>>1;i+=2)
   {
#ifdef PLATFORM
	    t = SSAT16_SHIFTR_3(esb[i]);
#else
	    t = shr(esb[i], 3);
#endif
	    energy = L_mac0(energy,t,t);
	}
   shift = norm_l(energy);
   shift = sub(6, shift);
   t = add(shift, 2);
   if (shift >= 0)
   {
#ifdef PLATFORM
	  shift = SSAT16_SHIFTR_1(t);
#else
      shift = shr(t, 1);
#endif
   }
   if (shift<0)
   {
      shift = 0;
#if WMOPS
      move16();
#endif
   }

   /* subsample esb to 4k */
   t = sub(shift, 1);
   if (t<0)
   {
      t=0;
#if WMOPS
      move16();
#endif
   }
   ps = esb;
   for (i=0;i<(L>>2);i++)
   {
      esb4k[i] = shr(*ps, t);
#ifdef WMOPS
      move16();
#endif
      ps+=4;
   }

   /* subsample input to 4k */
   ps = inbuf;
   for (i=0;i<(FRSZ>>2);i++)
   {
      in4k[i] = *ps;
      ps+=4;
#if WMOPS
      move16();
#endif
   }

#ifdef PLATFORM
   lagmax = getlag(in4k, esb4k, SSAT16_SHIFTR_1(LSW), SSAT16_SHIFTR_2(del), emax_fx, cor2max_fx);
   lagmax = SSAT16_SHIFTL_2(lagmax);  /* 16kHz domain */
#else
   lagmax = getlag(in4k, esb4k, shr(LSW, 1), shr(del, 2), emax_fx, cor2max_fx);
   lagmax = shl(lagmax, 2);  /* 16kHz domain */
#endif
   /* refine the lag to 8kHz */
   lagmax = s_min(lagmax, add(del, -REF));
   lagmax = s_max(lagmax, sub(REF, del));

   /* first, subsample esb to 8kHz but offset by our lag */
#ifdef PLATFORM
   L = SSAT16_SHIFTL_1(add(LSW, REF));
#else
   L = shl(add(LSW, REF), 1);
#endif
   n = sub(sub(del, lagmax),REF);
   ps = &esb[n];
#ifdef PLATFORM
   for (i = 0; i<SSAT16_SHIFTR_1(L); i++)
#else
   for (i=0;i<shr(L, 1);i++)
#endif
   {
      esb[i] = shr(*ps, shift);
#ifdef WMOPS
      move16();
#endif
      ps+=2;
   }

   /* subsample input to 8k */
   ps = inbuf;
   for (i = 0; i < (FRSZ >> 1); i++)
   {
       in8k[i] = *ps;
       ps += 2;
#if WMOPS
       move16();
#endif
   }

   refinement = getlag(in8k, esb, LSW, REF>>1, emax_fx, cor2max_fx);
#ifdef PLATFORM
   lagmax = add(lagmax, SSAT16_SHIFTL_1(refinement));
#else
   lagmax = add(lagmax, shl(refinement, 1));
#endif
   ee = L_mult0(inbuf[0], inbuf[0]);
   for (i=1;i<LSW;i++)
      ee = L_mac0(ee, inbuf[i], inbuf[i]);

	ee_exp = norm_l(ee);
	t = extract_h(L_shl(ee, ee_exp));

	ee_exp = add(ee_exp, emax_fx[1]);
	ee_man = extract_h(L_mult(t, emax_fx[0]));

   tt = sub(ee_exp, cor2max_fx[1]);
   if (tt>0)
      ee_man = shr(ee_man, tt);
   if (tt<0)
   {
      cor2max_fx[0] = shl(cor2max_fx[0], tt);
#ifdef WMOPS
      move16();
#endif
   }

   if (ee_man == 0)              /* % in degenerate case of zero vectors */
   {
#if WMOPS
      move16();
#endif
      lagmax=-100;               /* %   reset best lag to 0 */
   }
   else if (( sub(lagmax,(MAXOS-2))>0 )||( sub(lagmax,(-MAXOS+2))<0 )) /* don't trust near the boundary */
   {
#if WMOPS
      move16();test();
#endif
      lagmax=-100;
   }
   else
   {
#ifdef PLATFORM
	  if (sub(cor2max_fx[0], SSAT16_SHIFTR_2(ee_man)) <= 0) /* % 0.09 else if cos(theta) of two vectors < 0.3, */
#else
      if (sub(cor2max_fx[0], shr(ee_man, 2))<=0) /* % 0.09 else if cos(theta) of two vectors < 0.3, */
#endif
      {
#if WMOPS
         move16();
#endif
         lagmax=-100;               /* %   reset best lag to 0 */
      }
   }
#if DMEM
   deallocWord16(esb, 0, (FRSZ+2*MAXOS)-1);
   deallocWord16(esb4k, 0, ((FRSZ+2*MAXOS)/4)-1);
   deallocWord16(in4k, 0, (FRSZ/4)-1);
#endif
   return(lagmax);
}



/*-----------------------------------------------------------------------------
 * Function: WB_PLC_common()
 *
 * Description: PLC function called in both good and bad frames
 *
 * Inputs:  *plc  - pointer to plc state memory
 *          *out  - pointer to output buffer
 *          *xq   - pointer to output history buffer
 *          good_frame - 0=bad, 1=good
 *
 * Outputs: *out  - potentially modified output buffer
 *---------------------------------------------------------------------------*/
void 	WB_PLC_common(
struct 	WB_PLC_State *plc,
Word16	*out,
Word16	*xq,
Word16	good_frame)
{
#if DMEM
   Word16  *xw;
   Word32  *rl;
   Word16  *xwd;
   Word16  *awl;
#else
	Word16	xw[DFO+FRSZ];
	Word32	rl[1+LPCO];
	Word16	xwd[LXD];
	Word16	awl[1+LPCO];
#endif
   Word32   a0;
	Word16	cpp;
	int		i;
   Word16   tmp16;

	/* extract from the buffer to OUTPUT */
	W16copy(out, xq+XQOFF, FRSZ);

	/* PERforM LPC ANALYSIS WITH ASYMMETRICAL WINdoW */
	if (good_frame)
   {
#if DMEM
      rl = allocWord32(0, LPCO);
#endif
      Autocorr((int *)rl,xq+LXQ-WINSZ,win_plc,WINSZ,LPCO); /* get autocorrelation coeff. */
      Spectral_Smoothing(LPCO,(int *)rl,sstwin_h,sstwin_l);   /* spectral smoothing */
      Levinson(rl, plc->al, plc->alast, LPCO);    /* Levinson-Durbin recursion */
#if DMEM
      deallocWord32(rl, 0, LPCO);
#endif
      for (i=1;i<=LPCO;i++)
      {/* bandwidth expansion */
         plc->al[i] = mult_r(bwel[i],plc->al[i]);
#ifdef WMOPS
         move16();
#endif
      }
	}
#if DMEM
   xw = allocWord16(0, DFO+FRSZ-1);
#endif
	/* CALCULATE LPC PREDICTION RESIDUAL (temporarily put it in xw[] array) */
	azfilterQ0_Q1(plc->al,LPCO,xq+XQOFF,xw+DFO,FRSZ);

	/* CALCULATE AVERAGE MAGNITUDE OF LPC PREDICTION RESIDUAL */
   if (good_frame)
   {
	   a0 = abs_s(xw[DFO]);
	   for (i=1;i<FRSZ;i++)
		   a0 = L_add(a0,(Word32)abs_s(xw[DFO+i]));
#ifdef PLATFORM
	  tmp16 = (Word16) SSAT_SHIFTR_7(a0);/* divide by 128, Q1->Q2 */
#else
      tmp16 = (Word16) L_shr(a0, 7);/* divide by 128, Q1->Q2 */
#endif
	   plc->avm = add(tmp16, mult(tmp16, 19661));   /* avm *= 1.6 */
#ifdef WMOPS
      move16();
#endif
   }
	/* GET PERCEPTUALLY WEIGHTED VERSION OF SPEECH SIGNAL */
#if WMOPS
   move16();
#endif

#if DMEM
   awl = allocWord16(0, LPCO);
#endif
	awl[0] = plc->al[0];
	for (i=1;i<=LPCO; i++)
   {
      awl[i] = mult_r(STWAL[i],plc->al[i]);
#ifdef WMOPS
      move16();
#endif
   }
	apfilterQ1_Q0(awl, LPCO, xw+DFO, xw+DFO, FRSZ, plc->stwpml);
   W16copy(plc->stwpml, xw+DFO+FRSZ-LPCO, LPCO);

#if DMEM
   deallocWord16(awl, 0, LPCO);
#endif
#if DMEM
   xwd = allocWord16(0, LXD-1);
#endif
	/* PERforM 8:1 DECIMATION, UPDATE DECIMATED WEIGHTED SPEECH BUFFER */
   decim(xw, xwd, plc);
#if DMEM
   deallocWord16(xw, 0, DFO+FRSZ-1);
#endif
	/* do COARSE PITCH EXTRACTION & PITCH REFINEMENT ONLY IN GOOD FRAME */
	if (good_frame)
   {
		/* GET THE COARSE VERSION OF PITCH PERIOD USING 8:1 DECIMATION */
		cpp = coarsepitch(xwd, plc->cpplast);
		plc->cpplast=cpp;
#if WMOPS
      move16();
#endif
   }
#if DMEM
   deallocWord16(xwd, 0, LXD-1);
#endif
   if (good_frame)
   {
		/* REFINE PITCH PERIOD, FIND PITCH TAP & ANALYSIS WINdoW SIZE, ETC. */
		plc->pp=prfn(&plc->ptfe,&plc->cormax,&plc->energymax32,&plc->ppt,
			                &plc->wsz,&plc->scaled_flag,xq,cpp);
#ifdef WMOPS
      move16();
#endif
	}
    /* UPDATE PITCH PERIOD HISTORY BUFFER */
	for (i=PPHL-1;i>0;i--)
   {
      plc->pph[i] = plc->pph[i-1];
#if WMOPS
      move16();
#endif
   }
	plc->pph[0] = plc->pp;
#if WMOPS
   move16();
#endif
    /* UPDATE SHORT-TERM SYNTHESIS FILTER MEMORY */
   W16copy(plc->stsyml, &xq[LXQ-LPCO], LPCO);

	/* SHifT DECODED SPEECH BUFFER */
   W16copy(plc->xq,xq+FRSZ,XQOFF);
}

/*-----------------------------------------------------------------------------
 * Function: WB_PLC_erasure()
 *
 * Description: PLC function called in bad frames
 *
 * Inputs:  *plc  - pointer to plc state memory
 *          *out  - pointer to output buffer
 * Outputs: *qdb  - extrapolated samples beyond the output buffer required
 *                  for qmf memory, ringing, and rephasing
 *---------------------------------------------------------------------------*/
void    WB_PLC_erasure(
struct 	WB_PLC_State *plc,
Word16 	*out,
Word16  *encbuf,
Word16   *qdb)
{
#if DMEM
   Word16   *ltring;
   Word16   *tmp;
   Word16   *ring;
#else
	Word16	ltring[OLALG];			/* Q1 */
	Word16	tmp[FRSZ+CODELAY+MAXOS];	/* Q0 */
   Word16   ring[OLAL+LPCO];
#endif
	Word32	a0;
   Word16   *xq;
	Word16	scalep, delta, gaw, upw;
	Word16	pp;
	Word16	n, i, n1, tmp16;
   Word16   *pring, *p_tmp;

	xq = plc->xq;
   pring = &xq[LXQ];

	plc->cfecount++;		/* update consecutive frame erasure counter */
	plc->ngfae=0;
	pp = plc->pp;
#if WMOPS
   move16();move16();move16();
#endif

	/* FIND PITCH PERIOD & SCALING FACTOR for WAVEforM EXTRAPOLATION */
	if (sub(plc->cfecount,1)==0)
   { /* if it is the first erased frame */

      /* calcualte figure of merit to determine mixing ratio */
      plc->merit=merit(xq,plc->wsz,plc->cormax,
			        plc->energymax32,plc->scaled_flag);
      plc->ppinc=0;    /* Q7 pitch period increase initialized to 0 */
#if WMOPS
      move16();move16();
#endif

      /* CALCULATE AVERAGE PITCH PERIOD INCREASE AT THE FIRST ERASED FRAME */
		for (n=1;n<5;n++)
      {
         n1 = sub(n,1);
			delta = sub(plc->pph[n-1],plc->pph[n]);	/* Q0 pitch period change */
#ifdef WMOPS
         test();
#endif
			if ((delta > 0) && ( sub(i_mult(20,delta), plc->pph[n1])<0) )
         {
				plc->ppinc=roundInt(L_shl(L_mult(delta, div_n[n1]), 6)); /* Q6 */
#if WMOPS
            move16();
#endif
				plc->ppinc = s_min(plc->ppinc,128);
				break;
			}
         else if((delta < 0) && ( sub(i_mult(-20,delta),plc->pph[n1])<0))
         {
#ifdef WMOPS
            test();
#endif
            plc->ppinc=roundInt(L_shl(L_mult(delta, div_n[n1]), 6)); /* Q7 */
#if WMOPS
            move16();
#endif
				plc->ppinc = s_max(plc->ppinc, -64);
				break;
			}
		}
#ifdef PLATFORM
		plc->ppf = SSAT16_SHIFTL_6(pp); /* set Q6 version for later use */
#else
		plc->ppf = shl(pp,6); /* set Q6 version for later use */
#endif
#if WMOPS
      move16();
#endif
#if DMEM
      ltring = allocWord16(0, OLALG-1);
#endif

		/* CALCULATE AN APPROXIMATION OF LT SYNTHESIS FILTER RINGING. */
      /* FIRST, CALCULATE LPC PRESIDCTION RESIDUAL ON-THE-FLY */
		azfilterQ0_Q1(plc->al,LPCO,xq+XQOFF-pp,ltring,OLAL);

      /* scale LPC residual by pitch predictor tap */
      for (n=0;n<OLAL;n++)
      {
         a0 = L_mult(ltring[n], plc->ppt);
         a0 = L_shl(a0, 6);
         ltring[n] = roundInt(a0);       /* Q1 */
#if WMOPS
         move16();
#endif
		}
		/* FILTER LONG-TERM SYNTHSIS FILTER RINGING WITH LPC SYNTHESIS FILTER */
#if DMEM
      ring = allocWord16(0, OLAL+LPCO-1);
#endif
		apfilterQ1_Q0(plc->al,LPCO,ltring,ring+LPCO,OLAL,plc->stsyml);
      W16copy(pring, ring+LPCO, OLAL);

#if DMEM
      deallocWord16(ltring, 0, OLALG-1);
      deallocWord16(ring,   0, OLAL+LPCO-1);
#endif

      /* reset long ovlp */
      plc->ovlp = 0;
      plc->ovlpidx = 0;
      plc->pup = NULL;
      plc->pdown = NULL;
	}

   if (sub(plc->cfecount,2)==0)
   {  /* if it is the 2nd consecutively erased frame, */
      /* change pitch period by adding ppinc */
		plc->ppf = add(plc->ppf, plc->ppinc);
#if WMOPS
      move16();
#endif
#ifdef PLATFORM
		pp=SSAT16_SHIFTR_6(add(plc->ppf,32)); /* round off to nearest integer */
#else
		pp=shr(add(plc->ppf,32),6); /* round off to nearest integer */
#endif
		pp = s_min(pp,MAXPP);
		pp = s_max(pp,MINPP);
		plc->ppf = pp;
#if WMOPS
      move16();
#endif
	}
#ifdef WMOPS
   test();
#endif
   if ((sub(plc->cfecount,1)!=0)&&(sub(plc->cfecount,2)!=0)) {
		pp = plc->ppf;
#if WMOPS
      move16();
#endif
	}
	plc->pp = pp;
#if WMOPS
   move16();
#endif
    /* if FIGURE OF MERIT > LOW LIMIT, do PERIODIC WAVEforM EXTRAPOLATION */
	if (sub(plc->merit,256*MLO)>0)
   {
		/* EXTRAPOLATE BY OLAL SAMPLES, OVERLAP-ADD with RINGING */
		delta = 1638;
		upw = 0;
#if WMOPS
      move16();move16();
#endif
      /* first-phase extrapolation */
		for (i=0;i<OLAL;i++)
      {
			tmp16 = roundInt(L_shl(L_mult(plc->ptfe,xq[XQOFF+i-pp]),1));
			upw = add(upw, delta);
			a0 = L_mult(tmp16,upw);
			a0 = L_mac(a0,pring[i],sub(MAX_16,upw));
			xq[XQOFF+i] = roundInt(a0);
#ifdef WMOPS
         move16();
#endif
		}

		/* SECOND-PHASE EXTRAPOLATION OF DECODER OUTPUT SPEECH SIGNAL */
		for (i=OLAL;i<FRSZ+CODELAY+MAXOS;i++)
      {
		    xq[XQOFF+i] = roundInt(L_shl(L_mult(plc->ptfe,xq[XQOFF+i-pp]),1));
#ifdef WMOPS
          move16();
#endif
      }

	}

	/* if FIGURE OF MERIT < HIGH LIMIT, FEED WHITE NOISE THROUGH LPC FILTER,
       AND MIX THE RESULT WITH PERIODICALLY EXTRAPOLATED WAVEforM if NEEDED */
   if (sub(plc->merit, 256*MHI)<=0)
   {
		/* GENERATE ONE FRAME OF WHITE GAUSSIAN RANdoM NOISE WITH std = avm */
      n=plc->cfecount;
#if WMOPS
      move16();
#endif
#if DMEM
      tmp = allocWord16(0, FRSZ+CODELAY+MAXOS-1);
#endif
      for (i=0;i<FRSZ+CODELAY+MAXOS;i++)
      {
         if (sub(n,126)>0) n=sub(n,127);  /* modulo indexing */
         tmp[i] = mult_r(plc->avm,wn[s_and(n,127)]);	/* Q2*Q13 -> Q0 */
         n=add(n,plc->cfecount);
#if WMOPS
         move16();
#endif
      }

		/* FILTER THE WHITE NOISE WITH LPC SYNTHESIS FILTER */
		apfilterQ0_Q0(plc->al,LPCO,tmp,tmp,FRSZ+CODELAY+MAXOS,plc->stsyml);

		/* CALCULATE THE SCALING FACTORS for THE MIXTURE */
      if (sub(plc->merit, i_mult(256,MLO))>0 )
      {
         /* scaling factor for random noise component */
#ifdef PLATFORM
		 plc->scaler=SSAT16_SHIFTL_4(sub(MHI*256,plc->merit));	/* Q15 */
#else
         plc->scaler=shl(sub(MHI*256,plc->merit),4);	/* Q15 */
#endif
#ifdef WMOPS
         move16();
#endif
		   scalep=sub(MAX_16,plc->scaler); /* scaling for periodic component */
		    /* MIX THE TWO COMPONENTS */
         p_tmp = tmp;
         for (i=XQOFF;i<LXQ+CODELAY+MAXOS;i++)
         {
            a0 = L_mult(scalep, xq[i]);
		      a0 = L_mac(a0, plc->scaler, *p_tmp++);
		      xq[i] = roundInt(a0);
#if WMOPS
            move16();
#endif
          }
		}
      else
      {
          plc->scaler=MAX_16;	/* scaling factor for noise component */
#if WMOPS
          move16();
#endif
          /*
           * original code cause compile error, just remove the extra W16
           * error: '__builtin_memcpy' forming offset [594, 595] is out of the bounds [0, 594]
           * of object 'tmp' with type 'Word16[297]' {aka 'short int[297]'} [-Werror=array-bounds]
           * LXQ+CODELAY+MAXOS-XQOFF+1 = XQOFF+FRSZ+CODELAY+MAXOS-XQOFF+1
           *                           = FRSZ+CODELAY+MAXOS+1
           *                           = ARRAY_SIZE(tmp)+1
           */
          //W16copy(&xq[XQOFF], tmp, LXQ+CODELAY+MAXOS-XQOFF+1);
          W16copy(&xq[XQOFF], tmp, LXQ+CODELAY+MAXOS-XQOFF);
		}
#if DMEM
      deallocWord16(tmp, 0, FRSZ+CODELAY+MAXOS-1);
#endif
	}
   else
   {
        plc->scaler=0;
#if WMOPS
        move16();
#endif
	}
	/* if MORE THAN GATTST FRAMES INTO ERASURE, APPLY GAIN ATTENUATION WINdoW */

   if (sub(plc->cfecount,GATTST)>0)
   {
      if (sub(plc->cfecount,GATTEND)<=0)
      {
			delta = gawd[sub(plc->cfecount, (GATTST+1))];
			gaw = MAX_16;
#if WMOPS
			move16();move16();
#endif
         for (i=XQOFF; i<XQOFF+FRSZ; i++)
         {
            xq[i] = mult(xq[i], gaw);
            gaw = add(gaw, delta);
#ifdef WMOPS
            move16();
#endif
			}
         if(sub(plc->cfecount, GATTEND) < 0)
         {
            for (i=XQOFF+FRSZ; i<XQOFF+FRSZ+CODELAY+MAXOS; i++)
            {
               xq[i] = mult(xq[i], gaw);
               gaw = add(gaw, delta);
#ifdef WMOPS
               move16();
#endif
            }
         }
         else
         {
            W16zero(&xq[XQOFF+FRSZ], CODELAY+MAXOS);
         }
      }
      else /* if MORE THAN GATTEND FRAMES INTO ERASURE, MUTE OUTPUT SIGNAL */
      {
         W16zero(&xq[XQOFF],FRSZ+CODELAY+MAXOS);
      }
   }
   if (encbuf) W16copy(encbuf, &xq[XQOFF], FRSZ + 73);
	WB_PLC_common(plc,out,xq,0);
   if (qdb) W16copy(qdb, &xq[XQOFF+FRSZ], 22+MAXOS);
}

/*-----------------------------------------------------------------------------
 * Function: WB_PLC()
 *
 * Description: PLC function called in good frames
 *
 * Inputs:  *plc  - pointer to plc state memory
 *          *out  - pointer to output buffer
 *          *inbuf- pointer to the good frame input
 *
 * Outputs: *out  - potentially modified output buffer
 *---------------------------------------------------------------------------*/
void	WB_PLC(
struct 	WB_PLC_State *plc,
Word16 	*out,
Word16 	*inbuf)
{
   Word16   *xq;
	Word32	a0;

#ifdef ENABLE_TIME_STRETCH
    const Word16	*pup, *pdown;
   Word16   outlen;
#endif
	Word16   i, j, length;
   Word16   *pring;

#if DMEM
   Word16   *out2;
#else
   short out2[FRSZ];
#endif


   xq = plc->xq;
   pring = &xq[LXQ];
   W16copy(xq+XQOFF,inbuf,FRSZ);

	if (plc->cfecount != 0)
   {
      plc->nfle = plc->cfecount; /* update Number of Frames in Last Erasure */
      plc->ngfae = 1; /* set Number of Good Frames After Erasure to 1 */
#if WMOPS
      move16();move16();
#endif
	}
   else
   {
		 plc->ngfae++;   /* update Number of Good Frames After Erasure */
#if WMOPS
       move16();
#endif
       plc->ngfae = s_min(plc->ngfae, 9);
	}
	plc->cfecount=0;
#if WMOPS
   move16();
#endif

	if (sub(plc->ngfae,1)==0)
   {  /* if THIS IS THE FIRST GOOD FRAME AFTER THE ERASURE */
/* Do not enable time stretch */
#ifdef ENABLE_TIME_STRETCH
      outlen = FRSZ;
#if WMOPS
      move16();
#endif
      if (plc->lag!=0) /* -100 */
      {
#if DMEM
         out2 = allocWord16(0, FRSZ-1);
#endif
         /* refine the lag around the OLA window */
         plc->lag = refinelag( xq, plc->pp, inbuf, plc->lag);
#if WMOPS
         move16();
#endif
         outlen = add(FRSZ-MIN_UNSTBL, plc->lag);
         outlen = s_min(outlen, FRSZ);

         resample(inbuf+MIN_UNSTBL, out2+FRSZ-outlen, plc->lag);
         for (i=0;i<FRSZ-outlen+OLALG;i++)
         {
            xq[XQOFF+i] = roundInt(L_shl(L_mult(plc->ptfe,xq[XQOFF+i-plc->pp]),1));
#ifdef WMOPS
            move16();
#endif
         }

         pup=olaug;
         pdown=oladg;

         for (i=0;i<OLALG;i++)
         {
			   a0 = L_mult(xq[XQOFF+i], pup[i]);
			   a0 = L_mac(a0, pring[i], pdown[i]);
		  	   xq[XQOFF+i] = roundInt(a0);
#ifdef WMOPS
            move16();
#endif
		   }

         for (i=FRSZ-outlen;i<FRSZ-outlen+OLALG;i++)
         {
			   a0 = L_mult(out2[i],*pup++);
			   a0 = L_mac(a0, xq[XQOFF+i], *pdown++);
		  	   xq[XQOFF+i] = roundInt(a0);
#ifdef WMOPS
            move16();
#endif
		   }
         W16copy(&xq[XQOFF+i], &out2[i], FRSZ-i);

#if DMEM
         deallocWord16(out2, 0, FRSZ-1);
#endif
      }
      else
#endif
      {


		   if (sub(plc->scaler,MAX_16)==0)
         {  /* if LAST FRAME IS BASICALLY UNVOICED (scaler=1), USE SHORT OVERLAP-ADD */
                plc->ovlp=SOLAL;
                plc->pup=olaup;
                plc->pdown=oladown;
         }
		   else
         {				/* OTHERWISE, USE LONGER OVERLAP-ADD */
                plc->ovlp=OLALG;
                plc->pup=olaug;
                plc->pdown=oladg;
		   }
   #if WMOPS
         move16();
   #endif

         /* PERforM OVERLAP-ADD WITH RINGING OF CASCADED LT & ST SYNTHESIS FILTER */
         length = s_min(plc->ovlp, FRSZ);
         for (i=0;i<length;i++)
         {
			   a0 = L_mult(xq[XQOFF+i],plc->pup[i]);
			   a0 = L_mac(a0, pring[i], plc->pdown[i]);
		  	   xq[XQOFF+i] = roundInt(a0);
#ifdef WMOPS
            move16();
#endif
		   }

         if (plc->ovlp > FRSZ)
         {
             plc->ovlp = sub(plc->ovlp, FRSZ);
             plc->ovlpidx = add(plc->ovlpidx, FRSZ);
         }
         else
         {
             plc->ovlp = 0;
             plc->ovlpidx = 0;
         }
      }
      W16copy(out, xq+XQOFF, FRSZ);
   }

    /* Process unfinished overlap */
    if (sub(plc->ngfae, 1) > 0 && plc->ovlp > 0) {
       /* PERforM OVERLAP-ADD WITH RINGING OF CASCADED LT & ST SYNTHESIS FILTER */
       length = s_min(plc->ovlp, FRSZ);
       //print_vec(stdout, pring, length);
       for (i = 0, j = plc->ovlpidx; i < length; i++, j++)
       {
           a0 = L_mult(xq[XQOFF + i], plc->pup[j]);
           a0 = L_mac(a0, pring[j], plc->pdown[j]);
           xq[XQOFF + i] = roundInt(a0);
#ifdef WMOPS
           move16();
#endif
       }
       if (plc->ovlp > FRSZ)
       {
           plc->ovlp = sub(plc->ovlp, FRSZ);
           plc->ovlpidx = add(plc->ovlpidx, FRSZ);
       }
       else
       {
           plc->ovlp = 0;
           plc->ovlpidx = 0;
       }
       W16copy(out, xq + XQOFF, FRSZ);
   }
	WB_PLC_common(plc,out,xq,1);
}

#if 0
#define BUFFER_SIZE (160)
static short PcmOutBuf[BUFFER_SIZE];
static short PcmOutBuf1[BUFFER_SIZE];

int CreatLossPackets(short *pDataIn, int iLossPacketLength, int iPacketLossRate, int iFameLength)
{
	int iPacketLossCount = 0;
	static int  siFrameAllCount = 0;//control the rate of packet loss;

	siFrameAllCount++;
	int num = rand() % 10;
	if ((siFrameAllCount % 7 == 0) || ((siFrameAllCount + 1) % 7 == 0) || ((siFrameAllCount + 2) % 7 == 0))//(num==0)// (pDataIn[0] < -30000)//((siFrameAllCount == 841) || (siFrameAllCount == 842) || (siFrameAllCount == 843) || (siFrameAllCount == 844) || (siFrameAllCount == 845) || (siFrameAllCount == 846))// //if ((siFrameAllCount == 40) || (siFrameAllCount == 41)|| (siFrameAllCount == 42))// || (siFrameAllCount % 9 == 0) || (siFrameAllCount % 10 == 0))//(siFrameAllCount % iPacketLossRate == 0)//
	{
		memset(pDataIn, 0x00, iLossPacketLength);
		iPacketLossCount = 1;
	}
	else
	{
		iPacketLossCount = 0;
	}
	return iPacketLossCount;
}
int main(int argc, char **argv)
{
	char *PcmInFileName, *PcmOutFileName, *PlcOutFileName;
	FILE *fPcmInFile, *fPcmOutFile, *fPlcOutFile;
	Word16 qdb[22 + MAXOS];
	int nFrames;
	int iPlcFlag;

	PcmInFileName = "D:/bestechnic/PROJECTS/LPC_PLC(1)/LPC_PLC/LPC_PLC/SilenceBackground.pcm";
	PcmOutFileName = "SilenceBackgroundOut.pcm";
	PlcOutFileName = "SilenceBackgroundPLCOut3cotinues.pcm";
	/* open PCM input file */
	fPcmInFile = fopen(PcmInFileName, "rb");
	if (!fPcmInFile) {
		printf(" *** Error opening input file %s ***\n", PcmInFileName);
		return -1;
	}


	/* open PCM output file */
	fPcmOutFile = fopen(PcmOutFileName, "wb");
	if (!fPcmOutFile) {
		printf(" *** Error opening pcmoutput file %s ***\n", PcmOutFileName);
		return -1;
	}

	/* open PLC output file */
	fPlcOutFile = fopen(PlcOutFileName, "wb");
	if (!fPlcOutFile) {
		printf(" *** Error opening plcoutput file %s ***\n", PlcOutFileName);
		return -1;
	}
	nFrames = 0;
	struct WB_PLC_State      plc_state;
	struct WB_PLC_State      *plc;
	Reset_WB_PLC(&plc_state);
	plc = &plc_state;
	do {

		if (fread(PcmOutBuf, 2, BUFFER_SIZE, fPcmInFile) != BUFFER_SIZE)
		{
			break;
		}
		iPlcFlag = CreatLossPackets(PcmOutBuf, BUFFER_SIZE * 2, 10, BUFFER_SIZE);
		for (int i = 0; i < BUFFER_SIZE; i++)
			PcmOutBuf1[i] = PcmOutBuf[i];
		if (!(iPlcFlag))
		{
			//speech_plc_16000_AddToHistory(&lc, PcmOutBuf, BUFFER_SIZE);
			//for (int kk = 1;kk<100;kk++)
			WB_PLC(plc, PcmOutBuf, PcmOutBuf);
		}
		else
		{
			//speech_plc_16000_Dofe(&lc, PcmOutBuf, BUFFER_SIZE);
			//     for (int kk = 1;kk<100;kk++)
			WB_PLC_erasure(plc, PcmOutBuf, qdb);
		}
		fwrite(PcmOutBuf1, 2, BUFFER_SIZE, fPcmOutFile);
		//fwrite(CvsdOutBuf, 2, BUFFER_SIZE/2, fCvsdOutFile);
		fflush(fPcmOutFile);
		fwrite(PcmOutBuf, 2, BUFFER_SIZE, fPlcOutFile);
		fflush(fPlcOutFile);

		printf("Frame:%d\n", nFrames);
		nFrames++;


	} while (1);


	printf("end!\n");



	/* close files */
	fclose(fPcmOutFile);
	fclose(fPlcOutFile);

	return 0;
}

/*
int main()
{
	FILE *src_fd1, *out_fd;
	short *in;
	short *out;
	short *tmp;
	short *xq;
	char *src_filename1 = "SilenceBackground.pcm";
	char *out_filename = "SilenceBackgroundout.pcm";// "white_noise_out.pcm";
	src_fd1 = fopen(src_filename1, "rb");
	out_fd = fopen(out_filename, "wb");
	struct WB_PLC_State      plc_state;
	struct WB_PLC_State      *plc;
	Reset_WB_PLC(&plc_state);
	plc = &plc_state;
	if (!src_fd1)
	{
		printf("***Error opening input file1 %s ****\n", src_fd1);
		return -1;
	}
	if (!out_fd)
	{
		printf("Error opening output file %s \n", out_filename);
		return -1;
	}
	in = (short*)malloc(sizeof(short) * 329000);
	out = (short*)malloc(sizeof(short) * 329000);
	fread(in, sizeof(short), 329000, src_fd1);
	//for (int cfecount = 1; cfecount < 127; cfecount++)
	// WB PLC on Full Band Signal
	//WB_PLC_erasure(plc, &output[i*FRSZ], qdb);
	//WB PLC on LB
	for (int i = 0; i < (329000 - FRSZ); i += FRSZ)
	WB_PLC(plc, in+i, in+i);
	fwrite(in, sizeof(short), 329000, out_fd);
	fclose(out_fd);

}

*/
#endif
