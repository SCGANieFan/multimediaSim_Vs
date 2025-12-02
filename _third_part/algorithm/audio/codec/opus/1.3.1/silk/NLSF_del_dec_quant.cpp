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

#include "main.h"

/* Delayed-decision quantizer for NLSF residuals */
#ifndef HIFI_OPT
opus_int32 silk_NLSF_del_dec_quant(                             /* O    Returns RD value in Q25                     */
    opus_int8                   indices[],                      /* O    Quantization indices [ order ]              */
    const opus_int16            x_Q10[],                        /* I    Input [ order ]                             */
    const opus_int16            w_Q5[],                         /* I    Weights [ order ]                           */
    const opus_uint8            pred_coef_Q8[],                 /* I    Backward predictor coefs [ order ]          */
    const opus_int16            ec_ix[],                        /* I    Indices to entropy coding tables [ order ]  */
    const opus_uint8            ec_rates_Q5[],                  /* I    Rates []                                    */
    const opus_int              quant_step_size_Q16,            /* I    Quantization step size                      */
    const opus_int16            inv_quant_step_size_Q6,         /* I    Inverse quantization step size              */
    const opus_int32            mu_Q20,                         /* I    R/D tradeoff                                */
    const opus_int16            order                           /* I    Number of input values                      */
)
{
    opus_int         i, j, nStates, ind_tmp, ind_min_max, ind_max_min, in_Q10, res_Q10;
    opus_int         pred_Q10, diff_Q10, rate0_Q5, rate1_Q5;
    opus_int16       out0_Q10, out1_Q10;
    opus_int32       RD_tmp_Q25, min_Q25, min_max_Q25, max_min_Q25;
    opus_int         ind_sort[         NLSF_QUANT_DEL_DEC_STATES ];
    opus_int8        ind[              NLSF_QUANT_DEL_DEC_STATES ][ MAX_LPC_ORDER ];
    opus_int16       prev_out_Q10[ 2 * NLSF_QUANT_DEL_DEC_STATES ];
    opus_int32       RD_Q25[       2 * NLSF_QUANT_DEL_DEC_STATES ];
    opus_int32       RD_min_Q25[       NLSF_QUANT_DEL_DEC_STATES ];
    opus_int32       RD_max_Q25[       NLSF_QUANT_DEL_DEC_STATES ];
    const opus_uint8 *rates_Q5;

    opus_int out0_Q10_table[2 * NLSF_QUANT_MAX_AMPLITUDE_EXT];
    opus_int out1_Q10_table[2 * NLSF_QUANT_MAX_AMPLITUDE_EXT];

    for (i = -NLSF_QUANT_MAX_AMPLITUDE_EXT; i <= NLSF_QUANT_MAX_AMPLITUDE_EXT-1; i++)
    {
        out0_Q10 = silk_LSHIFT( i, 10 );
        out1_Q10 = silk_ADD16( out0_Q10, 1024 );
        if( i > 0 ) {
            out0_Q10 = silk_SUB16( out0_Q10, SILK_FIX_CONST( NLSF_QUANT_LEVEL_ADJ, 10 ) );
            out1_Q10 = silk_SUB16( out1_Q10, SILK_FIX_CONST( NLSF_QUANT_LEVEL_ADJ, 10 ) );
        } else if( i == 0 ) {
            out1_Q10 = silk_SUB16( out1_Q10, SILK_FIX_CONST( NLSF_QUANT_LEVEL_ADJ, 10 ) );
        } else if( i == -1 ) {
            out0_Q10 = silk_ADD16( out0_Q10, SILK_FIX_CONST( NLSF_QUANT_LEVEL_ADJ, 10 ) );
        } else {
            out0_Q10 = silk_ADD16( out0_Q10, SILK_FIX_CONST( NLSF_QUANT_LEVEL_ADJ, 10 ) );
            out1_Q10 = silk_ADD16( out1_Q10, SILK_FIX_CONST( NLSF_QUANT_LEVEL_ADJ, 10 ) );
        }
        out0_Q10_table[ i + NLSF_QUANT_MAX_AMPLITUDE_EXT ] = silk_RSHIFT( silk_SMULBB( out0_Q10, quant_step_size_Q16 ), 16 );
        out1_Q10_table[ i + NLSF_QUANT_MAX_AMPLITUDE_EXT ] = silk_RSHIFT( silk_SMULBB( out1_Q10, quant_step_size_Q16 ), 16 );
    }

    silk_assert( (NLSF_QUANT_DEL_DEC_STATES & (NLSF_QUANT_DEL_DEC_STATES-1)) == 0 );     /* must be power of two */

    nStates = 1;
    RD_Q25[ 0 ] = 0;
    prev_out_Q10[ 0 ] = 0;
    for( i = order - 1; i >= 0; i-- ) {
        rates_Q5 = &ec_rates_Q5[ ec_ix[ i ] ];
        in_Q10 = x_Q10[ i ];
        for( j = 0; j < nStates; j++ ) {
            pred_Q10 = silk_RSHIFT( silk_SMULBB( (opus_int16)pred_coef_Q8[ i ], prev_out_Q10[ j ] ), 8 );
            res_Q10  = silk_SUB16( in_Q10, pred_Q10 );
            ind_tmp  = silk_RSHIFT( silk_SMULBB( inv_quant_step_size_Q6, res_Q10 ), 16 );
            ind_tmp  = silk_LIMIT( ind_tmp, -NLSF_QUANT_MAX_AMPLITUDE_EXT, NLSF_QUANT_MAX_AMPLITUDE_EXT-1 );
            ind[ j ][ i ] = (opus_int8)ind_tmp;

            /* compute outputs for ind_tmp and ind_tmp + 1 */
            out0_Q10 = out0_Q10_table[ ind_tmp + NLSF_QUANT_MAX_AMPLITUDE_EXT ];
            out1_Q10 = out1_Q10_table[ ind_tmp + NLSF_QUANT_MAX_AMPLITUDE_EXT ];

            out0_Q10  = silk_ADD16( out0_Q10, pred_Q10 );
            out1_Q10  = silk_ADD16( out1_Q10, pred_Q10 );
            prev_out_Q10[ j           ] = out0_Q10;
            prev_out_Q10[ j + nStates ] = out1_Q10;

            /* compute RD for ind_tmp and ind_tmp + 1 */
            if( ind_tmp + 1 >= NLSF_QUANT_MAX_AMPLITUDE ) {
                if( ind_tmp + 1 == NLSF_QUANT_MAX_AMPLITUDE ) {
                    rate0_Q5 = rates_Q5[ ind_tmp + NLSF_QUANT_MAX_AMPLITUDE ];
                    rate1_Q5 = 280;
                } else {
                    rate0_Q5 = silk_SMLABB( 280 - 43 * NLSF_QUANT_MAX_AMPLITUDE, 43, ind_tmp );
                    rate1_Q5 = silk_ADD16( rate0_Q5, 43 );
                }
            } else if( ind_tmp <= -NLSF_QUANT_MAX_AMPLITUDE ) {
                if( ind_tmp == -NLSF_QUANT_MAX_AMPLITUDE ) {
                    rate0_Q5 = 280;
                    rate1_Q5 = rates_Q5[ ind_tmp + 1 + NLSF_QUANT_MAX_AMPLITUDE ];
                } else {
                    rate0_Q5 = silk_SMLABB( 280 - 43 * NLSF_QUANT_MAX_AMPLITUDE, -43, ind_tmp );
                    rate1_Q5 = silk_SUB16( rate0_Q5, 43 );
                }
            } else {
                rate0_Q5 = rates_Q5[ ind_tmp +     NLSF_QUANT_MAX_AMPLITUDE ];
                rate1_Q5 = rates_Q5[ ind_tmp + 1 + NLSF_QUANT_MAX_AMPLITUDE ];
            }
            RD_tmp_Q25            = RD_Q25[ j ];
            diff_Q10              = silk_SUB16( in_Q10, out0_Q10 );
            RD_Q25[ j ]           = silk_SMLABB( silk_MLA( RD_tmp_Q25, silk_SMULBB( diff_Q10, diff_Q10 ), w_Q5[ i ] ), mu_Q20, rate0_Q5 );
            diff_Q10              = silk_SUB16( in_Q10, out1_Q10 );
            RD_Q25[ j + nStates ] = silk_SMLABB( silk_MLA( RD_tmp_Q25, silk_SMULBB( diff_Q10, diff_Q10 ), w_Q5[ i ] ), mu_Q20, rate1_Q5 );
        }

        if( nStates <= NLSF_QUANT_DEL_DEC_STATES/2 ) {
            /* double number of states and copy */
            for( j = 0; j < nStates; j++ ) {
                ind[ j + nStates ][ i ] = ind[ j ][ i ] + 1;
            }
            nStates = silk_LSHIFT( nStates, 1 );
            for( j = nStates; j < NLSF_QUANT_DEL_DEC_STATES; j++ ) {
                ind[ j ][ i ] = ind[ j - nStates ][ i ];
            }
        } else {
            /* sort lower and upper half of RD_Q25, pairwise */
            for( j = 0; j < NLSF_QUANT_DEL_DEC_STATES; j++ ) {
                if( RD_Q25[ j ] > RD_Q25[ j + NLSF_QUANT_DEL_DEC_STATES ] ) {
                    RD_max_Q25[ j ]                         = RD_Q25[ j ];
                    RD_min_Q25[ j ]                         = RD_Q25[ j + NLSF_QUANT_DEL_DEC_STATES ];
                    RD_Q25[ j ]                             = RD_min_Q25[ j ];
                    RD_Q25[ j + NLSF_QUANT_DEL_DEC_STATES ] = RD_max_Q25[ j ];
                    /* swap prev_out values */
                    out0_Q10 = prev_out_Q10[ j ];
                    prev_out_Q10[ j ] = prev_out_Q10[ j + NLSF_QUANT_DEL_DEC_STATES ];
                    prev_out_Q10[ j + NLSF_QUANT_DEL_DEC_STATES ] = out0_Q10;
                    ind_sort[ j ] = j + NLSF_QUANT_DEL_DEC_STATES;
                } else {
                    RD_min_Q25[ j ] = RD_Q25[ j ];
                    RD_max_Q25[ j ] = RD_Q25[ j + NLSF_QUANT_DEL_DEC_STATES ];
                    ind_sort[ j ] = j;
                }
            }
            /* compare the highest RD values of the winning half with the lowest one in the losing half, and copy if necessary */
            /* afterwards ind_sort[] will contain the indices of the NLSF_QUANT_DEL_DEC_STATES winning RD values */
            while( 1 ) {
                min_max_Q25 = silk_int32_MAX;
                max_min_Q25 = 0;
                ind_min_max = 0;
                ind_max_min = 0;
                for( j = 0; j < NLSF_QUANT_DEL_DEC_STATES; j++ ) {
                    if( min_max_Q25 > RD_max_Q25[ j ] ) {
                        min_max_Q25 = RD_max_Q25[ j ];
                        ind_min_max = j;
                    }
                    if( max_min_Q25 < RD_min_Q25[ j ] ) {
                        max_min_Q25 = RD_min_Q25[ j ];
                        ind_max_min = j;
                    }
                }
                if( min_max_Q25 >= max_min_Q25 ) {
                    break;
                }
                /* copy ind_min_max to ind_max_min */
                ind_sort[     ind_max_min ] = ind_sort[     ind_min_max ] ^ NLSF_QUANT_DEL_DEC_STATES;
                RD_Q25[       ind_max_min ] = RD_Q25[       ind_min_max + NLSF_QUANT_DEL_DEC_STATES ];
                prev_out_Q10[ ind_max_min ] = prev_out_Q10[ ind_min_max + NLSF_QUANT_DEL_DEC_STATES ];
                RD_min_Q25[   ind_max_min ] = 0;
                RD_max_Q25[   ind_min_max ] = silk_int32_MAX;
                silk_memcpy( ind[ ind_max_min ], ind[ ind_min_max ], MAX_LPC_ORDER * sizeof( opus_int8 ) );
            }
            /* increment index if it comes from the upper half */
            for( j = 0; j < NLSF_QUANT_DEL_DEC_STATES; j++ ) {
                ind[ j ][ i ] += silk_RSHIFT( ind_sort[ j ], NLSF_QUANT_DEL_DEC_STATES_LOG2 );
            }
        }
    }

    /* last sample: find winner, copy indices and return RD value */
    ind_tmp = 0;
    min_Q25 = silk_int32_MAX;
    for( j = 0; j < 2 * NLSF_QUANT_DEL_DEC_STATES; j++ ) {
        if( min_Q25 > RD_Q25[ j ] ) {
            min_Q25 = RD_Q25[ j ];
            ind_tmp = j;
        }
    }
    for( j = 0; j < order; j++ ) {
        indices[ j ] = ind[ ind_tmp & ( NLSF_QUANT_DEL_DEC_STATES - 1 ) ][ j ];
        silk_assert( indices[ j ] >= -NLSF_QUANT_MAX_AMPLITUDE_EXT );
        silk_assert( indices[ j ] <=  NLSF_QUANT_MAX_AMPLITUDE_EXT );
    }
    indices[ 0 ] += silk_RSHIFT( ind_tmp, NLSF_QUANT_DEL_DEC_STATES_LOG2 );
    silk_assert( indices[ 0 ] <= NLSF_QUANT_MAX_AMPLITUDE_EXT );
    silk_assert( min_Q25 >= 0 );
    return min_Q25;
}
#else
opus_int32 silk_NLSF_del_dec_quant(                             /* O    Returns RD value in Q25                     */
    opus_int8                   indices[],                      /* O    Quantization indices [ order ]              */
    const opus_int16            x_Q10[],                        /* I    Input [ order ]                             */
    const opus_int16            w_Q5[],                         /* I    Weights [ order ]                           */
    const opus_uint8            pred_coef_Q8[],                 /* I    Backward predictor coefs [ order ]          */
    const opus_int16            ec_ix[],                        /* I    Indices to entropy coding tables [ order ]  */
    const opus_uint8            ec_rates_Q5[],                  /* I    Rates []                                    */
    const opus_int              quant_step_size_Q16,            /* I    Quantization step size                      */
    const opus_int16            inv_quant_step_size_Q6,         /* I    Inverse quantization step size              */
    const opus_int32            mu_Q20,                         /* I    R/D tradeoff                                */
    const opus_int16            order                           /* I    Number of input values                      */
)
{
    silk_assert((NLSF_QUANT_DEL_DEC_STATES & (NLSF_QUANT_DEL_DEC_STATES-1)) == 0); /* must be power of two */
    static const short _out0_Q10[] __attribute__ ((aligned (8))) = {-10138, -9114, -8090, -7066, -6042,
        -5018, -3994, -2970, -1946, -922, 0, 922, 1946, 2970, 3994,
        5018, 6042, 7066, 8090, 9114};
    static const short _out1_Q10[] __attribute__ ((aligned (8))) = {-9114, -8090, -7066, -6042, -5018,
        -3994, -2970, -1946, -922, 0, 922, 1946, 2970, 3994, 5018,
        6042, 7066, 8090, 9114, 10138};
    int out0_Q10_table[20] __attribute__ ((aligned (8)));
    int out1_Q10_table[20] __attribute__ ((aligned (8)));
    ae_int16x4 q = quant_step_size_Q16;
    ae_int16x4 * src1 = (ae_int16x4 *)_out0_Q10;
    ae_int16x4 * src2 = (ae_int16x4 *)_out1_Q10;
    ae_int32x2 * dest1 = (ae_int32x2 *)out0_Q10_table;
    ae_int32x2 * dest2 = (ae_int32x2 *)out1_Q10_table;
    ae_int32x2 d1,d2,d3,d4;
    for (int i = 0; i < 5; i++) {
        AE_MUL16X4(d1, d2, *src1++, q);
        AE_MUL16X4(d3, d4, *src2++, q);
        *dest1++ = d1 >> 16;
        *dest1++ = d2 >> 16;
        *dest2++ = d3 >> 16;
        *dest2++ = d4 >> 16;
    }
    int nStates = 1;
    int RD_Q25[8]; RD_Q25[0] = 0;
    short prev_out_Q10[8]; prev_out_Q10[0] = 0;
    int i, j, ind_tmp;
    int ind_sort[4];
    opus_int8 ind[              NLSF_QUANT_DEL_DEC_STATES ][ MAX_LPC_ORDER ];
    opus_int32 RD_min_Q25[       NLSF_QUANT_DEL_DEC_STATES ];
    opus_int32 RD_max_Q25[       NLSF_QUANT_DEL_DEC_STATES ];
    for(int i = order - 1; i >= 0; i--) {
        int index = ec_ix[i];
        const unsigned char * rates_Q5 = &ec_rates_Q5[index];
        int in_Q10 = x_Q10[i];
        int coef_q8 = pred_coef_Q8[i];
        for(int j = 0; j < nStates; j++) {
            int pred_Q10 = (coef_q8 * prev_out_Q10[j] >> 8);
            int res_Q10 = in_Q10 - pred_Q10;
            int ind_tmp = inv_quant_step_size_Q6 * res_Q10 >> 16;
            if(ind_tmp > 9) ind_tmp = 9;
            if(ind_tmp < -10) ind_tmp = -10;
            ind[j][i] = (opus_int8)ind_tmp;
            /* compute outputs for ind_tmp and ind_tmp + 1 */
            int out0_Q10 = out0_Q10_table[ind_tmp + 10] + pred_Q10;
            int out1_Q10 = out1_Q10_table[ind_tmp + 10] + pred_Q10;
            prev_out_Q10[j] = out0_Q10;
            prev_out_Q10[j + nStates] = out1_Q10;
            /* compute RD for ind_tmp and ind_tmp + 1 */
            short rate0_Q5 = 0, rate1_Q5 = 0;
            if(ind_tmp >= 3) {
                if(ind_tmp == 3) {
                    rate0_Q5 = rates_Q5[ind_tmp + 4];
                    rate1_Q5 = 280;
                } else {
                    rate0_Q5 = 108 + 43 * ind_tmp;
                    rate1_Q5 = rate0_Q5 + 43;
                }
            } else if(ind_tmp <= -4) {
                if(ind_tmp == -4) {
                    rate0_Q5 = 280;
                    rate1_Q5 = rates_Q5[ind_tmp + 5];
                } else {
                    rate0_Q5 = 108 + -43 * ind_tmp;
                    rate1_Q5 = rate0_Q5 - 43;
                }
            } else {
                rate0_Q5 = rates_Q5[ind_tmp + 4];
                rate1_Q5 = rates_Q5[ind_tmp + 5];
            }
            ae_int64 d1 = RD_Q25[j];
            ae_int64 d2 = d1;
            short w_q5 = w_Q5[i];
            int diff_Q101 = in_Q10 - out0_Q10;
            int diff_Q102 = in_Q10 - out1_Q10;
            int tmp1 = (int)(int64_t)AE_MUL32_LL(diff_Q101, diff_Q101);
            int tmp2 = (int)(int64_t)AE_MUL32_LL(diff_Q102, diff_Q102);
            AE_MULA32_LL(d1, tmp1, w_q5);
            AE_MULA32_LL(d2, tmp2, w_q5);
            AE_MULA32_LL(d1, mu_Q20, rate0_Q5);
            AE_MULA32_LL(d2, mu_Q20, rate1_Q5);
            RD_Q25[j] = (int)(int64_t)d1;
            RD_Q25[j + nStates] = (int)(int64_t)d2;
        }
        if(nStates <= 2) {
            /* double number of states and copy */
            for(int j = 0; j < nStates; j++) {
                ind[j + nStates][i] = ind[j][i] + 1;
            }
            nStates = nStates << 1;
            for(int j = nStates; j < 4; j++) {
                ind[j][i] = ind[j - nStates][i];
            }
        } else {
            /* sort lower and upper half of RD_Q25, pairwise */
            for(int j = 0; j < 4; j++) {
                if(RD_Q25[j] > RD_Q25[j + 4]) {
                    RD_max_Q25[j] = RD_Q25[j];
                    RD_min_Q25[j] = RD_Q25[j + 4];
                    RD_Q25[j] = RD_min_Q25[j];
                    RD_Q25[j + 4] = RD_max_Q25[j];
                    /* swap prev_out values */
                    short out0_Q10 = prev_out_Q10[j];
                    prev_out_Q10[j] = prev_out_Q10[j + 4];
                    prev_out_Q10[j + 4] = out0_Q10;
                    ind_sort[j] = j + 4;
                } else {
                    RD_min_Q25[j] = RD_Q25[j];
                    RD_max_Q25[j] = RD_Q25[j + 4];
                    ind_sort[j] = j;
                }
            }
            /* compare the highest RD values of the winning half with the lowest one in the losing half, and copy if necessary */
            /* afterwards ind_sort[] will contain the indices of the NLSF_QUANT_DEL_DEC_STATES winning RD values */
            while( 1 ) {
                int min_max_Q25 = silk_int32_MAX;
                int max_min_Q25 = 0;
                int ind_min_max = 0;
                int ind_max_min = 0;
                for( j = 0; j < NLSF_QUANT_DEL_DEC_STATES; j++ ) {
                    if( min_max_Q25 > RD_max_Q25[ j ] ) {
                        min_max_Q25 = RD_max_Q25[ j ];
                        ind_min_max = j;
                    }
                    if( max_min_Q25 < RD_min_Q25[ j ] ) {
                        max_min_Q25 = RD_min_Q25[ j ];
                        ind_max_min = j;
                    }
                }
                if( min_max_Q25 >= max_min_Q25 ) {
                    break;
                }
                /* copy ind_min_max to ind_max_min */
                ind_sort[     ind_max_min ] = ind_sort[     ind_min_max ] ^ NLSF_QUANT_DEL_DEC_STATES;
                RD_Q25[       ind_max_min ] = RD_Q25[       ind_min_max + NLSF_QUANT_DEL_DEC_STATES ];
                prev_out_Q10[ ind_max_min ] = prev_out_Q10[ ind_min_max + NLSF_QUANT_DEL_DEC_STATES ];
                RD_min_Q25[   ind_max_min ] = 0;
                RD_max_Q25[   ind_min_max ] = silk_int32_MAX;
                silk_memcpy( ind[ ind_max_min ], ind[ ind_min_max ], MAX_LPC_ORDER * sizeof( opus_int8 ) );
            }
            /* increment index if it comes from the upper half */
            for( j = 0; j < NLSF_QUANT_DEL_DEC_STATES; j++ ) {
                ind[ j ][ i ] += silk_RSHIFT( ind_sort[ j ], NLSF_QUANT_DEL_DEC_STATES_LOG2 );
            }
        }
    }
    /* last sample: find winner, copy indices and return RD value */
    ind_tmp = 0;
    int min_Q25 = silk_int32_MAX;
    for( j = 0; j < 2 * NLSF_QUANT_DEL_DEC_STATES; j++ ) {
        if( min_Q25 > RD_Q25[ j ] ) {
            min_Q25 = RD_Q25[ j ];
            ind_tmp = j;
        }
    }
    for( j = 0; j < order; j++ ) {
        indices[ j ] = ind[ ind_tmp & ( NLSF_QUANT_DEL_DEC_STATES - 1 ) ][ j ];
        silk_assert( indices[ j ] >= -NLSF_QUANT_MAX_AMPLITUDE_EXT );
        silk_assert( indices[ j ] <=  NLSF_QUANT_MAX_AMPLITUDE_EXT );
    }
    indices[ 0 ] += silk_RSHIFT( ind_tmp, NLSF_QUANT_DEL_DEC_STATES_LOG2 );
    silk_assert( indices[ 0 ] <= NLSF_QUANT_MAX_AMPLITUDE_EXT );
    silk_assert( min_Q25 >= 0 );
    return min_Q25;
}
#endif