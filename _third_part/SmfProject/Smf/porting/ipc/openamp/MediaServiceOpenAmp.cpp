#include <stdio.h>
#ifdef KERNEL_NUTTX
#include <nuttx/rpmsg/rpmsg.h>
#else
#include <open-amp/rptun.h>
#endif
#include <openamp/rpmsg.h>
#include "IMsvc.h"
#include "smf_api.h"
#include "smf_debug.h"
#include "SmfPorting.h"

// #ifdef __cplusplus
// extern "C" {
// #endif
// #include <rpchan/bes_rpchan.h>
// #ifdef __cplusplus
// }
// #endif

using namespace smf; 
using namespace smf::api; 

#define SMF_RPMSG_EPT_NAME "smf_ept"
#ifdef KERNEL_NUTTX
EXTERNC void *kmm_malloc(size_t size);
EXTERNC void *kmm_zalloc(size_t size);
EXTERNC void kmm_free(void *p);
#else
#include "ada/ada_kmm_pool.cpp"
#endif
namespace smf {
	static bool _init = false;
	static int ept_cb(struct rpmsg_endpoint* ept, void* data, size_t len, uint32_t src, void* priv) {
		auto svc = (IMsvc*)ept->priv;
		svc->Receive((smf_msg_t*)data);
		return 0;
	}

	static void ept_unbind(struct rpmsg_endpoint* ept) {
		rpmsg_destroy_ept(ept);
		kmm_free(ept);
	}

	static void dev_create_ept(struct rpmsg_device* rdev, const char* name, uint32_t dest, IMsvc* svc) {
		auto ept = (rpmsg_endpoint*)kmm_zalloc(sizeof(rpmsg_endpoint));
		// dbgTestPDL(sizeof(rpmsg_endpoint));
		returnIfErrC(, !ept);
		svc->Register([svc](smf_msg_t* msg) {
			auto ept = (rpmsg_endpoint*)svc->Tags();
			if (msg->cpusrc == msg->cpudst) {
				// local msg
				svc->Receive((smf_msg_t*)msg);
				return true;
			}
			if (!ept) {
				while (!is_rpmsg_ept_ready(ept)) {
					sleep_for(1);
					ept = (rpmsg_endpoint*)svc->Tags();
				}
			}
			int ret = rpmsg_send(ept, msg, msg->size);
			returnIfErrCS(false, ret < 0, "rpmsg_send failed!!! , ret = %d", ret);
			return true;
			});
		ept->priv = svc;
		int ret = rpmsg_create_ept(ept, rdev, SMF_RPMSG_EPT_NAME, RPMSG_ADDR_ANY, dest, ept_cb, ept_unbind);
		if (ret) {
			dbgTestPXL("create ept failed!! name = %s ret = %d", name, ret);
			kmm_free(ept);
		}
		else {
			svc->Tags(ept);
			dbgTestPXL("ept create suceess, name = %s", name);
		}
	}

	static bool dev_match(struct rpmsg_device* rdev, void* priv, const char* name, uint32_t dest) {
		dbgTestPSL(name);
		return !strcmp(name, SMF_RPMSG_EPT_NAME);
	}

	static void dev_bind(struct rpmsg_device* rdev, void* priv, const char* name, uint32_t dest) {
		dbgTestPSL(name);
		dev_create_ept(rdev, name, dest, (IMsvc*)priv);
	}

	static void dev_created(struct rpmsg_device* rdev, void* priv) {
		dbgTestPL();
		dev_create_ept(rdev, SMF_RPMSG_EPT_NAME, RPMSG_ADDR_ANY, (IMsvc*)priv);
	}

	static void dev_destroy(struct rpmsg_device *rdev, void *priv) {
		dbgTestPL();
		auto ept = (rpmsg_endpoint*)((IMsvc*)priv)->Tags();
		if(!ept)return;
		auto svc = (IMsvc*)ept->priv;
		if(!svc)return;
		svc->Tags(0);
		rpmsg_destroy_ept(ept);
		kmm_free(ept);
	}
	bool smf_message_master_init() {
		IMsvc* svc = 0;
		returnIfErrC(false, !smf_get("msvc/imsvc", &svc));
		returnIfErrC(false, !svc);
		if(_init)return true;
		returnIfErrC(false, rpmsg_register_callback(svc, NULL, dev_destroy, dev_match, dev_bind));
		_init = true;
		return true;
	}
	bool smf_message_master_deinit() {
		IMsvc* svc = 0;
		returnIfErrC(false, !smf_get("msvc/imsvc", &svc));
		returnIfErrC(false, !svc);
		rpmsg_unregister_callback(svc, NULL, dev_destroy, dev_match, dev_bind);
		// dev_destroy(NULL, svc);
		return true;
	}

	bool smf_message_init(bool master) {
		IMsvc* svc = 0;
		returnIfErrC(false, !smf_get("msvc/imsvc", &svc));
		returnIfErrC(false, !svc);
		if(master)	{
			// returnIfErrC(false, rpmsg_register_callback(svc, NULL, NULL, dev_match, dev_bind));
			// bes_rpchan_register_callback(HAL_RMT_IPC_CORE_SYS_HIFIC0, BES_RPCHAN_CB_LV4, BES_RPCHAN_START_MASK, (void*)&smf_message_master_init);
			smf_message_master_init();
			// bes_rpchan_register_callback(HAL_RMT_IPC_CORE_SYS_HIFIC0, BES_RPCHAN_CB_LV4, BES_RPCHAN_CLOSE_MASK, (void*)&smf_message_master_deinit);
		}
		else {
			int res = rpmsg_register_callback(svc,dev_created,dev_destroy,0,0);
			// dbgTestPDL(res);.
			returnIfErrC(false, res != 0);
		}
		return true;
	}
	bool smf_ipc_open(uint8_t cpuid) {
		return true;
	}

	bool smf_ipc_close(uint8_t cpuid) {
		return true;
	}
}