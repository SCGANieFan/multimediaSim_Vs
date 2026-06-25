// #include <stdio.h>
#include <stdint.h>
bool LimitTrigger(int8_t dir, int8_t& cnt, int8_t lower, int8_t upper, int8_t lowerTrigger, int8_t upperTrigger) {
	if (dir != 1 && dir != -1) return false;
	int8_t cnt_old = cnt;
	if (dir == 1) {
		++cnt;
		cnt = cnt < lower ? lower : cnt;
		cnt = cnt > upper ? upper : cnt;
		if (cnt_old < cnt && cnt == upperTrigger) {
			return true;
		}
	}
	else {
		--cnt;
		cnt = cnt < lower ? lower : cnt;
		cnt = cnt > upper ? upper : cnt;
		if (cnt_old > cnt && cnt == lowerTrigger) {
			return true;
		}
	}
	return false;
}

