#pragma once
#include <stdint.h>
namespace smf {
	class TimerCast {
	public:
		void Reset() { _delay = 0; }
		uint32_t toMs(uint32_t timepoint_ms);
		uint32_t toUs(uint32_t timepoint_us);
		int sleep_until(uint32_t idx, uint32_t timepoint_ms);
		bool set(uint32_t keys, void* vals);
	protected:
		int64_t _delay = 0;
		int16_t _sleep_max = 0;
		int16_t _dead_line = 5;
		int16_t _reset = 100;
	};
}