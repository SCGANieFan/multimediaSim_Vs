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
#include "mutex.h"

using namespace smf;
using namespace smf::api;

#define dbgIpcPXL(s,...) dbgDevPXL(s,##__VA_ARGS__)
static mutex _openamp_mtx;

static struct rpmsg_endpoint *_smf_cpu_ept_map[SMF_CPU_ID_END];
smf_msg_eptname_cpu_t * _eptname_cpu_table;
typedef bool (*rpmsg_boot_cb_t)(const char *name);

static uint32_t _ept_max;
static bool _is_main_core;
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

static const char *get_remote_cpu_name(int smf_cpu_id) {
	for(int idx = 0; idx < _ept_max; idx++) {
		if (_eptname_cpu_table[idx].cpuid == smf_cpu_id) {
			return _eptname_cpu_table[idx].cpu_name;
		}
	}
	return 0;
}

static bool is_slave(int idx) {
	returnIfErrC(false, idx >= _ept_max);
	if(_eptname_cpu_table[idx].master) {
		return false;
	}
	return true;
}

static int get_idx(const char* cpu_name) {
	for(int idx = 0; idx < _ept_max; idx++) {
		auto cpu_name_in_table = _eptname_cpu_table[idx].cpu_name;
		if(!cpu_name_in_table ||  !cpu_name) continue;
		if(!strncmp(cpu_name, cpu_name_in_table, strlen(cpu_name_in_table))) {
			return idx;
		}
	}
	return -1;
}

static bool is_valid(int cpuid) {
	for (int idx = 0; idx < _ept_max; idx++) {
		if(_eptname_cpu_table[idx].cpuid==cpuid)
			return true;
	}
	return false;
}

static bool check_ept_name(const char* ept_name, int idx) {
	auto ept_name_in_table = _eptname_cpu_table[idx].ept_name;
	if (strncmp(ept_name, ept_name_in_table, strlen(ept_name_in_table)) == 0) {
		return true;
	}
	dbgWarnPXL("ept name not match[%s][%s]", ept_name, ept_name_in_table);
	return false;
}

static bool save_ept(int idx, rpmsg_endpoint* ept) {
	returnIfErrC(false, idx >= _ept_max);
	auto smf_cpu_id = _eptname_cpu_table[idx].cpuid;
	returnIfErrC(false, smf_cpu_id >= SMF_CPU_ID_END);
	_smf_cpu_ept_map[smf_cpu_id] = ept;
	return true;
}

static rpmsg_endpoint* find_ept(int smf_cpu_id) {
	returnIfErrC(0, smf_cpu_id >= SMF_CPU_ID_END);
	return _smf_cpu_ept_map[smf_cpu_id] ? _smf_cpu_ept_map[smf_cpu_id] : _smf_cpu_ept_map[1];
}

bool internal_extra_data_recv(void *data, size_t len) {
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
		//ICSBlock(remote_ics);
		//smf_ics_ready();
		//dbgDevPXL("Update ICS: %p", remote_ics);
		break;
	}
	case INTERNAL_CONNECT_CORE: {
		auto cpu_name = get_remote_cpu_name((int)para);
		returnIfErrC(false, !cpu_name);
		dbgDevPXL("Connect to core : %s", cpu_name);
		returnIfErrC(false, !_connect_cb);
		returnIfErrC(false, !_connect_cb(cpu_name));
		break;
	}
	case INTERNAL_DISCONNECT_CORE: {
		auto cpu_name = get_remote_cpu_name((int)para);
		returnIfErrC(false, !cpu_name);
		dbgDevPXL("Disconnect to core : %s", cpu_name);
		returnIfErrC(false, !_disconnect_cb);
		returnIfErrC(false, !_disconnect_cb(cpu_name));
		break;
	}
	default: dbgErrPXL("unknow cmd:%u", cmd); break;
	}
	return true;
}

bool internal_extra_data_send(rpmsg_endpoint* ept, uint8_t cmd, uint32_t data) {
	returnIfErrC(false, !ept);
	internal_extra_msg_t msg;
	msg.cmd = cmd;
	memcpy(&msg.para[1], &data, 4);
	returnIfErrC(false, !rpmsg_send(ept, &msg, sizeof(msg)));
	return true;
}

