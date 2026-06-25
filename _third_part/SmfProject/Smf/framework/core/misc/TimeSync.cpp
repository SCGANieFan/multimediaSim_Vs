#include "TimeSync.h"
#include "SmfPorting.h"
#include "smf_debug.h"

using namespace smf;

void TimeSync::reset() {
	_ms0 = get_local_ms();
	_fcount = 0;
	_reset = false;
}

uint32_t TimeSync::sync() {
	++_fcount;
	int64_t ms = get_local_ms() - _ms0;
	auto msTarget = _fcount * _scale.num  / _scale.den;
	int32_t dms = msTarget - ms;
	// dbgTestPXL("%d,%d,%d", dms, (int)ms, (int)msTarget);
	auto sms = dms;
	if (dms > 0) {
		if (dms > _threshold) {
			dbgWarnPXL("reset:%d,%u,%u", dms, (uint32_t)msTarget, (uint32_t)ms);
			_reset = true;
			sms = 10;
		}
	}
	else if (dms < 0) {
		if (dms < -_threshold) {
			dbgWarnPXL("reset:%d,%u,%u", dms, (uint32_t)msTarget, (uint32_t)ms);
			_reset = true;
		}
		sms = 0;
	}
	else {
		sms = 0;
	}
	return sms;
}