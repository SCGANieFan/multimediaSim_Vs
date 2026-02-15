#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include "wavematch.h"



void test16() {
	//generate data
	int16_t cmp16[4] = { -1,0,1,0 };
	int16_t ref16[] = { 
		-1,1,0,-1,
		-1,0,-1,1,
		-1,0,1,2,
		-1,0,1,0,
		-1,1,0,-1,
	};
	const int16_t channels = 1;
	int32_t cmp16_sample = sizeof(cmp16) / (sizeof(int16_t) * channels);
	int32_t ref16_sample = sizeof(ref16) / (sizeof(int16_t) * channels);
	int32_t seek_sample = ref16_sample - cmp16_sample;
	int32_t match_sample = cmp16_sample;
	do_wave_format_match_ret ret = DoWaveFormMatchAllCh16(&ref16[0], &cmp16[0], channels, seek_sample, match_sample);
	printf("index:%d, deviation:%d\n", ret.best_index, ret.deviation);
}
void test32() {
	//generate data
	int32_t cmp32[4] = { -1,0,1,0 };
	int32_t ref32[] = {
		-1,1,0,-1,
		-1,0,-1,1,
		-1,0,1,0,
		-1,0,1,2,
		-1,1,0,-1,
	};
	const int16_t channels = 1;
	int32_t cmp32_sample = sizeof(cmp32) / (sizeof(int32_t) * channels);
	int32_t ref32_sample = sizeof(ref32) / (sizeof(int32_t) * channels);
	int32_t seek_sample = ref32_sample - cmp32_sample;
	int32_t match_sample = cmp32_sample;
	do_wave_format_match_ret ret = DoWaveFormMatchAllCh32(&ref32[0], &cmp32[0], channels, seek_sample, match_sample);
	printf("index:%d, deviation:%d\n", ret.best_index, ret.deviation);
}

int main() {
	test16();
	test32();
	return 0;
}
