#pragma once
#include "SmfMsg.h"
#include "SmfMsg.def.h"
#include "SmfPortingDevice.h"
namespace smf {
	namespace ssvc {
		enum eStreamMax {
			eStreamMax = 128,
			eSetidMax = 31,
		};
		enum class eStream : uint16_t {
			null = 0,
			//server
			audioPlayer = 0x080,
			audioRecord = 0x090,
			streamServer = 0x0a0,
			streamCommand = 0x0f0,
			//cmd
			add = 0,
			addResponse,
			remove,
			removeResponse,
			set,
			setResponse,
			get,
			getResponse,
			//command
			job = 0x0f1,//0x0f0 unused
			jobResponse = 0x0f1,
			device = 0x0f3,//0x0f2 unused
			deviceResponse = 0x0f3,
			notify = 0x0ff,//0x0fe unused
			notifyResponse = 0x0ff,
		};
		constexpr eStream Combine(eStream a, eStream b) {
			return (eStream)((uint16_t)a + (uint16_t)b);
		}
		enum class eNotify : uint8_t {
			begin = 0,
			eos,//1
			error,//2
			done,//3
			status,//4
			progress,//5
			meta,//6
			media,//7
			start_sucess,//8
			start_fail,//9
			vad = 0x10,
			wakeup,//0x11
			aivioce,//0x12
			kws,//0x13
			stop_sucess,//0x14
			stop_fail,//0x15
			voip,//0x16
			end,
		};

		template<int X>
		struct XMsgAdd {
			uint32_t para[4];//0~3
			uint64_t name;//4~5
			uint32_t params[X];//6~45
		};
		using MsgAdd_t = XMsgAdd<0>;

		struct MsgRemove_t {
			uint64_t name;
		};

		template<int X>
		struct XMsgSet {
			uint64_t data_pointer;
			uint16_t path;
			uint16_t is_value;
			uint32_t data_size;
			char data[X];
		};
		using MsgSet_t = XMsgSet<0>;

		template<int X>
		struct XMsgGet {
			uint64_t callback;
			uint64_t callback_priv;
			uint64_t sema;
			uint64_t buff;
			uint32_t timepoint;
			uint32_t data_size;
			char data[X];
		};
		using MsgGet_t = XMsgGet<0>;

		struct MsgJob_t {
			uint32_t other;
		};

		struct MsgDevice_t {
			MediaDeviceCfg_t cfg;
		};

		template<int X>
		struct XMsgNotify {
			uint8_t seqNO;
			uint8_t rev8;
			uint16_t data_size;
			uint8_t data[X];
		};
		using MsgNotify_t = XMsgNotify<0>;

		enum class eFlagsExt : uint32_t {
			cacheable = 1u << 0,
			job = 1u << 1,
			config = 1u << 2,
		};
	}
}