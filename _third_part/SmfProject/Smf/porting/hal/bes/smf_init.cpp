#include "SmfPorting.h"
#include "smf_api.h"
#include "hal_trace.h"

namespace smf {
	typedef void (*CbOutput)(const char* str, int size);
	WEAK bool smf_initialize(bool reinit) {
		smf_log_redirect1((CbOutput)&hal_trace_output);
		return true;
	}
}


