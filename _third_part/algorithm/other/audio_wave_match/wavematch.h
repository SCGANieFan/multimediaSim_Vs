#ifndef __WAVE_MATCH_H__
#define __WAVE_MATCH_H__

#include <stdint.h>

typedef struct {
	int32_t best_index; //sample
	int32_t deviation;
}do_wave_format_match_ret;


do_wave_format_match_ret DoWaveFormMatchAllCh16(int16_t* ref, int16_t* cmp, int16_t channels, int32_t seekSample, int32_t matchSample);
do_wave_format_match_ret DoWaveFormMatchAllCh32(int32_t* ref, int32_t* cmp, int16_t channels, int32_t seekSample, int32_t matchSample);

#endif