#include <stdint.h>
#include "smf_debug.h"
#include "SmfPorting.h"
#include "hal_sysfreq.h"
#include "hal_cmu.h"

namespace smf {
	bool set_cpu_freq(uint32_t hz) {
		auto user = HAL_SYSFREQ_USER_DSP;
		//auto last = hal_sysfreq_get_user_freq(user);
		auto freq = HAL_CMU_FREQ_208M;
		auto diff = 10000000;
		switch (hz) {
		case CPU_FREQ_MAX:freq = HAL_CMU_FREQ_208M; hz = 192000000; break;
		case CPU_FREQ_MIN:freq = HAL_CMU_FREQ_32K; hz = 32000; break;
		default:
			if (hz >= 96000000 - diff)freq = HAL_CMU_FREQ_208M;
			else if(hz > 0)freq = HAL_CMU_FREQ_104M;
			else freq = HAL_CMU_FREQ_32K;
			break;
		}
		static uint8_t _idx = 0;
		static uint8_t last = 0;
		//dbgTestPXL("[%u]%u,%u<-%u", _idx, hz, freq, last);
		if (freq == last) {
			_idx = 0;
			return true;
		}
		else if (freq > last) {
			dbgNotePXL("[%u]%ukhz,%u<-%u", _idx, hz/1000, freq, last);
			_idx = 0;
			hal_sysfreq_req(user, freq);
			last = freq;
		}
		else {
			if (_idx > 2) {
				dbgNotePXL("[%u]%ukhz,%u<-%u", _idx, hz/1000, freq, last);
				_idx = 0;
				hal_sysfreq_req(user, freq);
				last = freq;
			}
			else {
				_idx++;
			}
		}
		return true;
	}
}
