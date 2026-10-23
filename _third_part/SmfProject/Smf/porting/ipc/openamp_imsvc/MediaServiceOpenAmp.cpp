#include <stdio.h>
#include <open-amp/rptun.h>
#include "cmsis_os.h"
#include "smf_api.h"
#include "IMsvc.h"
#include "smf_debug.h"
#include "SmfPorting.h"
#include "smf_api_ics.h"
#ifdef USE_BES_RPCHAN
#include "open-amp/rpchan/bes_rpchan.h"
#endif

using namespace smf;
using namespace smf::api;

static struct rpmsg_endpoint *_smf_cpu_ept_map[SMF_CPU_ID_END];

typedef bool (*rpmsg_boot_cb_t)(uint8_t cpuid);
static rpmsg_boot_cb_t _connect_cb;
static rpmsg_boot_cb_t _disconnect_cb;

#ifdef __NuttX__
void *kmm_malloc(size_t size);
void *kmm_zalloc(size_t size);
void kmm_free(void *p);
#else // __NuttX__
static inline void *kmm_malloc(size_t size) {
	return metal_allocate_memory(size);
}
static inline void *kmm_zalloc(size_t size) {
	auto *p = kmm_malloc(size);
	if (p == nullptr) {
		return p;
	}
	memset(p, 0, size);
	return p;
}
static inline void kmm_free(void *p) {
	metal_free_memory(p);
}
#endif // __NuttX__

struct smf_ics_t;
namespace smf {
	smf_ics_t* ICSBlock();
	void ICSBlock(void*);
};

typedef struct {
	union {
		uint8_t cmd;
		uint8_t para[5];
	};
}internal_extra_msg_t;

typedef enum {
	INTERNAL_UPDATE_ICS = 0,
	INTERNAL_CONNECT_CORE = 1,
	INTERNAL_DISCONNECT_CORE = 2
}inter_e;

static inline bool is_slave(int cpuid) {
	return get_cpu_idx() > cpuid;
}

static inline uint8_t get_cpuid_from_ept_name(const char* ept_name) {
	//dbgDevPXL("%s", ept_name);
	auto src = ept_name[3] - '0';
	auto dst = ept_name[4] - '0';
	auto cpuid = get_cpu_idx();
	if (!(ept_name[0] == 's'
		&& ept_name[1] == 'm'
		&& ept_name[2] == 'f'
		&& ept_name[5] == '\0'
		&& src > 0 && src < 8
		&& dst > 0 && dst < 8
		&& (src == cpuid || dst == cpuid)
		)) {
		//dbgWarnPXL("[%u]%s", cpuid, ept_name);
		return 0;
	}
	//
	return src == cpuid ? dst : src;
}

static inline bool check_ept_name(const char* ept_name) {
	return get_cpuid_from_ept_name(ept_name);
}

static inline bool check_ept_name(const char* ept_name, int cpuid) {
	return cpuid && cpuid == get_cpuid_from_ept_name(ept_name);
}

static inline uint64_t create_ept_name(uint8_t cpuid) {
	auto src = get_cpu_idx();
	smf_int8x8_t x;
	x.u8[0] = 's';
	x.u8[1] = 'm';
	x.u8[2] = 'f';
	x.u8[3] = (src < cpuid ? src : cpuid) + '0';
	x.u8[4] = (src > cpuid ? src : cpuid) + '0';
	x.u8[5] = 0;
	return x.u64;
}

static inline bool internal_extra_data_recv(IMsvc* scv, void *data, size_t len) {
	returnIfErrC(false, len < sizeof(internal_extra_msg_t));
	auto msg = (internal_extra_msg_t *)data;
	uint32_t para;
	memcpy(&para, &msg->para[1], 4);
	auto cmd = msg->cmd;
	switch (cmd) {
	case INTERNAL_UPDATE_ICS: {
		auto remote_ics = (void *)para;
		returnIfErrC(false, !remote_ics);
		auto local_ics = ICSBlock();
		if (local_ics) {
			returnIfErrCS(false, local_ics != remote_ics, "%p,%p", local_ics, remote_ics);
			return true;
		}
		ICSBlock(remote_ics);
		smf_ics_ready();
		dbgNotePXL("Update ICS: %p", remote_ics);
		break;
	}
	case INTERNAL_CONNECT_CORE: {
		dbgNotePXL("Connect to core : %u", para);
		returnIfErrC(false, !_connect_cb);
		returnIfErrC(false, !_connect_cb(para));
		break;
	}
	case INTERNAL_DISCONNECT_CORE: {
		dbgNotePXL("Disconnect to core : %u", para);
		returnIfErrC(false, !_disconnect_cb);
		returnIfErrC(false, !_disconnect_cb(para));
		break;
	}
	default: dbgErrPXL("unknow cmd:%u", cmd); break;
	}
	return true;
}

