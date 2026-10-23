#include "IMsvc.h"
#include "smf_debug.h"
#include "smf_api.h"
using namespace smf; 
using namespace smf::api; 
namespace smf {
	bool smf_message_init(bool master) {
		dbgTestPL();
		IMsvc* svc = 0;
		returnIfErrC(false, !smf_gets("msvc/imsvc", &svc));
		returnIfErrC(false, !svc);
		svc->Register([svc](smf_msg_t* msg) {
			return svc->Receive(msg);
			});
		return true;
	}

	bool smf_ipc_open(uint8_t cpuid) {
		return true;
	}

	bool smf_ipc_close(uint8_t cpuid) {
		return true;
	}

	WEAK int dsp_open() {
		return 0;
	}
	WEAK int dsp_close() {
		return 0;
	}
}