bool let_other_core_connect(rpmsg_endpoint* ept, const char *cpu_name) {
	returnIf(true, !_is_main_core);
	returnIfErrC(false, !cpu_name);
	if (_connect_cb) {
		_connect_cb(cpu_name);
	}
	auto ics = ICSBlock();
	returnIfErrC(false, !ics);
	internal_extra_data_send(ept, INTERNAL_UPDATE_ICS, (uint32_t)ics);
	dbgDevPXL("Send ICS(%p) to %p", ics, ept);

	auto binding_idx = get_idx(cpu_name);
	returnIfErrC(false, binding_idx == -1);
	auto binding_smf_cpu_id = _eptname_cpu_table[binding_idx].cpuid;

	/* Send internal msg to other core to create connect */
	for(int idx = 0; idx < _ept_max; idx++) {
		auto cpu_name_in_table = _eptname_cpu_table[idx].cpu_name;
		auto smf_cpu_id = _eptname_cpu_table[idx].cpuid;
		if(!cpu_name_in_table ||  !cpu_name) continue;
		if(0 != strncmp(cpu_name, cpu_name_in_table, strlen(cpu_name_in_table))) {
			auto cur_ept = find_ept(smf_cpu_id);
			if (cur_ept) {
				dbgTestPXL("Let [%s] connect to [%s]", cpu_name_in_table, cpu_name);
				internal_extra_data_send(cur_ept, INTERNAL_CONNECT_CORE, (uint32_t)binding_smf_cpu_id);
				dbgTestPXL("Let [%s] connect to [%s]", cpu_name, cpu_name_in_table);
				internal_extra_data_send(ept, INTERNAL_CONNECT_CORE, (uint32_t)smf_cpu_id);
			}
		}
	}
	return true;
}

bool let_other_core_disconnect(rpmsg_endpoint* ept, const char *cpu_name) {
	returnIf(true, !_is_main_core);
	returnIfErrC(false, !cpu_name);
	if (_disconnect_cb) {
		_disconnect_cb(cpu_name);
	}
	auto disconnecting_idx = get_idx(cpu_name);
	returnIfErrC(false, disconnecting_idx == -1);
	auto disconnecting_smf_cpu_id = _eptname_cpu_table[disconnecting_idx].cpuid;

	/* Send internal msg to other core to create connect */
	for(int idx = 0; idx < _ept_max; idx++) {
		auto cpu_name_in_table = _eptname_cpu_table[idx].cpu_name;
		auto smf_cpu_id = _eptname_cpu_table[idx].cpuid;
		if(!cpu_name_in_table ||  !cpu_name) continue;
		if(0 != strncmp(cpu_name, cpu_name_in_table, strlen(cpu_name_in_table))) {
			auto cur_ept = find_ept(smf_cpu_id);
			if (cur_ept) {
				dbgTestPXL("Let [%s] disconnect to [%s]", cpu_name_in_table, cpu_name);
				internal_extra_data_send(cur_ept, INTERNAL_DISCONNECT_CORE, (uint32_t)disconnecting_smf_cpu_id);
				dbgTestPXL("Let [%s] disconnect to [%s]", cpu_name, cpu_name_in_table);
				internal_extra_data_send(ept, INTERNAL_DISCONNECT_CORE, (uint32_t)smf_cpu_id);
			}
		}
	}
	return true;
}

static int ept_rx_cb(struct rpmsg_endpoint *ept,void *data, size_t len, uint32_t src,void *priv) {
	if (len < sizeof(smf_msg_t)) {
		internal_extra_data_recv(data, len);
		return 0;
	}

	auto scv = (IMsvc*)ept->priv;
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
	const char * cpu_name = rpmsg_get_cpuname(rdev);
	auto idx = get_idx(cpu_name);
	if (idx == -1) {
		dbgIpcPXL("no need create ept on [%s]", cpu_name);
		return 0;
	}
	returnIf(0, !check_ept_name(ept_name, idx));

	/* Create */
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
	returnIf(0, !save_ept(idx, ept));

	/* Register MSVC send callback */
	// scv->Register([](smf_msg_t* msg) {
	// 	struct rpmsg_endpoint * eptt = 0;
	// 	eptt = find_ept(msg->cpudst);
	// 	if (!eptt) {
	// 		if (is_valid(msg->cpudst)) {
	// 			if (smf_ics_is_poweron(msg->cpudst)) {
	// 				for (int i = 0; i < 100; i++) {
	// 					eptt = find_ept(msg->cpudst);
	// 					if (eptt)break;
	// 					else sleep_for(10);
	// 				}
	// 			}
	// 		}
	// 		else {
	// 			eptt = find_ept(SMF_CPU_AP);
	// 		}
	// 	}
	// 	returnIfErrCS(false, !eptt, "msg->cpudst = %d",msg->cpudst);
	// 	rpmsg_send(eptt, msg, msg->size);
	// 	return true;
	// });

	return ept;
}

