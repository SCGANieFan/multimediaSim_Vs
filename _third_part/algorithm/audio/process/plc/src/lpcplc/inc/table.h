/* ITU-T G.722 Appendix III                                                      */
/* Version:       1.0                                                            */
/* Revision Date: Nov.02, 2006                                                   */

/*
  ITU-T G.722 Appendix III ANSI-C Source Code

  This software has been developed by Broadcom Corporation.

  Copyright (c)  Broadcom Corporation 2006.  All rights reserved.

  COPYRIGHT : This file is the property of Broadcom Corporation.  It cannot
  be copied, used, distributed or modified without obtaining authorization
  from Broadcom Corporation.  If such authorization is provided, any modified
  version of the software must contain this header.

  WARRANTIES : This software is made available by  Broadcom Corporation in the
  hope that it will be useful, but without any warranty, including but not
  limited to any warranty of non-infringement of any third party intellectual
  property rights.  Broadcom Corporation is not liable for any direct or
  indirect consequence  or damages related to the use of the provided software,
  whether or not foreseeable .
*/

#include "typedef.h"

/* For functions: hsbupd() and lsbupd() */
extern const Word16 inv_frm_size[];

/* For function: quantl_toupdatescaling_logscl() */
extern const Word16 wlil4rilil[];
extern const Word16 q4[];

/* For functions: hsbdec(), plc_hsbdec() */
extern const Word16 NGFAEOFFSET_P1[];

/* LPC analysis windowing */
extern const Word16   win_plc[];

/* spectral smooth technique */
extern const Word16   sstwin_h[];
extern const Word16   sstwin_l[];

/* bandwidth expansion */
extern const Word16   bwel[];

/* spectral weighting */
extern const Word16   STWAL[];

extern const Word16 nbphtab[];
extern const Word16 nbpltab[];

extern const Word16 ola3[];
extern const Word16 ola4[];
extern const Word16 ola5[];
extern const Word16 ola6[];
extern const Word16 ola7[];
extern const Word16 ola8[];

/* coarse pitch search */
extern const  Word16  bdf[];
extern const  Word16  x[];
extern const  Word16  x2[];
extern const  Word16  invk[];
extern const  Word16  MPTH[];

extern const	Word16	pp9cb[];

extern const	Word16	olaup[];
extern const	Word16 	oladown[];
extern const	Word16  olaug[];
extern const	Word16  oladg[];

extern const	Word16	wn[];
extern const	Word16	gawd[];

extern const	Word16	div_n[];

extern const Word32 Pow2(Word16, Word16);

extern const Word16	tablog[];
extern const Word16	tabpow[];


/* The following are used to control the internal Q-value in apfilter_shift */
#define apQ         6
#define ap_shift    (16-apQ)

