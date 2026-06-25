#include "TimerCast.h"
#include "SmfPorting.h"
#include "SmfHash.h"
#include "smf_debug.h"
using namespace smf;

uint32_t TimerCast::toMs(uint32_t ms) {
	if (!_delay) {
		_delay = get64_ms() - ms;
	}
	return ms + _delay;
}
uint32_t TimerCast::toUs(uint32_t us) {
	if (!_delay) {
		_delay = get64_us() - us;
	}
	return us + _delay;
}

int TimerCast::sleep_until(uint32_t idx, uint32_t timepoint) {
	int16_t sleep_max = _sleep_max;
	int16_t dead_line = _dead_line;
	int16_t reset = _reset;
	auto target = toMs(timepoint);
	auto curr = get64_ms();
	int diff = target - curr;
	//dbgTestPXL("%d,%u/%u,%u,%d", (int32_t)_delay, (uint32_t)curr, (uint32_t)target, (uint32_t)timepoint, (int32_t)diff);
	if (diff > reset || diff<-reset) {//reset _delay.
		dbgWarnPXL("[%u][reset]%d,%u/%u,%u,%d>%d", idx, (int32_t)_delay, (uint32_t)curr, (uint32_t)target, (uint32_t)timepoint, (int32_t)diff, sleep_max);
		_delay = 0;
		target = toMs(timepoint);
		diff = target - curr;
	}
	if (sleep_max && (diff > sleep_max)) {
		dbgWarnPXL("[%u]%d,%u/%u,%u,%d>%d", idx, (int32_t)_delay, (uint32_t)curr, (uint32_t)target, (uint32_t)timepoint, (int32_t)diff, sleep_max);
		sleep_for(sleep_max);
		return diff - sleep_max;
	}
	else if (diff < -dead_line) {
		dbgWarnPXL("[%u]%d,%u/%u,%u,%d,%d", idx, (int32_t)_delay, (uint32_t)curr, (uint32_t)target, (uint32_t)timepoint, (int32_t)diff, dead_line);
		return diff;
	}
	else if(diff > dead_line){
		sleep_for(diff - dead_line);
		return dead_line;
	}
	else {
		return diff;
	}
}

bool TimerCast::set(uint32_t keys, void* vals) {
	switch (keys) {
	case Hash("sleepMax"):_sleep_max = (int)vals; return true;
	case Hash("deadline"):_dead_line = (int)vals; return true;
	case Hash("reset"):_reset = (int)vals; return true;
	default:return false;
	}
}