static bool master_match(struct rpmsg_device* rdev,void* priv, const char* ept_name,uint32_t dest) {
	const char * cpu_name = rpmsg_get_cpuname(rdev);
	auto idx = get_idx(cpu_name);
	if (idx == -1) {
		dbgIpcPXL("no need create ept on %s", cpu_name);
		return false;
	}
	returnIf(false, !check_ept_name(ept_name, idx));
	return true;
}

static void master_bind(struct rpmsg_device* rdev,void* priv, const char* ept_name,uint32_t dest) {
	unique_lock<mutex> lck(_openamp_mtx);
	auto ept = create_ept(rdev, ept_name, dest, (IMsvc*)priv);
	returnIfErrC0(!ept);
	dbgDevPXL("[%s] ept create success", ept_name);
	const char * cpu_name = rpmsg_get_cpuname(rdev);
	let_other_core_connect(ept, cpu_name);
}

static void slave_create(struct rpmsg_device *rdev, void *priv) {
	const char * cpu_name = rpmsg_get_cpuname(rdev);
	auto idx = get_idx(cpu_name);
	if (idx == -1) {
		dbgIpcPXL("no need create ept on %s", cpu_name);
		return;
	}

	/* Only slave need actively create ept */
	if(is_slave(idx)) {
		auto ept_name = _eptname_cpu_table[idx].ept_name;
		auto ret = create_ept(rdev, ept_name, RPMSG_ADDR_ANY, (IMsvc*)priv);
		returnIfErrC0(!ret);
		dbgDevPXL("[%s] ept create success", ept_name);
	}
}

static void destroy(struct rpmsg_device *rdev, void *priv) {
	const char * cpu_name = rpmsg_get_cpuname(rdev);
	auto idx = get_idx(cpu_name);
	if (idx == -1) {
		dbgIpcPXL("no ept created on %s", cpu_name);
		return;
	}

	auto ept = find_ept(_eptname_cpu_table[idx].cpuid);
	if(ept){
		const char * cpu_name = rpmsg_get_cpuname(rdev);
		let_other_core_disconnect(ept, cpu_name);
		dbgDevPXL("%s,%p", cpu_name, ept);
		rpmsg_destroy_ept(ept);
		save_ept(idx, nullptr);
		kmm_free(ept);
	}
	if(_is_main_core) {
		dbgDevPXL("ics power off");
		smf_ics_poweroff(_eptname_cpu_table[idx].cpuid);
	}
}

EXTERNC bool smf_message_eptname_cpu_table_init(smf_msg_eptname_cpu_t *eptname_cpu_table, uint32_t ept_num) {
	returnIfErrC(false, !eptname_cpu_table);
	returnIfErrC(false, !ept_num);
	_eptname_cpu_table = eptname_cpu_table;
	_ept_max = ept_num;
	return true;
}

EXTERNC void smf_rpmsg_register_boot_cb(rpmsg_boot_cb_t connect_cb, rpmsg_boot_cb_t disconnect_cb) {
	_connect_cb = connect_cb;
	_disconnect_cb = disconnect_cb;
}

static bool ipc_send(smf_msg_t* msg) {
	struct rpmsg_endpoint* eptt = 0;
	eptt = find_ept(msg->cpudst);
	if (!eptt) {
		if (is_valid(msg->cpudst)) {
			if (smf_ics_is_poweron(msg->cpudst)) {
				for (int i = 0; i < 100; i++) {
					eptt = find_ept(msg->cpudst);
					if (eptt)break;
					else sleep_for(10);
				}
			}
		}
		else {
			eptt = find_ept(SMF_CPU_AP);
		}
	}
	returnIfErrCS(false, !eptt, "cpudst:%u", msg->cpudst);
	rpmsg_send(eptt, msg, msg->size);
	return true;
}

namespace smf {
	bool smf_message_init(bool master) {
		_is_main_core = master;
		IMsvc* scv = 0;
		returnIfErrC(false, !smf_gets("msvc/imsvc",&scv))
		returnIfErrC(false, !scv);
		scv->Register(&ipc_send);
		rpmsg_register_callback(scv, slave_create, destroy, master_match, master_bind);
		return true;
	}

	bool smf_ipc_open() {
		return true;
	}

	bool smf_ipc_close() {
		return true;
	}
}
