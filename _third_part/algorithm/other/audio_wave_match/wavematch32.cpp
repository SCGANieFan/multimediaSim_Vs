#include "wavematch.h"

static inline int32_t Abs(int32_t x) { return x > 0 ? x : -x; }

do_wave_format_match_ret DoWaveFormMatchAllCh32(int32_t* ref, int32_t* cmp, int16_t channels, int32_t seekSample, int32_t matchSample) {
	int32_t* pRef = ref;
	int32_t* pCmp = cmp;
	int32_t sOpt = 0;
	// s=0
	int64_t scoreOpt = 0;
	const int32_t sample_all = channels * matchSample;
	for(int32_t m = 0; m < sample_all; m+= 1) {
		scoreOpt+=Abs(pRef[m]-pCmp[m]);
	}
	pRef += channels;
	for (int32_t s = 1; s < seekSample; s+= 1) {
		int64_t score = 0;
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
	ret.deviation = (int32_t )(scoreOpt / sample_all);
	return ret;
}
