#include "wavematch.h"

static inline int16_t Abs(int16_t x) { return x > 0 ? x : -x; }

do_wave_format_match_ret DoWaveFormMatchAllCh16(int16_t* ref, int16_t* cmp, int16_t channels, int32_t seekSample, int32_t matchSample) {
	int16_t* pRef = ref;
	int16_t* pCmp = cmp;
	int32_t sOpt = 0;
	// s=0
	int32_t scoreOpt = 0;
	const int32_t sample_all = channels * matchSample;
	for(int32_t m = 0; m < sample_all; m+= 1) {
		scoreOpt+=Abs(pRef[m]-pCmp[m]);
	}
	pRef += channels;
	for (int32_t s = 1; s < seekSample; s+= 1) {
		int32_t score = 0;
		for(int32_t m = 0; m < sample_all; m+= 1) {
			score+=Abs(pRef[m]-pCmp[m]);
		}
		if (score <= scoreOpt) {
			scoreOpt = score;
			sOpt = s;
		}
		pRef += channels;
	}
	do_wave_format_match_ret ret;
	ret.best_index = sOpt;
	ret.deviation = scoreOpt / sample_all;
	return ret;
}
