#include <stdint.h>
static int32_t AgcSmooth(int16_t* input, int16_t* output, int16_t sample, int16_t channels, int32_t gainQ8, int32_t prev_gainQ8) {
	const int32_t MAX_SHORT_Q8 = 32767 * ((int32_t)1 << 8);
	const int32_t MIN_SHORT_Q8 = -32768 * ((int32_t)1 << 8);
	const int32_t GAIN_SMOOTHING_ALPHA_Q8 = ((int32_t)1 << 8) / 10;
	int16_t max_abs_val = 0;
	if (channels != 1) return 0;
	for (int s = 0; s < sample; s++) {
		int16_t abs_val = input[s];
		abs_val = abs_val > 0 ? abs_val : -abs_val;
		max_abs_val = max_abs_val < abs_val ? abs_val : max_abs_val;
	}
	int32_t gain_max_possible_Q8 = gainQ8;
	if (max_abs_val > 0) gain_max_possible_Q8 = MAX_SHORT_Q8 / max_abs_val;
	int32_t raw_gain_Q8 = gainQ8 < gain_max_possible_Q8 ? gainQ8 : gain_max_possible_Q8;
	int32_t gain_step_Q8 = raw_gain_Q8 - prev_gainQ8;
#if 0
	int32_t smoothed_gain_Q8 = raw_gain_Q8;
	if (gain_step_Q8 > 0)
		smoothed_gain_Q8 = prev_gainQ8 + (GAIN_SMOOTHING_ALPHA_Q8 * gain_step_Q8 >> 8);
#else
	int32_t smoothed_gain_Q8 = prev_gainQ8 + (GAIN_SMOOTHING_ALPHA_Q8 * gain_step_Q8 >> 8);
#endif
	for (int s = 0; s < sample; s++) {
		int32_t val = (int32_t)input[s];
		val = val * smoothed_gain_Q8;
		val = val > MAX_SHORT_Q8?MAX_SHORT_Q8:val;
		val = val < MIN_SHORT_Q8?MIN_SHORT_Q8:val;
		output[s] = (int16_t)(val >> 8);
	}
	prev_gainQ8 = smoothed_gain_Q8;
	return prev_gainQ8;
}
