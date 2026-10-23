#include "IMsvc.h"
#include "smf_api.h"
#include "smf_debug.h"
#include "SmfPorting.h"
#include "app_ipc_api.h"

using namespace smf; 
using namespace smf::api; 
namespace smf {
	bool smf_message_init(bool master) {
		IMsvc* svc = 0;
		returnIfErrC(false, !smf_gets("msvc/imsvc",&svc));
		returnIfErrC(false, !svc);
		//
		//uint8_t target_cpu = svc->TargetID();
		//
		ipc_rx_cb_t rxcb = [](const void* data, uint32_t len, void* priv) {
			return (uint32_t)((IMsvc*)priv)->Receive((smf_msg_t*)data);
		};
		//returnIfErrC(false, !app_ipc_service_add(target_cpu, IPC_SERVICE_MEDIA_CMD, 0, rxcb, svc));
		for (int i = SMF_CPU_DEFAULT + 1; i < SMF_CPU_ID_END; i++) {
			if (!app_ipc_service_add(i, IPC_SERVICE_MEDIA_CMD, 0, rxcb, svc)) {
				dbgWarnPXL("app_ipc_service_add(%d)", i);
			}
		}
		//
		svc->Register([](smf_msg_t* msg) {
			auto cpuid = msg->cpudst;
			if (!cpuid)cpuid = (get_cpu_idx() == SMF_CPU_DSP) ? SMF_CPU_AP : SMF_CPU_DSP;
			return app_ipc_data_send(cpuid, IPC_SERVICE_MEDIA_CMD, msg, msg->size);
			});
		//
		return true;
	}

	bool smf_ipc_open(uint8_t cpuid) {
#if 0
		static bool ipc_port_first_open = false;
		int rst = 1;
		if (ipc_port_first_open == false) {
			switch (cpuid) {
			case CPU_DSP: rst = app_ipc_m552dsp_port_open(); break;
			}
			if (!rst) {
				smf_message_init(true);
				ipc_port_first_open = true;
			}
		}
		else {
			rst = app_ipc_reopen_cpu(cpuid);
		}
		returnIfErrCS(false, rst, "%d", rst);
		return true;
#else
		auto rst = app_ipc_open_cpu(cpuid);
		returnIfErrCS(false, !rst, "%d", rst);
		return true;
#endif 
	}

	bool smf_ipc_close(uint8_t cpuid) {
		app_ipc_close_cpu(cpuid);
		return true;
	}
}