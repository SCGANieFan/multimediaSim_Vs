#include "random.h"
uint32_t Random(uint32_t rangeMin, uint32_t rangeMax) {
	static uint32_t x = 1;
	static uint32_t y = 362436069;
	static uint32_t z = 521288629;
	static uint32_t w = 88675123;
	rangeMin = rangeMin > rangeMax ? rangeMax : rangeMin;
	uint32_t t = x ^ (x << 11);
	x = y; y = z; z = w;
	w = (w ^ (w >> 19)) ^ (t ^ (t >> 8));
	uint32_t random_num = w;
	uint32_t range = rangeMax - rangeMin + 1;
	uint32_t random_out = random_num % range;// [0,rangeMax - rangeMin]
	return random_out + rangeMin;// [rangeMin,rangeMax]
}
