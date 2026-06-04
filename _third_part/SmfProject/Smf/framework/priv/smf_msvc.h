#pragma once
#include "SmfMsg.h"
#include "SmfMsg.def.h"
#include "SmfPortingDevice.h"
#include <functional>
namespace smf {
	namespace msg {
		enum {
			SMF_MSG_Invoke = SMF_MSG_MSVC,
			SMF_MSG_InvokeResponse,
			SMF_MSG_Config,//2
			SMF_MSG_ConfigResponse,
			SMF_MSG_Sync,//4
			SMF_MSG_SyncResponse,
			SMF_MSG_Rev0,//6
			SMF_MSG_Rev0Response,
			SMF_MSG_Rev1,//8
			SMF_MSG_Rev1Response,
			SMF_MSG_Device,//a
			SMF_MSG_DeviceResponse,
			SMF_MSG_MIPS,//c
			SMF_MSG_MIPSResponse,
			SMF_MSG_Memcpy,//e
			SMF_MSG_MemcpyResponse,

			SMF_MSG_Algo,//10
			SMF_MSG_AlgoResponse,
			SMF_MSG_MIPSReport,//12
			SMF_MSG_MIPSReportResponse,
			SMF_MSG_WakeUp,//14
			SMF_MSG_WakeUpResponse,
			SMF_MSG_Extra,//16
			SMF_MSG_ExtraResponse,
			SMF_MSG_Test,//18
			SMF_MSG_TestResponse,
			SMF_MSG_LogSet,//1a
			SMF_MSG_LogSetResponse,
		};
		//
		struct ConfigV1_t {
			void* buff;
			uint32_t size;
			uint16_t params_size;
			uint16_t script_size;
			char* script;
			uint32_t* params;
		};
		//
		struct Invoke_t {
			std::function<void(void)> func;
		};
		using MsgInvoke_t = TMsg<Invoke_t>;
		//
		struct Memcpy_t {
			void* dst;
			void* src;
			uint32_t size;
		};
		//
		// struct Mips_t {
		// 	uint32_t freq_mhz;
		// 	int avg_mips;
		// 	int max_mips;
		// 	int min_mips;
		// };
		using Mips_t = smf_mips_t;
		//
		struct Sync_t {
			//uint32_t local_timer;
			//uint32_t local_timer_sync;
			//uint32_t rtc_sync;
			uint32_t sys_time;//ms
			uint32_t fast_time;//ms
			uint32_t rtc;//ms
			uint32_t rev;//align64
			uint64_t local_time;//ms
		};
	
		struct algocfg_t {
			uint32_t sample_rate;
			uint8_t sample_bits;
			uint8_t channel_num;
			uint8_t rev[2];
			uint32_t chmap;
			void* pri;
		};
		struct SetDevice_t {
			uint32_t devid;
			uint32_t opt;//0:close,1:open
			MediaDeviceCfg_t cfg;
		};
		struct SetAlgo_t {
			uint32_t algoid;
			uint32_t status;//0:close,1:open,2:set;3:get
			algocfg_t cfg;
		};
		struct SetLog_t {
			union {
				struct {
					bool en;
					uint8_t rev[1];
					uint16_t ch;
				};
				struct {
					uint32_t flags;
				};
			};
			bool set_by_flags;
		};
		using WakeUpRes_t = smf_wakeup_t;
		using SendExtData = smf_extra_t;
		//
		enum {
			CONFIG_V1 = 1u << 0,
			MONOPOLY = 1u<<1,
		};
		//
		typedef struct SmfCpuidCfg_t {
			uint8_t cpuid;
			uint16_t start : 12;
			uint16_t end : 12;
		}SmfCpuidCfg_t;
	}
}

EXTERNC bool smf_msvc_open(bool master);
EXTERNC bool smf_msvc_close();
EXTERNC bool smf_msvc_sync(uint8_t cpuid, int timeout);
EXTERNC bool smf_msvc_is_ready(uint8_t cpuid);
