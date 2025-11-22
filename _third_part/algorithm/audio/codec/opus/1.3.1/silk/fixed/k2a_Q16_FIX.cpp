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

/* Step up function, converts reflection coefficients to prediction coefficients */
#ifndef HIFI_OPT
void silk_k2a_Q16(
    opus_int32                  *A_Q24,             /* O    Prediction coefficients [order] Q24                         */
    const opus_int32            *rc_Q16,            /* I    Reflection coefficients [order] Q16                         */
    const opus_int32            order               /* I    Prediction order                                            */
)
{
    opus_int   k, n;
    opus_int32 rc, tmp1, tmp2;

    for( k = 0; k < order; k++ ) {
        rc = rc_Q16[ k ];
        for( n = 0; n < (k + 1) >> 1; n++ ) {
            tmp1 = A_Q24[ n ];
            tmp2 = A_Q24[ k - n - 1 ];
            A_Q24[ n ]         = silk_SMLAWW( tmp1, tmp2, rc );
            A_Q24[ k - n - 1 ] = silk_SMLAWW( tmp2, tmp1, rc );
        }
        A_Q24[ k ] = -silk_LSHIFT( rc, 8 );
    }
}
#else
void silk_k2a_Q16(
    opus_int32                  *A_Q24,             /* O    Prediction coefficients [order] Q24                         */
    const opus_int32            *rc_Q16,            /* I    Reflection coefficients [order] Q16                         */
    const opus_int32            order               /* I    Prediction order                                            */
)
{
    opus_int   k, n;
    opus_int32 rc, tmp1, tmp2;
    rc = rc_Q16[0];
    A_Q24[0] = -silk_LSHIFT( rc, 8 );

    rc = rc_Q16[1];
	tmp1 = tmp2 = A_Q24[0];
	int32_t c1 = (int)(int64_t)(AE_MUL32_LL(tmp2, rc) >> 16);
    A_Q24[0] = tmp1 + c1;
    A_Q24[1] = -silk_LSHIFT( rc, 8 );
    rc = rc_Q16[2];
    tmp1 = A_Q24[0];
    tmp2 = A_Q24[1];
    c1 = (int)(int64_t)(AE_MUL32_LL(tmp2, rc) >> 16);
    int32_t c2 = (int)(int64_t)(AE_MUL32_LL(tmp1, rc) >> 16);
	A_Q24[ 0 ] = c1 + tmp1;
	A_Q24[ 1 ] = c2 + tmp2;
	A_Q24[ 2 ] = -silk_LSHIFT( rc, 8 );

	rc = rc_Q16[3];
    tmp1 = A_Q24[0];
    tmp2 = A_Q24[2];
    c1 = (int)(int64_t)(AE_MUL32_LL(tmp2, rc) >> 16);
    c2 = (int)(int64_t)(AE_MUL32_LL(tmp1, rc) >> 16);
	A_Q24[ 0 ] = tmp1 + c1;
	A_Q24[ 2 ] = tmp2 + c2;
	tmp1 = A_Q24[1];
	c1 = (int)(int64_t)(AE_MUL32_LL(tmp1, rc) >> 16);
	A_Q24[ 1 ] = tmp1 + c1;
	A_Q24[ 3 ] = -silk_LSHIFT( rc, 8 );

	rc = rc_Q16[4];
	for(n = 0; n < 2; n++)
	{
		tmp1 = A_Q24[ n ];
		tmp2 = A_Q24[ 3 - n ];
	    c1 = (int)(int64_t)(AE_MUL32_LL(tmp2, rc) >> 16);
	    c2 = (int)(int64_t)(AE_MUL32_LL(tmp1, rc) >> 16);
		A_Q24[ n ]     = tmp1 + c1;
		A_Q24[ 3 - n ] = tmp2 + c2;
	}
	A_Q24[ 4 ] = -silk_LSHIFT( rc, 8 );

	rc = rc_Q16[5];
	for(n = 0; n < 2; n++)
	{
		tmp1 = A_Q24[ n ];
		tmp2 = A_Q24[ 4 - n ];
	    c1 = (int)(int64_t)(AE_MUL32_LL(tmp2, rc) >> 16);
	    c2 = (int)(int64_t)(AE_MUL32_LL(tmp1, rc) >> 16);
		A_Q24[ n ]     = tmp1 + c1;
		A_Q24[ 4 - n ] = tmp2 + c2;
	}
	tmp1 = A_Q24[2];
	c1 = (int)(int64_t)(AE_MUL32_LL(tmp1, rc) >> 16);
	A_Q24[ 2 ] = tmp1 + c1;
	A_Q24[ 5 ] = -silk_LSHIFT( rc, 8 );

	rc = rc_Q16[6];
	for(n = 0; n < 3; n++)
	{
		tmp1 = A_Q24[ n ];
		tmp2 = A_Q24[ 5 - n ];
	    c1 = (int)(int64_t)(AE_MUL32_LL(tmp2, rc) >> 16);
	    c2 = (int)(int64_t)(AE_MUL32_LL(tmp1, rc) >> 16);
		A_Q24[ n ]     = tmp1 + c1;
		A_Q24[ 5 - n ] = tmp2 + c2;
	}
	A_Q24[ 6 ] = -silk_LSHIFT( rc, 8 );

	rc = rc_Q16[7];
	for(n = 0; n < 3; n++)
	{
		tmp1 = A_Q24[ n ];
		tmp2 = A_Q24[ 6 - n ];
	    c1 = (int)(int64_t)(AE_MUL32_LL(tmp2, rc) >> 16);
	    c2 = (int)(int64_t)(AE_MUL32_LL(tmp1, rc) >> 16);
		A_Q24[ n ]     = tmp1 + c1;
		A_Q24[ 6 - n ] = tmp2 + c2;
	}
	tmp1 = A_Q24[3];
	c1 = (int)(int64_t)(AE_MUL32_LL(tmp1, rc) >> 16);
	A_Q24[ 3 ] = tmp1 + c1;
	A_Q24[ 7 ] = -silk_LSHIFT( rc, 8 );

	rc = rc_Q16[8];
	for(n = 0; n < 4; n++)
	{
		tmp1 = A_Q24[ n ];
		tmp2 = A_Q24[ 7 - n ];
	    c1 = (int)(int64_t)(AE_MUL32_LL(tmp2, rc) >> 16);
	    c2 = (int)(int64_t)(AE_MUL32_LL(tmp1, rc) >> 16);
		A_Q24[ n ]     = tmp1 + c1;
		A_Q24[ 7 - n ] = tmp2 + c2;
	}
	A_Q24[ 8 ] = -silk_LSHIFT( rc, 8 );

	rc = rc_Q16[9];
	for(n = 0; n < 4; n++)
	{
		tmp1 = A_Q24[ n ];
		tmp2 = A_Q24[ 8 - n ];
	    c1 = (int)(int64_t)(AE_MUL32_LL(tmp2, rc) >> 16);
	    c2 = (int)(int64_t)(AE_MUL32_LL(tmp1, rc) >> 16);
		A_Q24[ n ]     = tmp1 + c1;
		A_Q24[ 8 - n ] = tmp2 + c2;
	}
	tmp1 = A_Q24[4];
	c1 = (int)(int64_t)(AE_MUL32_LL(tmp1, rc) >> 16);
	A_Q24[ 4 ] = tmp1 + c1;
	A_Q24[ 9 ] = -silk_LSHIFT( rc, 8 );

	rc = rc_Q16[10];
	for(n = 0; n < 5; n++)
	{
		tmp1 = A_Q24[ n ];
		tmp2 = A_Q24[ 9 - n ];
	    c1 = (int)(int64_t)(AE_MUL32_LL(tmp2, rc) >> 16);
	    c2 = (int)(int64_t)(AE_MUL32_LL(tmp1, rc) >> 16);
		A_Q24[ n ]     = tmp1 + c1;
		A_Q24[ 9 - n ] = tmp2 + c2;
	}
	A_Q24[ 10 ] = -silk_LSHIFT( rc, 8 );

	rc = rc_Q16[11];
	for(n = 0; n < 5; n++)
	{
		tmp1 = A_Q24[ n ];
		tmp2 = A_Q24[ 10 - n ];
	    c1 = (int)(int64_t)(AE_MUL32_LL(tmp2, rc) >> 16);
	    c2 = (int)(int64_t)(AE_MUL32_LL(tmp1, rc) >> 16);
		A_Q24[ n ]     = tmp1 + c1;
		A_Q24[ 10 - n ] = tmp2 + c2;
	}
	tmp1 = A_Q24[5];
	c1 = (int)(int64_t)(AE_MUL32_LL(tmp1, rc) >> 16);
	A_Q24[ 5 ] = tmp1 + c1;
	A_Q24[ 11 ] = -silk_LSHIFT( rc, 8 );

    for( k = 12; k < order; k++ ) {
        rc = rc_Q16[ k ];
        for( n = 0; n < (k + 1) >> 1; n++ ) {
            tmp1 = A_Q24[ n ];
            tmp2 = A_Q24[ k - n - 1 ];
            c1 = (int)(int64_t)(AE_MUL32_LL(tmp2, rc) >> 16);
            c2 = (int)(int64_t)(AE_MUL32_LL(tmp1, rc) >> 16);
            A_Q24[ n ]     = tmp1 + c1;
            A_Q24[ 10 - n ] = tmp2 + c2;
        }
        A_Q24[ k ] = -silk_LSHIFT( rc, 8 );
    }
} 
#endif