static inline bool internal_extra_data_send(rpmsg_endpoint* ept, uint8_t cmd, uint32_t data) {
	returnIfErrC(false, !ept);
	internal_extra_msg_t msg;
	msg.cmd = cmd;
	memcpy(&msg.para[1], &data, 4);
	returnIfErrC(false, !rpmsg_send(ept, &msg, sizeof(msg)));
	return true;
}

static inline bool let_other_core_connect(rpmsg_endpoint* ept, uint8_t cpuid) {
	returnIfErrC(false, !cpuid);
	if (_connect_cb) {
		_connect_cb(cpuid);
	}
	auto ics = ICSBlock();
	returnIfErrC(false, !ics);
	internal_extra_data_send(ept, INTERNAL_UPDATE_ICS, (uint32_t)ics);
	dbgNotePXL("Send ICS(%p) to %s", ics, ept->name);

	auto src = get_cpu_idx();
	auto dst = cpuid;// get_cpuid_from_ept_name(ept->name);

	/* Send internal msg to other core to create connect */
	for(int idx = 1; idx < SMF_CPU_ID_END; idx++) {
		auto cur = _smf_cpu_ept_map[idx];
		if (cur && (idx != src) && (idx != dst)) {
			dbgDevPXL("Let [%u] connect to [%u]", idx, dst);
			internal_extra_data_send(cur, INTERNAL_CONNECT_CORE, (uint32_t)dst);
			dbgDevPXL("Let [%u] connect to [%u]", dst, idx);
			internal_extra_data_send(ept, INTERNAL_CONNECT_CORE, (uint32_t)idx);
		}
	}
	return true;
}

static inline bool let_other_core_disconnect(rpmsg_endpoint* ept, uint8_t cpuid) {
	if (_disconnect_cb) {
		_disconnect_cb(cpuid);
	}
	//
	auto src = get_cpu_idx();
	auto dst = cpuid;// get_cpuid_from_ept_name(ept->name);

	/* Send internal msg to other core to create connect */
	for (int idx = 1; idx < SMF_CPU_ID_END; idx++) {
		auto cur = _smf_cpu_ept_map[idx];
		if (cur && (idx != src) && (idx != dst)) {
			dbgDevPXL("Let [%u] disconnect to [%u]", idx, dst);
			internal_extra_data_send(cur, INTERNAL_DISCONNECT_CORE, (uint32_t)dst);
			dbgDevPXL("Let [%u] disconnect to [%u]", dst, idx);
			internal_extra_data_send(ept, INTERNAL_DISCONNECT_CORE, (uint32_t)idx);
		}
	}
	return true;
}

static int ept_rx_cb(struct rpmsg_endpoint *ept,void *data, size_t len, uint32_t src,void *priv) {
	auto scv = (IMsvc*)ept->priv;
	if (len < sizeof(smf_msg_t)) {
		internal_extra_data_recv(scv, data, len);
		return 0;
	}

	scv->Receive((smf_msg_t*)data);
	return 0;
}

static void ept_unbind(struct rpmsg_endpoint* ept) {
	rpmsg_destroy_ept(ept);
	kmm_free(ept);
}

static rpmsg_endpoint* create_ept(struct rpmsg_device* rdev,
                                  const char* ept_name,
                                  uint32_t dest,
								  IMsvc* scv) {
	/* Check */
	auto cpuid = get_cpuid_from_ept_name(ept_name);
	if (cpuid == 0) {
		dbgDevPXL("no need create ept on [%s]", ept_name);
		return 0;
	}

	/* Create */
	dbgDevPXL("%s", ept_name);
	auto ept = (struct rpmsg_endpoint*)kmm_zalloc(sizeof(struct rpmsg_endpoint));
	returnIfErrC(0, !ept);
	ept->priv = scv;
	int ret = rpmsg_create_ept(ept, rdev, ept_name,
	            RPMSG_ADDR_ANY, dest, ept_rx_cb, ept_unbind);
	if (ret) {
		dbgErrPXL("[%s]create failed! ret=%d", ept_name, ret);
		kmm_free(ept);
		return 0;
	}

	/* Save endpoint handle in map */
	_smf_cpu_ept_map[cpuid] = ept;

	/* Register MSVC send callback */
	//scv->Register([](smf_msg_t* msg) {
	//	struct rpmsg_endpoint * eptt = 0;
	//	eptt = _smf_cpu_ept_map[msg->cpudst];
	//	if (!eptt) {
	//		eptt = _smf_cpu_ept_map[SMF_CPU_AP];
	//	}
	//	returnIfErrCS(false, !eptt, "msg->cpudst = %d", msg->cpudst);
	//	rpmsg_send(eptt, msg, msg->size);
	//	return true;
	//});

	return ept;
}

