#ifndef OPUS_CONFIG_H
#define OPUS_CONFIG_H

#define FIXED_POINT           1
#define OPUS_BUILD            1

#define NONTHREADSAFE_PSEUDOSTACK
#define DISABLE_FLOAT_API

// #if defined(__hifi4__)||defined(__hifi5s)
// #define HIFI_OPT
// #endif

#ifdef HIFI_OPT
#include <xtensa/tie/xt_misc.h>
#include <xtensa/tie/xt_mul.h>
#include <xtensa/tie/xt_hifi2.h>
//#define ORIGINAL_PRECISION
#endif

//#include "SmfStatistics.h"

#ifdef WIN32
#define OPUS_OPEN_ENC 1
#define OPUS_OPEN_DEC 1
#define OPUS_ENC_SILK_ONLY 0
#define OPUS_ENC_CELT_ONLY 0
#define OPUS_DEC_SILK_ONLY 0
#define OPUS_DEC_CELT_ONLY 0
#endif

#if OPUS_OPEN_ENC
#if OPUS_ENC_SILK_ONLY & (!OPUS_ENC_CELT_ONLY)
#define OPUS_ENC_SILK 1
#elif (!OPUS_ENC_SILK_ONLY ) & OPUS_ENC_CELT_ONLY
#define OPUS_ENC_CELT 1
#endif
#endif

#if OPUS_OPEN_DEC
#if OPUS_DEC_SILK_ONLY & (!OPUS_DEC_CELT_ONLY)
#define OPUS_DEC_SILK 1
#elif (!OPUS_DEC_SILK_ONLY ) & OPUS_DEC_CELT_ONLY
#define OPUS_DEC_CELT 1
#endif
#endif


#if (OPUS_OPEN_ENC)&&(!OPUS_ENC_SILK)
#define COMPILE_CELT_ENC 1
#endif

#if (OPUS_OPEN_DEC)&&(!OPUS_DEC_SILK)
#define COMPILE_CELT_DEC 1
#endif

#endif /* OPUS_CONFIG_H */

