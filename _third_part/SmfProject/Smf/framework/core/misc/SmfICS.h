#pragma once
#include <stdint.h>
#include "Locker.h"
#include "smf_debug.h"
//ics: inter core shared-block
namespace smf {
	namespace ics {
		bool Register(void* buff, int size, bool master);
		//
		class Locker: public smf::Locker {
		public:
			Locker(uint32_t timeout = 0);
		};
		//
		bool SetPower(bool on, uint8_t cpuid = 0);
		bool SetReady();
		bool SetReady(uint8_t cpuid);
		bool IsPowerOn(uint8_t cpuid);
		bool IsReady(uint8_t cpuid = 0, int timeout = 0, int skip = 0);
		uint64_t GetCpuName64(uint8_t cpuid);
		const char* GetCpuName(uint8_t cpuid);
		uint8_t& CpuRefs(uint8_t id);
		//
		void* GetDisplay(uint8_t idx = 0);
		uint64_t GetRtcMs();
		void RtcSync();
		//
		bool SetScript(void* buff, uint32_t size, bool cache = false);
		const char* GetScript(bool cache = false);
		//
		void* GetStream();
		//
		struct mmgr_t;
		struct mmgr_t* GetMmgr();
		//
		void* GetLogFifo(uint8_t cpuid = 0);
		//
		void* GetHook(uint8_t cpuid = 0);
		bool SetHook(void*, uint8_t cpuid = 0);
		//
		uint8_t* GetMsgRoute();
		//
		bool SetFreq(uint32_t freq);//mhz
		bool SetFreq(uint8_t cpuid, uint32_t freq);//mhz
		uint32_t GetFreqMax();
		bool CheckIcsBuffer(void* buff, int size);
		bool Check();
	}
}