static bool master_match(struct rpmsg_device* rdev,void* priv, const char* ept_name,uint32_t dest) {
	return check_ept_name(ept_name);
}

static void master_bind(struct rpmsg_device* rdev,void* priv, const char* ept_name,uint32_t dest) {
	auto scv = (IMsvc*)priv;
	auto ept = create_ept(rdev, ept_name, dest, scv);
	returnIfErrCS0(!ept, "%s", ept_name);
	//dbgDevPXL("%s,%u", ept_name, scv->IsMaster());
	auto cpuid = get_cpuid_from_ept_name(ept_name);
	if (scv->IsMaster()) {
		let_other_core_connect(ept, cpuid);
	}
}

static void slave_create(struct rpmsg_device *rdev, void *priv) {
	auto scv = (IMsvc*)priv;
	const char * cpu_name = rpmsg_get_cpuname(rdev);
	auto cpuid = scv->GetIpc(cpu_name);
	//dbgDevPXL("%u:%s", cpuid, cpu_name);
	if (cpuid == 0) {
		//dbgDevPXL("no need create ept on %s", cpu_name);
		return;
	}

	/* Only slave need actively create ept */
	if(is_slave(cpuid)) {
		auto name64 = create_ept_name(cpuid);
		auto ept_name = (char*)&name64;
		//dbgDevPXL("%s", ept_name);
		auto ret = create_ept(rdev, ept_name, RPMSG_ADDR_ANY, (IMsvc*)priv);
		returnIfErrCS0(!ret, "%s", ept_name);
		//dbgDevPXL("%u:%s ept create success", cpuid, ept_name);
	}
}

static void slave_destroy(struct rpmsg_device *rdev, void *priv) {
	auto scv = (IMsvc*)priv;
	const char * cpu_name = rpmsg_get_cpuname(rdev);
	auto cpuid = scv->GetIpc(cpu_name);
	//dbgDevPXL("[%u]%s", cpuid, cpu_name);
	if (cpuid == 0) {
		//dbgDevPXL("no ept created on %s", cpu_name);
		return;
	}

	auto& ept = _smf_cpu_ept_map[cpuid];
	if(ept){
		dbgDevPXL("%s", ept->name);
		if (scv->IsMaster()) {
			let_other_core_disconnect(ept, cpuid);
		}
		//dbgDevPXL("%u,%p", cpuid, ept);
		rpmsg_destroy_ept(ept);
		kmm_free(ept);
		ept = 0;
	}
	smf_ics_poweroff(cpuid);
}

EXTERNC void smf_rpmsg_register_boot_cb(rpmsg_boot_cb_t connect_cb, rpmsg_boot_cb_t disconnect_cb) {
	_connect_cb = connect_cb;
	_disconnect_cb = disconnect_cb;
}

static bool ipc_send(smf_msg_t* msg) {
	struct rpmsg_endpoint* eptt = 0;
	eptt = _smf_cpu_ept_map[msg->cpudst];
	if (!eptt) {
		IMsvc* scv = IMsvc::Msvc();
		returnIfErrC(false, !scv);
		if (scv->GetIpc(msg->cpudst)) {
			if (smf_ics_is_poweron(msg->cpudst)) {
				for (int i = 0; i < 100; i++) {
					eptt = _smf_cpu_ept_map[msg->cpudst];
					if (eptt)break;
					else sleep_for(10);
				}
			}
		}
		else {
			eptt = _smf_cpu_ept_map[SMF_CPU_AP];
		}
		//eptt = _smf_cpu_ept_map[SMF_CPU_AP];
	}
	returnIfErrCS(false, !eptt, "cpudst:%u", msg->cpudst);
	rpmsg_send(eptt, msg, msg->size);
	return true;
}

namespace smf {
	bool smf_message_init(bool master) {
		IMsvc* scv = IMsvc::Msvc();
		returnIfErrC(false, !scv);
		scv->Register(&ipc_send);
		rpmsg_register_callback(scv, slave_create, slave_destroy, master_match, master_bind);
		return true;
	}

	bool smf_ipc_open() {
		return true;
	}

	bool smf_ipc_close() {
		return true;
	}
}
