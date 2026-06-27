#pragma once

#ifdef __arm__
#include "../cpu/m33/smf_arm_dsp_m33.h"
#elif defined(__hifi4__)
#include "../cpu/hifi4/smf_arm_dsp_hifi4.h"
#else
#include "../cpu/other/smf_arm_dsp_c.h"
#endif
