#include "smf_api.h"
#include "smf_debug.h"
#include "SmfHash.h"
#include "IMsvc.h"

#ifdef MM_PLATFORM_HIFI4
#include "app_rpc.h"
#else
#include "app_rpc_api.h"
#endif
#include "rpc_m55_dsp.h"

using namespace smf; 
using namespace smf::api; 
namespace smf {
	typedef bool(*CbMsg)(smf_msg_t*);
	typedef uint32_t(*ipc_rx_cb_t)(const void* data, uint32_t len, void* priv);
	IMsvc* svc = 0;

	static void smf_cmd_transmit_handler(uint8_t* ptr, uint16_t len)
	{
		// dbgNotePPL(ptr);
		// dbgNotePDL(len);
		app_rpc_send_data_no_rsp(APP_RPC_CORE_DSP_M55, M55_DSP_TASK_CMD_SMF_NO_RSP, ptr, len);
	}

	static void smf_cmd_received_handler(uint8_t* ptr, uint16_t len)
	{
		// dbgNotePPL(ptr);
		// dbgNotePDL(len);
		returnIfErrC(, !svc);
		svc->Receive((smf_msg_t*)ptr);
	}


	bool core_bridge_send(smf_msg_t* msg)
	{
		bool rst = true;
		// dbgNotePL();
		app_rpc_send_cmd(APP_RPC_CORE_DSP_M55, M55_DSP_TASK_CMD_SMF_NO_RSP, (uint8_t*)msg, msg->size);
		return rst;
	}

	RPC_M55_DSP_TASK_CMD_TO_ADD(M55_DSP_TASK_CMD_SMF_NO_RSP,
		"M55_DSP_TASK_CMD_SMF_NO_RSP",
		smf_cmd_transmit_handler,
		smf_cmd_received_handler,
		0,
		NULL,
		NULL,
		NULL);


	bool smf_message_init(bool master) {
		returnIfErrC(false, !smf_gets("msvc/imsvc", &svc));
		returnIfErrC(false, !svc);

		svc->Register([](smf_msg_t* msg)->bool {
			if (msg->cpusrc == msg->cpudst) {
				// local msg
				svc->Receive((smf_msg_t*)msg);
				return true;
			}
			int ret = core_bridge_send(msg);
			returnIfErrCS(false, ret < 0, "core_bridge_send failed!!! , ret = %d", ret);
			return true;
		});
		return true;
	}

	bool smf_ipc_open(uint8_t cpuid) {
		return true;
	}

	bool smf_ipc_close(uint8_t cpuid) {
		return true;
	}
}
