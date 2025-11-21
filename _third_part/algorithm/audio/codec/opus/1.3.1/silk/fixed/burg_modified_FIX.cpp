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

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "SigProc_FIX.h"
#include "define.h"
#include "tuning_parameters.h"
#include "pitch.h"

#define MAX_FRAME_SIZE              384             /* subfr_length * nb_subfr = ( 0.005 * 16000 + 16 ) * 4 = 384 */

#define QA                          25
#define N_BITS_HEAD_ROOM            3
#define MIN_RSHIFTS                 -16
#define MAX_RSHIFTS                 (32 - QA)

/* Compute reflection coefficients from input signal */
#ifndef HIFI_OPT
void silk_burg_modified_c(
    opus_int32                  *res_nrg,           /* O    Residual energy                                             */
    opus_int                    *res_nrg_Q,         /* O    Residual energy Q value                                     */
    opus_int32                  A_Q16[],            /* O    Prediction coefficients (length order)                      */
    const opus_int16            x[],                /* I    Input signal, length: nb_subfr * ( D + subfr_length )       */
    const opus_int32            minInvGain_Q30,     /* I    Inverse of max prediction gain                              */
    const opus_int              subfr_length,       /* I    Input signal subframe length (incl. D preceding samples)    */
    const opus_int              nb_subfr,           /* I    Number of subframes stacked in x                            */
    const opus_int              D,                  /* I    Order                                                       */
    int                         arch                /* I    Run-time architecture                                       */
)
{
    opus_int         k, n, s, lz, rshifts, reached_max_gain;
    opus_int32       C0, num, nrg, rc_Q31, invGain_Q30, Atmp_QA, Atmp1, tmp1, tmp2, x1, x2;
    const opus_int16 *x_ptr;
    opus_int32       C_first_row[ SILK_MAX_ORDER_LPC ];
    opus_int32       C_last_row[  SILK_MAX_ORDER_LPC ];
    opus_int32       Af_QA[       SILK_MAX_ORDER_LPC ];
    opus_int32       CAf[ SILK_MAX_ORDER_LPC + 1 ];
    opus_int32       CAb[ SILK_MAX_ORDER_LPC + 1 ];
    opus_int32       xcorr[ SILK_MAX_ORDER_LPC ];
    opus_int64       C0_64;

    celt_assert( subfr_length * nb_subfr <= MAX_FRAME_SIZE );

    /* Compute autocorrelations, added over subframes */
    C0_64 = silk_inner_prod16_aligned_64( x, x, subfr_length*nb_subfr, arch );
    lz = silk_CLZ64(C0_64);
    rshifts = 32 + 1 + N_BITS_HEAD_ROOM - lz;
    if (rshifts > MAX_RSHIFTS) rshifts = MAX_RSHIFTS;
    if (rshifts < MIN_RSHIFTS) rshifts = MIN_RSHIFTS;

    if (rshifts > 0) {
        C0 = (opus_int32)silk_RSHIFT64(C0_64, rshifts );
    } else {
        C0 = silk_LSHIFT32((opus_int32)C0_64, -rshifts );
    }

    CAb[ 0 ] = CAf[ 0 ] = C0 + silk_SMMUL( SILK_FIX_CONST( FIND_LPC_COND_FAC, 32 ), C0 ) + 1;                                /* Q(-rshifts) */
    silk_memset( C_first_row, 0, SILK_MAX_ORDER_LPC * sizeof( opus_int32 ) );
    if( rshifts > 0 ) {
        for( s = 0; s < nb_subfr; s++ ) {
            x_ptr = x + s * subfr_length;
            for( n = 1; n < D + 1; n++ ) {
                C_first_row[ n - 1 ] += (opus_int32)silk_RSHIFT64(
                    silk_inner_prod16_aligned_64( x_ptr, x_ptr + n, subfr_length - n, arch ), rshifts );
            }
        }
    } else {
        for( s = 0; s < nb_subfr; s++ ) {
            int i;
            opus_int32 d;
            x_ptr = x + s * subfr_length;
            celt_pitch_xcorr(x_ptr, x_ptr + 1, xcorr, subfr_length - D, D, arch );
            for( n = 1; n < D + 1; n++ ) {
               for ( i = n + subfr_length - D, d = 0; i < subfr_length; i++ )
                  d = MAC16_16( d, x_ptr[ i ], x_ptr[ i - n ] );
               xcorr[ n - 1 ] += d;
            }
            for( n = 1; n < D + 1; n++ ) {
                C_first_row[ n - 1 ] += silk_LSHIFT32( xcorr[ n - 1 ], -rshifts );
            }
        }
    }
    silk_memcpy( C_last_row, C_first_row, SILK_MAX_ORDER_LPC * sizeof( opus_int32 ) );

    /* Initialize */
    CAb[ 0 ] = CAf[ 0 ] = C0 + silk_SMMUL( SILK_FIX_CONST( FIND_LPC_COND_FAC, 32 ), C0 ) + 1;                                /* Q(-rshifts) */

    invGain_Q30 = (opus_int32)1 << 30;
    reached_max_gain = 0;
    for( n = 0; n < D; n++ ) {
        /* Update first row of correlation matrix (without first element) */
        /* Update last row of correlation matrix (without last element, stored in reversed order) */
        /* Update C * Af */
        /* Update C * flipud(Af) (stored in reversed order) */
        if( rshifts > -2 ) {
            for( s = 0; s < nb_subfr; s++ ) {
                x_ptr = x + s * subfr_length;
                x1  = -silk_LSHIFT32( (opus_int32)x_ptr[ n ],                    16 - rshifts );        /* Q(16-rshifts) */
                x2  = -silk_LSHIFT32( (opus_int32)x_ptr[ subfr_length - n - 1 ], 16 - rshifts );        /* Q(16-rshifts) */
                tmp1 = silk_LSHIFT32( (opus_int32)x_ptr[ n ],                    QA - 16 );             /* Q(QA-16) */
                tmp2 = silk_LSHIFT32( (opus_int32)x_ptr[ subfr_length - n - 1 ], QA - 16 );             /* Q(QA-16) */
                for( k = 0; k < n; k++ ) {
                    C_first_row[ k ] = silk_SMLAWB( C_first_row[ k ], x1, x_ptr[ n - k - 1 ]            ); /* Q( -rshifts ) */
                    C_last_row[ k ]  = silk_SMLAWB( C_last_row[ k ],  x2, x_ptr[ subfr_length - n + k ] ); /* Q( -rshifts ) */
                    Atmp_QA = Af_QA[ k ];
                    tmp1 = silk_SMLAWB( tmp1, Atmp_QA, x_ptr[ n - k - 1 ]            );                 /* Q(QA-16) */
                    tmp2 = silk_SMLAWB( tmp2, Atmp_QA, x_ptr[ subfr_length - n + k ] );                 /* Q(QA-16) */
                }
                tmp1 = silk_LSHIFT32( -tmp1, 32 - QA - rshifts );                                       /* Q(16-rshifts) */
                tmp2 = silk_LSHIFT32( -tmp2, 32 - QA - rshifts );                                       /* Q(16-rshifts) */
                for( k = 0; k <= n; k++ ) {
                    CAf[ k ] = silk_SMLAWB( CAf[ k ], tmp1, x_ptr[ n - k ]                    );        /* Q( -rshift ) */
                    CAb[ k ] = silk_SMLAWB( CAb[ k ], tmp2, x_ptr[ subfr_length - n + k - 1 ] );        /* Q( -rshift ) */
                }
            }
        } else {
            for( s = 0; s < nb_subfr; s++ ) {
                x_ptr = x + s * subfr_length;
                x1  = -silk_LSHIFT32( (opus_int32)x_ptr[ n ],                    -rshifts );            /* Q( -rshifts ) */
                x2  = -silk_LSHIFT32( (opus_int32)x_ptr[ subfr_length - n - 1 ], -rshifts );            /* Q( -rshifts ) */
                tmp1 = silk_LSHIFT32( (opus_int32)x_ptr[ n ],                    17 );                  /* Q17 */
                tmp2 = silk_LSHIFT32( (opus_int32)x_ptr[ subfr_length - n - 1 ], 17 );                  /* Q17 */
                for( k = 0; k < n; k++ ) {
                    C_first_row[ k ] = silk_MLA( C_first_row[ k ], x1, x_ptr[ n - k - 1 ]            ); /* Q( -rshifts ) */
                    C_last_row[ k ]  = silk_MLA( C_last_row[ k ],  x2, x_ptr[ subfr_length - n + k ] ); /* Q( -rshifts ) */
                    Atmp1 = silk_RSHIFT_ROUND( Af_QA[ k ], QA - 17 );                                   /* Q17 */
                    /* We sometimes have get overflows in the multiplications (even beyond +/- 2^32),
                       but they cancel each other and the real result seems to always fit in a 32-bit
                       signed integer. This was determined experimentally, not theoretically (unfortunately). */
                    tmp1 = silk_MLA_ovflw( tmp1, x_ptr[ n - k - 1 ],            Atmp1 );                      /* Q17 */
                    tmp2 = silk_MLA_ovflw( tmp2, x_ptr[ subfr_length - n + k ], Atmp1 );                      /* Q17 */
                }
                tmp1 = -tmp1;                                                                           /* Q17 */
                tmp2 = -tmp2;                                                                           /* Q17 */
                for( k = 0; k <= n; k++ ) {
                    CAf[ k ] = silk_SMLAWW( CAf[ k ], tmp1,
                        silk_LSHIFT32( (opus_int32)x_ptr[ n - k ], -rshifts - 1 ) );                    /* Q( -rshift ) */
                    CAb[ k ] = silk_SMLAWW( CAb[ k ], tmp2,
                        silk_LSHIFT32( (opus_int32)x_ptr[ subfr_length - n + k - 1 ], -rshifts - 1 ) ); /* Q( -rshift ) */
                }
            }
        }

        /* Calculate nominator and denominator for the next order reflection (parcor) coefficient */
        tmp1 = C_first_row[ n ];                                                                        /* Q( -rshifts ) */
        tmp2 = C_last_row[ n ];                                                                         /* Q( -rshifts ) */
        num  = 0;                                                                                       /* Q( -rshifts ) */
        nrg  = silk_ADD32( CAb[ 0 ], CAf[ 0 ] );                                                        /* Q( 1-rshifts ) */
        for( k = 0; k < n; k++ ) {
            Atmp_QA = Af_QA[ k ];
            lz = silk_CLZ32( silk_abs( Atmp_QA ) ) - 1;
            lz = silk_min( 32 - QA, lz );
            Atmp1 = silk_LSHIFT32( Atmp_QA, lz );                                                       /* Q( QA + lz ) */

            tmp1 = silk_ADD_LSHIFT32( tmp1, silk_SMMUL( C_last_row[  n - k - 1 ], Atmp1 ), 32 - QA - lz );  /* Q( -rshifts ) */
            tmp2 = silk_ADD_LSHIFT32( tmp2, silk_SMMUL( C_first_row[ n - k - 1 ], Atmp1 ), 32 - QA - lz );  /* Q( -rshifts ) */
            num  = silk_ADD_LSHIFT32( num,  silk_SMMUL( CAb[ n - k ],             Atmp1 ), 32 - QA - lz );  /* Q( -rshifts ) */
            nrg  = silk_ADD_LSHIFT32( nrg,  silk_SMMUL( silk_ADD32( CAb[ k + 1 ], CAf[ k + 1 ] ),
                                                                                Atmp1 ), 32 - QA - lz );    /* Q( 1-rshifts ) */
        }
        CAf[ n + 1 ] = tmp1;                                                                            /* Q( -rshifts ) */
        CAb[ n + 1 ] = tmp2;                                                                            /* Q( -rshifts ) */
        num = silk_ADD32( num, tmp2 );                                                                  /* Q( -rshifts ) */
        num = silk_LSHIFT32( -num, 1 );                                                                 /* Q( 1-rshifts ) */

        /* Calculate the next order reflection (parcor) coefficient */
        if( silk_abs( num ) < nrg ) {
            rc_Q31 = silk_DIV32_varQ( num, nrg, 31 );
        } else {
            rc_Q31 = ( num > 0 ) ? silk_int32_MAX : silk_int32_MIN;
        }

        /* Update inverse prediction gain */
        tmp1 = ( (opus_int32)1 << 30 ) - silk_SMMUL( rc_Q31, rc_Q31 );
        tmp1 = silk_LSHIFT( silk_SMMUL( invGain_Q30, tmp1 ), 2 );
        if( tmp1 <= minInvGain_Q30 ) {
            /* Max prediction gain exceeded; set reflection coefficient such that max prediction gain is exactly hit */
            tmp2 = ( (opus_int32)1 << 30 ) - silk_DIV32_varQ( minInvGain_Q30, invGain_Q30, 30 );            /* Q30 */
            rc_Q31 = silk_SQRT_APPROX( tmp2 );                                                  /* Q15 */
            if( rc_Q31 > 0 ) {
                /* Newton-Raphson iteration */
                rc_Q31 = silk_RSHIFT32( rc_Q31 + silk_DIV32( tmp2, rc_Q31 ), 1 );                       /* Q15 */
                rc_Q31 = silk_LSHIFT32( rc_Q31, 16 );                                                   /* Q31 */
                if( num < 0 ) {
                    /* Ensure adjusted reflection coefficients has the original sign */
                    rc_Q31 = -rc_Q31;
                }
            }
            invGain_Q30 = minInvGain_Q30;
            reached_max_gain = 1;
        } else {
            invGain_Q30 = tmp1;
        }

        /* Update the AR coefficients */
        for( k = 0; k < (n + 1) >> 1; k++ ) {
            tmp1 = Af_QA[ k ];                                                                  /* QA */
            tmp2 = Af_QA[ n - k - 1 ];                                                          /* QA */
            Af_QA[ k ]         = silk_ADD_LSHIFT32( tmp1, silk_SMMUL( tmp2, rc_Q31 ), 1 );      /* QA */
            Af_QA[ n - k - 1 ] = silk_ADD_LSHIFT32( tmp2, silk_SMMUL( tmp1, rc_Q31 ), 1 );      /* QA */
        }
        Af_QA[ n ] = silk_RSHIFT32( rc_Q31, 31 - QA );                                          /* QA */

        if( reached_max_gain ) {
            /* Reached max prediction gain; set remaining coefficients to zero and exit loop */
            for( k = n + 1; k < D; k++ ) {
                Af_QA[ k ] = 0;
            }
            break;
        }

        /* Update C * Af and C * Ab */
        for( k = 0; k <= n + 1; k++ ) {
            tmp1 = CAf[ k ];                                                                    /* Q( -rshifts ) */
            tmp2 = CAb[ n - k + 1 ];                                                            /* Q( -rshifts ) */
            CAf[ k ]         = silk_ADD_LSHIFT32( tmp1, silk_SMMUL( tmp2, rc_Q31 ), 1 );        /* Q( -rshifts ) */
            CAb[ n - k + 1 ] = silk_ADD_LSHIFT32( tmp2, silk_SMMUL( tmp1, rc_Q31 ), 1 );        /* Q( -rshifts ) */
        }
    }

    if( reached_max_gain ) {
        for( k = 0; k < D; k++ ) {
            /* Scale coefficients */
            A_Q16[ k ] = -silk_RSHIFT_ROUND( Af_QA[ k ], QA - 16 );
        }
        /* Subtract energy of preceding samples from C0 */
        if( rshifts > 0 ) {
            for( s = 0; s < nb_subfr; s++ ) {
                x_ptr = x + s * subfr_length;
                C0 -= (opus_int32)silk_RSHIFT64( silk_inner_prod16_aligned_64( x_ptr, x_ptr, D, arch ), rshifts );
            }
        } else {
            for( s = 0; s < nb_subfr; s++ ) {
                x_ptr = x + s * subfr_length;
                C0 -= silk_LSHIFT32( silk_inner_prod_aligned( x_ptr, x_ptr, D, arch), -rshifts);
            }
        }
        /* Approximate residual energy */
        *res_nrg = silk_LSHIFT( silk_SMMUL( invGain_Q30, C0 ), 2 );
        *res_nrg_Q = -rshifts;
    } else {
        /* Return residual energy */
        nrg  = CAf[ 0 ];                                                                            /* Q( -rshifts ) */
        tmp1 = (opus_int32)1 << 16;                                                                             /* Q16 */
        for( k = 0; k < D; k++ ) {
            Atmp1 = silk_RSHIFT_ROUND( Af_QA[ k ], QA - 16 );                                       /* Q16 */
            nrg  = silk_SMLAWW( nrg, CAf[ k + 1 ], Atmp1 );                                         /* Q( -rshifts ) */
            tmp1 = silk_SMLAWW( tmp1, Atmp1, Atmp1 );                                               /* Q16 */
            A_Q16[ k ] = -Atmp1;
        }
        *res_nrg = silk_SMLAWW( nrg, silk_SMMUL( SILK_FIX_CONST( FIND_LPC_COND_FAC, 32 ), C0 ), -tmp1 );/* Q( -rshifts ) */
        *res_nrg_Q = -rshifts;
    }
}
#else
void silk_burg_modified_c(
    opus_int32                  *res_nrg,           /* O    Residual energy                                             */
    opus_int                    *res_nrg_Q,         /* O    Residual energy Q value                                     */
    opus_int32                  A_Q16[],            /* O    Prediction coefficients (length order)                      */
    const opus_int16            x[],                /* I    Input signal, length: nb_subfr * ( D + subfr_length )       */
    const opus_int32            minInvGain_Q30,     /* I    Inverse of max prediction gain                              */
    const opus_int              subfr_length,       /* I    Input signal subframe length (incl. D preceding samples)    */
    const opus_int              nb_subfr,           /* I    Number of subframes stacked in x                            */
    const opus_int              D,                  /* I    Order                                                       */
    int                         arch                /* I    Run-time architecture                                       */
)
{
    int frame_size = subfr_length * nb_subfr;
    celt_assert(frame_size <= MAX_FRAME_SIZE);
    int C0 = 0, invGain_Q30 = 1 << 30, reached_max_gain = 0;
    int CAb[25], CAf[25];
    int C_first_row[24], xcorr[24], C_last_row[24], Af_QA[24];
    const short * x_ptr;
    /* Compute autocorrelations, added over subframes */
    ae_int64 C0_64 = silk_inner_prod16_aligned_64(x, x, frame_size, arch);
    int rshifts = 32 + N_BITS_HEAD_ROOM - AE_NSA64(C0_64);
    if (rshifts > MAX_RSHIFTS) rshifts = MAX_RSHIFTS;
    if (rshifts < MIN_RSHIFTS) rshifts = MIN_RSHIFTS;
    if (rshifts > 0) {
        C0 = (int)(int64_t)(C0_64 >> rshifts);
    } else {
        C0 = (int)(int64_t)(C0_64 << -rshifts);
    }
    int tmp = SILK_FIX_CONST(FIND_LPC_COND_FAC, 32);
    CAb[0] = CAf[0] = C0 + XT_MULSH(tmp , C0) + 1; /* Q(-rshifts) *///
    silk_memset(C_first_row, 0, SILK_MAX_ORDER_LPC * sizeof(opus_int32));
    if(rshifts > 0) {
        for(int s = 0; s < nb_subfr; s++) {
            x_ptr = x + s * subfr_length;
            for(int n = 1; n < D + 1; n++) {
                ae_int64 tmp1 = (int64_t)silk_inner_prod16_aligned_64(x_ptr, x_ptr + n, subfr_length - n, arch);
                int tmp2 = (int)(int64_t)(tmp1 >> rshifts);
                C_first_row[n - 1] += tmp2;
            }
        }
    } else {
        for(int s = 0; s < nb_subfr; s++) {
            x_ptr = x + s * subfr_length;
            int offset = subfr_length - D;
            celt_pitch_xcorr(x_ptr, x_ptr + 1, xcorr, offset, D, arch);
            int loops = D + 1;
            for(int n = 1; n < loops; n++) {
                ae_int64 sum = xcorr[n - 1];
                int i =  n + offset;
                for (; i < subfr_length; i++)
                    AE_MULA16_00(sum, x_ptr[i], x_ptr[i - n]);
                int d = (int)(int64_t)sum;
                C_first_row[n - 1] += d << -rshifts;
                xcorr[n - 1] = d;
            }
        }
    }
    silk_memcpy(C_last_row, C_first_row, SILK_MAX_ORDER_LPC * sizeof(opus_int32));
    for(int n = 0; n < D; n++) {
        if(rshifts > -2) {
            for(int s = 0; s < nb_subfr; s++) {
                //ZY:Is it better to use AE_MULAFP32X16X2RS?
                x_ptr = x + s * subfr_length;
                int a1 = x_ptr[n];
                int b1 = x_ptr[subfr_length - n - 1];
                int shift = 16 - rshifts;
                int x1  = -(a1 << shift); /* Q(16-rshifts) */
                int x2  = -(b1 << shift); /* Q(16-rshifts) */
                int tmp1 = a1 << (QA - 16); /* Q(QA-16) */
                int tmp2 = b1 << (QA - 16); /* Q(QA-16) */
                const short * src1 = x_ptr + n - 1;
                const short * src2 = x_ptr + subfr_length - n;
                for(int k = 0; k < n; k++) {
                    short s1 = *src1--;
                    short s2 = *src2++;
                    C_first_row[k] += (int)(int64_t)(AE_MUL32X16_L0(x1, s1) >> 16); /* Q( -rshifts ) */
                    C_last_row[k] += (int)(int64_t)(AE_MUL32X16_L0(x2, s2) >> 16); /* Q( -rshifts ) */
                    int Atmp_QA = Af_QA[k];
                    tmp1 += (int)(int64_t)(AE_MUL32X16_L0(Atmp_QA, s1) >> 16); /* Q(QA-16) */
                    tmp2 += (int)(int64_t)(AE_MUL32X16_L0(Atmp_QA, s2) >> 16); /* Q(QA-16) */
                }
                shift = 32 - QA - rshifts;
                tmp1 = -tmp1 << shift; /* Q(16-rshifts) */
                tmp2 = -tmp2 << shift; /* Q(16-rshifts) */
                src1 = x_ptr + n;
                src2 = x_ptr + subfr_length - n - 1;
                for(int k = 0; k <= n; k++) {
                    short s1 = *src1--;
                    short s2 = *src2++;
                    CAf[k] += (int)(int64_t)(AE_MUL32X16_L0(tmp1, s1) >> 16); /* Q( -rshift ) */
                    CAb[k] += (int)(int64_t)(AE_MUL32X16_L0(tmp2, s2) >> 16); /* Q( -rshift ) */
                }
            }
        } else {
            for(int s = 0; s < nb_subfr; s++) {
                //ZY:Is it better to use AE_MULAAAAQ32X16?
                x_ptr = x + s * subfr_length;
                int a1 = x_ptr[n];
                int b1 = x_ptr[subfr_length - n - 1];
                int shift = -rshifts;
                int x1 = -(a1 << shift); /* Q( -rshifts ) */
                int x2 = -(b1 << shift); /* Q( -rshifts ) */
                ae_int64 sum_tmp1 = a1 << 17; /* Q17 */
                ae_int64 sum_tmp2 = b1 << 17; /* Q17 */
                const short * src1 = x_ptr + n - 1;
                const short * src2 = x_ptr + subfr_length - n;
                for(int k = 0; k < n; k++) {
                    short s1 = *src1--;
                    short s2 = *src2++;
                    ae_int64 sum1 = C_first_row[k];
                    ae_int64 sum2 = C_last_row[k];
                    AE_MULA32X16_L0(sum1, x1, s1);
                    AE_MULA32X16_L0(sum2, x2, s2);
                    C_first_row[k] = (int)(int64_t)sum1; /* Q( -rshifts ) */
                    C_last_row[k] = (int)(int64_t)sum2; /* Q( -rshifts ) */
                    int Atmp1 = silk_RSHIFT_ROUND(Af_QA[k], QA - 17); /* Q17 */
                    /* We sometimes have get overflows in the multiplications (even beyond +/- 2^32),
                       but they cancel each other and the real result seems to always fit in a 32-bit
                       signed integer. This was determined experimentally, not theoretically (unfortunately). */
                    AE_MULA32X16_L0(sum_tmp1, Atmp1, s1); /* Q17 */
                    AE_MULA32X16_L0(sum_tmp2, Atmp1, s2); /* Q17 */
                }
                int tmp1 = (int)(int64_t)(-sum_tmp1); /* Q17 */
                int tmp2 = (int)(int64_t)(-sum_tmp2); /* Q17 */
                for(int k = 0; k <= n; k++) {
                    int c1 = x_ptr[n - k] << (-rshifts - 1);
                    int c2 = x_ptr[subfr_length - n - 1 + k] << (-rshifts - 1);
                    CAf[k] += (int)(int64_t)(AE_MUL32_LL(tmp1, c1) >> 16); /* Q( -rshift ) */
                    CAb[k] += (int)(int64_t)(AE_MUL32_LL(tmp2, c2) >> 16); /* Q( -rshift ) */
                }
            }
        }
        /* Calculate nominator and denominator for the next order reflection (parcor) coefficient */
        int tmp1 = C_first_row[n]; /* Q( -rshifts ) */
        int tmp2 = C_last_row[n]; /* Q( -rshifts ) */
        int num  = 0; /* Q( -rshifts ) */
        int nrg  = CAb[0] + CAf[0]; /* Q( 1-rshifts ) */
        int idx = n - 1;
        for(int k = 0; k < n; k++) {
            int Atmp_QA = Af_QA[k];
            int lz = XT_NSA(XT_ABS(Atmp_QA));
            lz = XT_MIN(32 - QA, lz);
            int Atmp1 = Atmp_QA << lz; /* Q( QA + lz ) */
            int shift = 32 - QA - lz;
            int b1 = XT_MULSH(C_last_row[idx - k], Atmp1);
            tmp1 += b1 << shift; /* Q( -rshifts ) */
            int b2 = XT_MULSH(C_first_row[idx - k ], Atmp1);
            tmp2 += b2 << shift; /* Q( -rshifts ) */
            int b3 = XT_MULSH(CAb[n - k], Atmp1);
            num += b3 << shift; /* Q( -rshifts ) */
            int b4 = XT_MULSH((CAb[k + 1] + CAf[k + 1]), Atmp1);
            nrg += b4 << shift; /* Q( 1-rshifts ) */
        }
        CAf[n + 1] = tmp1; /* Q( -rshifts ) */
        CAb[n + 1] = tmp2; /* Q( -rshifts ) */
        num += tmp2; /* Q( -rshifts ) */
        num = -num << 1; /* Q( 1-rshifts ) */
        /* Calculate the next order reflection (parcor) coefficient */
        int rc_Q31 = 0;
        if(XT_ABS(num) < nrg) {
            rc_Q31 = silk_DIV32_varQ(num, nrg, 31);
        } else {
            rc_Q31 = (num > 0) ? silk_int32_MAX : silk_int32_MIN;
        }
        /* Update inverse prediction gain */
        tmp1 = ((int)1 << 30) - XT_MULSH(rc_Q31, rc_Q31);
        tmp1 = XT_MULSH(invGain_Q30, tmp1) << 2;
        if(tmp1 <= minInvGain_Q30) {
            /* Max prediction gain exceeded; set reflection coefficient such that max prediction gain is exactly hit */
            tmp2 = ((int)1 << 30) - silk_DIV32_varQ(minInvGain_Q30, invGain_Q30, 30); /* Q30 */
            rc_Q31 = silk_SQRT_APPROX(tmp2); /* Q15 */
            if(rc_Q31 > 0) {
                /* Newton-Raphson iteration */
                rc_Q31 = (rc_Q31 + tmp2 / rc_Q31) >> 1; /* Q15 */
                rc_Q31 = rc_Q31 << 16; /* Q31 */
                if(num < 0) {
                    /* Ensure adjusted reflection coefficients has the original sign */
                    rc_Q31 = -rc_Q31;
                }
            }
            invGain_Q30 = minInvGain_Q30;
            reached_max_gain = 1;
        } else {
            invGain_Q30 = tmp1;
        }
        /* Update the AR coefficients */
        int loops = (n + 1) >> 1;
        for(int k = 0; k < loops; k++) {
            tmp1 = Af_QA[k]; /* QA */
            tmp2 = Af_QA[n - k - 1]; /* QA */
            Af_QA[k] = tmp1 + (XT_MULSH(tmp2, rc_Q31) << 1); /* QA */
            Af_QA[n - k - 1] = tmp2 + (XT_MULSH(tmp1, rc_Q31) << 1); /* QA */
        }
        Af_QA[n] = rc_Q31 >> (31 - QA); /* QA */
        if(reached_max_gain) {
            /* Reached max prediction gain; set remaining coefficients to zero and exit loop */
            for(int k = n + 1; k < D; k++) {
                Af_QA[k] = 0;
            }
            break;
        }
        /* Update C * Af and C * Ab */
        for(int k = 0; k <= n + 1; k++) {
            tmp1 = CAf[k]; /* Q( -rshifts ) */
            tmp2 = CAb[n - k + 1 ]; /* Q( -rshifts ) */
            CAf[k] = tmp1 + (XT_MULSH(tmp2, rc_Q31) << 1); /* Q( -rshifts ) */
            CAb[n - k + 1] = tmp2 + (XT_MULSH(tmp1, rc_Q31) << 1); /* Q( -rshifts ) */
        }
    }
    if(reached_max_gain) {
        for(int k = 0; k < D; k++) {
            /* Scale coefficients */
            A_Q16[k] = -silk_RSHIFT_ROUND(Af_QA[k], QA - 16);
        }
        /* Subtract energy of preceding samples from C0 */
        if(rshifts > 0) {
            for(int s = 0; s < nb_subfr; s++) {
                x_ptr = x + s * subfr_length;
                C0 -= (opus_int32)silk_RSHIFT64((int64_t)silk_inner_prod16_aligned_64(x_ptr, x_ptr, D, arch), rshifts);
            }
        } else {
            for(int s = 0; s < nb_subfr; s++) {
                x_ptr = x + s * subfr_length;
                C0 -= silk_LSHIFT32(silk_inner_prod_aligned(x_ptr, x_ptr, D, arch), -rshifts);
            }
        }
        /* Approximate residual energy */
        *res_nrg = XT_MULSH(invGain_Q30, C0) << 2;
        *res_nrg_Q = -rshifts;
    } else {
        /* Return residual energy */
        int nrg  = CAf[0]; /* Q( -rshifts ) */
        int tmp1 = (opus_int32)1 << 16; /* Q16 */
        for(int k = 0; k < D; k++) {
            int Atmp1 = silk_RSHIFT_ROUND(Af_QA[k], QA - 16); /* Q16 */
            nrg += (int)(int64_t)(AE_MUL32_LL(CAf[k + 1], Atmp1) >> 16); /* Q( -rshifts ) */
            tmp1 += (int)(int64_t)(AE_MUL32_LL(Atmp1, Atmp1) >> 16); /* Q16 */
            A_Q16[k] = -Atmp1;
        }
        int c = SILK_FIX_CONST(FIND_LPC_COND_FAC, 32);
        c = XT_MULSH(c, C0);
        *res_nrg = nrg + (int)(int64_t)(AE_MUL32_LL(c, -tmp1) >> 16); /* Q( -rshifts ) */
        *res_nrg_Q = -rshifts;
    }
}
#endif