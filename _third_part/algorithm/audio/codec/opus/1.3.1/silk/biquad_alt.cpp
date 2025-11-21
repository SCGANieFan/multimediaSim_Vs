/***********************************************************************
Copyright (c) 2006-2011, Skype Limited. All rights reserved.
Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions
are met:
- Redistributions of source code must retain the above copyright notice,
this list of conditions and the following disclaimer.
- Redistributions in binary form must reproduce the above copyright
notice, this list of conditions and the following disclaimer in the
documentation and/or other materials provided with the distribution.
- Neither the name of Internet Society, IETF or IETF Trust, nor the
names of specific contributors, may be used to endorse or promote
products derived from this software without specific prior written
permission.
THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE
LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
POSSIBILITY OF SUCH DAMAGE.
***********************************************************************/

/*                                                                      *
 * silk_biquad_alt.c                                              *
 *                                                                      *
 * Second order ARMA filter                                             *
 * Can handle slowly varying filter coefficients                        *
 *                                                                      */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "SigProc_FIX.h"
#ifndef HIFI_OPT
/* Second order ARMA filter, alternative implementation */
void silk_biquad_alt_stride1(
    const opus_int16            *in,                /* I     input signal                                               */
    const opus_int32            *B_Q28,             /* I     MA coefficients [3]                                        */
    const opus_int32            *A_Q28,             /* I     AR coefficients [2]                                        */
    opus_int32                  *S,                 /* I/O   State vector [2]                                           */
    opus_int16                  *out,               /* O     output signal                                              */
    const opus_int32            len                 /* I     signal length (must be even)                               */
)
{
    /* DIRECT FORM II TRANSPOSED (uses 2 element state vector) */
    opus_int   k;
    opus_int32 inval, A0_U_Q28, A0_L_Q28, A1_U_Q28, A1_L_Q28, out32_Q14;

    /* Negate A_Q28 values and split in two parts */
    A0_L_Q28 = ( -A_Q28[ 0 ] ) & 0x00003FFF;        /* lower part */
    A0_U_Q28 = silk_RSHIFT( -A_Q28[ 0 ], 14 );      /* upper part */
    A1_L_Q28 = ( -A_Q28[ 1 ] ) & 0x00003FFF;        /* lower part */
    A1_U_Q28 = silk_RSHIFT( -A_Q28[ 1 ], 14 );      /* upper part */

    for( k = 0; k < len; k++ ) {
        /* S[ 0 ], S[ 1 ]: Q12 */
        inval = in[ k ];
        out32_Q14 = silk_LSHIFT( silk_SMLAWB( S[ 0 ], B_Q28[ 0 ], inval ), 2 );

        S[ 0 ] = S[1] + silk_RSHIFT_ROUND( silk_SMULWB( out32_Q14, A0_L_Q28 ), 14 );
        S[ 0 ] = silk_SMLAWB( S[ 0 ], out32_Q14, A0_U_Q28 );
        S[ 0 ] = silk_SMLAWB( S[ 0 ], B_Q28[ 1 ], inval);

        S[ 1 ] = silk_RSHIFT_ROUND( silk_SMULWB( out32_Q14, A1_L_Q28 ), 14 );
        S[ 1 ] = silk_SMLAWB( S[ 1 ], out32_Q14, A1_U_Q28 );
        S[ 1 ] = silk_SMLAWB( S[ 1 ], B_Q28[ 2 ], inval );

        /* Scale back to Q0 and saturate */
        out[ k ] = (opus_int16)silk_SAT16( silk_RSHIFT( out32_Q14 + (1<<14) - 1, 14 ) );
    }
}
#else
// ticks        ori          opt
//            1766000       517900
#define SHIFT 16383
void silk_biquad_alt_stride1(
    const opus_int16            *in,                /* I     input signal                                               */
    const opus_int32            *B_Q28,             /* I     MA coefficients [3]                                        */
    const opus_int32            *A_Q28,             /* I     AR coefficients [2]                                        */
    opus_int32                  *S,                 /* I/O   State vector [2]                                           */
    opus_int16                  *out,               /* O     output signal                                              */
    const opus_int32            len                 /* I     signal length (must be even)                               */
)
{
    /* DIRECT FORM II TRANSPOSED (uses 2 element state vector) */
    opus_int   k,temp32,B0;
    opus_int32 A0_U_Q28, A0_L_Q28, A1_U_Q28, A1_L_Q28, out32_Q14;
    opus_int16 A[4] __attribute__ ((aligned (8)));
    opus_int32 SS[4] __attribute__ ((aligned (8)));
    opus_int32 B12[2] __attribute__ ((aligned (8)));
    opus_int32 *pSS0,*pSS1,*pSS2;
    const opus_int16 *pIn;
    ae_int32x2 temp_ae32x2,out_ae32x2,B12_ae32x2,SS_ae32x2;
    ae_int16x4 A_ae16x4,inval_ae16x4;
    ae_int16x4 *p_ae16x4;
    ae_int32x2 *p_ae32x2,*pSS0_ae32x2,*pSS2_ae32x2;

    /* Negate A_Q28 values and split in two parts */
    A0_L_Q28 = ( -A_Q28[ 0 ] ) & 0x00003FFF;        /* lower part */
    A0_U_Q28 = silk_RSHIFT( -A_Q28[ 0 ], 14 );      /* upper part */
    A1_L_Q28 = ( -A_Q28[ 1 ] ) & 0x00003FFF;        /* lower part */
    A1_U_Q28 = silk_RSHIFT( -A_Q28[ 1 ], 14 );      /* upper part */
    A[0] = A0_L_Q28;A[1] = A1_L_Q28;
    A[2] = A0_U_Q28;A[3] = A1_U_Q28;
    p_ae16x4 = (ae_int16x4 *)&A[0];
    AE_L16X4_IP (A_ae16x4,p_ae16x4,0);

    SS[0] = S[0];SS[1] = S[1];
    SS[2] = S[1];SS[3] = 0;
    pSS0_ae32x2 = (ae_int32x2 *)&SS[0];
    pSS2_ae32x2 = (ae_int32x2 *)&SS[2];
    pSS0=&SS[0];
    pSS1=&SS[1];
    pSS2=&SS[2];

    B0 = B_Q28[0];
    B12[0] = B_Q28[1];
    B12[1] = B_Q28[2];
    p_ae32x2 = (ae_int32x2*)&B12[0];
    AE_L32X2_IP (B12_ae32x2,p_ae32x2,0);

    pIn = in;
    for( k = 0; k < len; k++ ) {
        inval_ae16x4 = *pIn;
        temp32 = (*pSS0)+(opus_int32)(((opus_int64)B0*(*pIn++))>>16);
        out32_Q14 = temp32<<2;

        out_ae32x2 = out32_Q14;
        AE_L32X2_IP(SS_ae32x2,pSS2_ae32x2,0);

        temp_ae32x2 = AE_MULFP32X16X2S_H (out_ae32x2, A_ae16x4);
        temp_ae32x2 = AE_SRAI32R(temp_ae32x2,15);
        SS_ae32x2 = AE_ADD32 (SS_ae32x2, temp_ae32x2);
        AE_S32X2_IP(SS_ae32x2,pSS0_ae32x2,0);

        temp_ae32x2 = AE_MULFP32X16X2S_L (out_ae32x2, A_ae16x4);
        temp_ae32x2 = AE_SRAI32(temp_ae32x2,1);
        SS_ae32x2 = AE_ADD32 (SS_ae32x2, temp_ae32x2);
        AE_S32X2_IP(SS_ae32x2,pSS0_ae32x2,0);

        temp_ae32x2 = AE_MULFP32X16X2S_L (B12_ae32x2, inval_ae16x4);
        temp_ae32x2 = AE_SRAI32(temp_ae32x2,1);
        SS_ae32x2 = AE_ADD32 (SS_ae32x2, temp_ae32x2);
        AE_S32X2_IP(SS_ae32x2,pSS0_ae32x2,0);

        (*pSS2)=(*pSS1);

        temp32 = (out32_Q14 + SHIFT)>>14;
        out[ k ] = (opus_int16)silk_SAT16(temp32);
    }
    S[0] = SS[0];S[1]=SS[1];

}
#endif
void silk_biquad_alt_stride2_c(
    const opus_int16            *in,                /* I     input signal                                               */
    const opus_int32            *B_Q28,             /* I     MA coefficients [3]                                        */
    const opus_int32            *A_Q28,             /* I     AR coefficients [2]                                        */
    opus_int32                  *S,                 /* I/O   State vector [4]                                           */
    opus_int16                  *out,               /* O     output signal                                              */
    const opus_int32            len                 /* I     signal length (must be even)                               */
)
{
    /* DIRECT FORM II TRANSPOSED (uses 2 element state vector) */
    opus_int   k;
    opus_int32 A0_U_Q28, A0_L_Q28, A1_U_Q28, A1_L_Q28, out32_Q14[ 2 ];

    /* Negate A_Q28 values and split in two parts */
    A0_L_Q28 = ( -A_Q28[ 0 ] ) & 0x00003FFF;        /* lower part */
    A0_U_Q28 = silk_RSHIFT( -A_Q28[ 0 ], 14 );      /* upper part */
    A1_L_Q28 = ( -A_Q28[ 1 ] ) & 0x00003FFF;        /* lower part */
    A1_U_Q28 = silk_RSHIFT( -A_Q28[ 1 ], 14 );      /* upper part */

    for( k = 0; k < len; k++ ) {
        /* S[ 0 ], S[ 1 ], S[ 2 ], S[ 3 ]: Q12 */
        out32_Q14[ 0 ] = silk_LSHIFT( silk_SMLAWB( S[ 0 ], B_Q28[ 0 ], in[ 2 * k + 0 ] ), 2 );
        out32_Q14[ 1 ] = silk_LSHIFT( silk_SMLAWB( S[ 2 ], B_Q28[ 0 ], in[ 2 * k + 1 ] ), 2 );

        S[ 0 ] = S[ 1 ] + silk_RSHIFT_ROUND( silk_SMULWB( out32_Q14[ 0 ], A0_L_Q28 ), 14 );
        S[ 2 ] = S[ 3 ] + silk_RSHIFT_ROUND( silk_SMULWB( out32_Q14[ 1 ], A0_L_Q28 ), 14 );
        S[ 0 ] = silk_SMLAWB( S[ 0 ], out32_Q14[ 0 ], A0_U_Q28 );
        S[ 2 ] = silk_SMLAWB( S[ 2 ], out32_Q14[ 1 ], A0_U_Q28 );
        S[ 0 ] = silk_SMLAWB( S[ 0 ], B_Q28[ 1 ], in[ 2 * k + 0 ] );
        S[ 2 ] = silk_SMLAWB( S[ 2 ], B_Q28[ 1 ], in[ 2 * k + 1 ] );

        S[ 1 ] = silk_RSHIFT_ROUND( silk_SMULWB( out32_Q14[ 0 ], A1_L_Q28 ), 14 );
        S[ 3 ] = silk_RSHIFT_ROUND( silk_SMULWB( out32_Q14[ 1 ], A1_L_Q28 ), 14 );
        S[ 1 ] = silk_SMLAWB( S[ 1 ], out32_Q14[ 0 ], A1_U_Q28 );
        S[ 3 ] = silk_SMLAWB( S[ 3 ], out32_Q14[ 1 ], A1_U_Q28 );
        S[ 1 ] = silk_SMLAWB( S[ 1 ], B_Q28[ 2 ], in[ 2 * k + 0 ] );
        S[ 3 ] = silk_SMLAWB( S[ 3 ], B_Q28[ 2 ], in[ 2 * k + 1 ] );

        /* Scale back to Q0 and saturate */
        out[ 2 * k + 0 ] = (opus_int16)silk_SAT16( silk_RSHIFT( out32_Q14[ 0 ] + (1<<14) - 1, 14 ) );
        out[ 2 * k + 1 ] = (opus_int16)silk_SAT16( silk_RSHIFT( out32_Q14[ 1 ] + (1<<14) - 1, 14 ) );
    }
}
