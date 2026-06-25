#include "smf_debug.h"
#include "Object.h"
#include "SmfPorting.h"
#include "SmfString.h"
#include "SmfICS.h"
#include "SmfICS.def.h"
#include "time.h"
#include "SmfDef.h"
using namespace smf;
using namespace smf::ics;
#define IS_ALGIN64(v) (!(((uint32_t)(v))&63))
namespace smf{
	namespace ics {
		bool Register(void* buff, int size, bool master) {
			dbgTestPXL("[ics]%p,%d>%u,%d", buff, size, sizeof(smf_ics_t), master);
			returnIfErrC(false, !buff);
			returnIfErrC(false, size < sizeof(smf_ics_t));
			ICSBlock(buff);
			auto _ics = (smf_ics_t*)buff;
			if (master) {
				memset(buff, 0, size);
				_ics->magic = _ics->magic_end = SMF_ICS_MASK;
				_ics->version = SMF_ICS_VERSION;
				_ics->size = _ics->size_end = sizeof(smf_ics_t);
				RtcSync();
				Root().Flags().Set(IS_Master, true);
			}
			else if (_ics->magic == SMF_ICS_MASK && _ics->size == sizeof(smf_ics_t)
				&& _ics->magic_end == SMF_ICS_MASK && _ics->size_end == sizeof(smf_ics_t)
				&& _ics->version == SMF_ICS_VERSION
				) {
			}
			else {
				dbgErrPXL("ics fail!");
				dbgErrDump(buff, size);
				return false;
			}
			//
			auto cpuid = get_cpu_idx();
			_ics->names[cpuid] = CpuName64();
			_ics->power |= 1u << cpuid;
			return true;
		}
		//
		static void* _xtid = 0;
		Locker::Locker(uint32_t timeout) 
			:smf::Locker(((smf_ics_t*)ICSBlock())->lock, _xtid, timeout)
		{
		}
		///
		void RtcSync() {
			auto _ics = (smf_ics_t*)ICSBlock();
			returnIfErrC0(!_ics);
			auto& local = _ics->time;
			auto lck = intr_lock();
			local.sys = get_timestamp();
			local.rtc = time(0);
			intr_unlock(lck);
		}

		uint64_t GetRtcMs() {
			auto _ics = (smf_ics_t*)ICSBlock();
			returnIfErrC(0, !_ics);
			auto& local = _ics->time;
			return (uint64_t)get_timestamp() + local.rtc - local.sys;
		}
		uint8_t& CpuRefs(uint8_t id) {
			auto _ics = (smf_ics_t*)ICSBlock();
			return _ics->cpus[id];
		}
		bool SetPower(bool on, uint8_t cpuid) {
			auto _ics = (smf_ics_t*)ICSBlock();
			returnIfErrC(false,!_ics);
			if(!cpuid)
				cpuid = get_cpu_idx();
			if (on) {
				_ics->power |= 1u<<cpuid;
			}
			else {
				_ics->power &= ~(1u << cpuid);
				_ics->ready &= ~(1u << cpuid);
				//_ics->hooks[cpuid] = 0;
				//memset(&_ics->log[cpuid], 0, sizeof(smf_fifo_t));
			}
			return true;
		}
		bool SetReady() {
			auto _ics = (smf_ics_t*)ICSBlock();
			returnIfErrC(false, !_ics);
			auto cpuid = get_cpu_idx();
			_ics->ready |= 1u << cpuid;
			return true;
		}
	
		bool SetReady(uint8_t cpuid) {
			auto _ics = (smf_ics_t*)ICSBlock();
			returnIfErrC(false, !_ics);
			_ics->ready |= 1u << cpuid;
			return true;
		}

		bool IsPowerOn(uint8_t cpuid) {
			auto _ics = (smf_ics_t*)ICSBlock();
			returnIfErrC(false, !_ics);
			if (!cpuid)cpuid = get_cpu_idx();
			auto mask = 1u << cpuid;
			return _ics->ready & mask;
		}
		bool IsReady(uint8_t cpuid, int timeout, int skip) {
			auto _ics = (smf_ics_t*)ICSBlock();
			returnIfErrC(false, !_ics);
			if (!cpuid)cpuid = get_cpu_idx();			
			auto mask = 1u << cpuid;
			if ((_ics->magic == 0xbe5be5be) && (_ics->ready & mask)) {
				return true;
			}
			if (timeout) {
				if (skip <= 0) {
					skip = 10;
				}
				auto cnt = timeout;
				while (cnt > 0) {
					if ((_ics->magic == 0xbe5be5be) && (_ics->ready & mask)) {
						return true;
					}
					sleep_for(skip);
					cnt -= skip;
				}
				dbgWarnPXL("%d,%d", cpuid, timeout);
			}
			return false;
		}
		uint64_t GetCpuName64(uint8_t cpuid) {
			auto _ics = (smf_ics_t*)ICSBlock();
			returnIfErrC(0, !_ics);
			return _ics->names[cpuid];
		}
		const char* GetCpuName(uint8_t cpuid) {
			auto _ics = (smf_ics_t*)ICSBlock();
			returnIfErrC(0, !_ics);
			return (const char*)&_ics->names[cpuid];
		}

		void* GetDisplay(uint8_t idx) {
			auto _ics = (smf_ics_t*)ICSBlock();
			returnIfErrC(0, !_ics);
			returnIfErrC(0, idx >= sizeof(_ics->display) / sizeof(_ics->display[0]));
			auto buff = _ics->display[idx];
			returnIfErrC(0, !buff);
			//cache_invalid(buff, sizeof(smf_display_t));
			return buff;
		}

		bool SetScript(void* addr, uint32_t size, bool cache) {
			dbgTestPXL("%p,%u,%u", addr, size, cache);
			auto _ics = (smf_ics_t*)ICSBlock();
			returnIfErrC(false, !_ics);
			returnIfErrC(false, !addr);
			returnIfErrC(false, !size);
			_ics->script.data = (uint32_t)addr;
			_ics->script.size = size;
			if (cache) {
				returnIfErrC(false, cache && !IS_ALGIN64(addr));
				cache_writeback(addr, Align64(size));
			}
			return true;
		}

		const char* GetScript(bool cache) {
			auto _ics = (smf_ics_t*)ICSBlock();
			returnIfErrC(0, !_ics);
			auto addr = _ics->script.data;
			auto size = _ics->script.size;
			returnIfErrC(0, !addr);
			returnIfErrC(0, !size);
			if (cache) {
				returnIfErrC(0, cache && !IS_ALGIN64(addr));
				cache_invalid((void*)addr, Align64(size));
			}
			dbgTestPXL("%p,%u,%u", addr, size, cache);
			return (const char*)addr;
		}

		void* GetStream() {
			auto _ics = (smf_ics_t*)ICSBlock();
			returnIfErrC(0, !_ics);
			return& _ics->stream;
		}

		struct mmgr_t* GetMmgr() {
			auto _ics = (smf_ics_t*)ICSBlock();
			returnIfErrC(0, !_ics);
			return &_ics->mmgr;
		}

		void* GetLogFifo(uint8_t cpuid) {
			auto _ics = (smf_ics_t*)ICSBlock();
			returnIfErrC(0, !_ics);
			if (!cpuid)cpuid = get_cpu_idx();
			return &_ics->log[cpuid];
		}
		
		void* GetHook(uint8_t cpuid) {
			auto _ics = (smf_ics_t*)ICSBlock();
			returnIfErrC(0, !_ics);
			if (!cpuid)cpuid = get_cpu_idx();
			return (void*)_ics->hooks[cpuid];
		}

		bool SetHook(void* hook, uint8_t cpuid) {
			auto _ics = (smf_ics_t*)ICSBlock();
			returnIfErrC(false, !_ics);
			if (!cpuid)cpuid = get_cpu_idx();
			_ics->hooks[cpuid] = (uint32_t)hook;
			return true;
		}

		uint8_t* GetMsgRoute() {
			auto _ics = (smf_ics_t*)ICSBlock();
			returnIfErrC(0, !_ics);
			return _ics->msg.route;
		}

		bool SetFreq(uint32_t freq) {
			return SetFreq(get_cpu_idx(), freq);
		}
		bool SetFreq(uint8_t cpuid, uint32_t freq) {
			auto _ics = (smf_ics_t*)ICSBlock();
			returnIfErrC(false, !_ics);
			_ics->sysfreq[cpuid] = freq;
			return true;
		}
		uint32_t GetFreqMax() {
			auto _ics = (smf_ics_t*)ICSBlock();
			returnIfErrC(0, !_ics);
			uint32_t freqmax = 0;
			for (auto freq : _ics->sysfreq) {
				if (freqmax < freq) {
					freqmax = freq;
				}
			}
			return freqmax;
		}
		bool CheckIcsBuffer(void* buff, int size){
			if (!buff)return false;
			auto _ics = (smf_ics_t*)buff;
			if (_ics->magic == SMF_ICS_MASK && _ics->size == sizeof(smf_ics_t)
				&& _ics->magic_end == SMF_ICS_MASK && _ics->size_end == sizeof(smf_ics_t)
				&& _ics->version == SMF_ICS_VERSION
				){
					return true;
				}else{
					dbgErrPXL("ics fail!");
					dbgErrDump(buff, size);
					return false;
				}
		}
		bool Check() {
			return CheckIcsBuffer(ICSBlock(), sizeof(smf_ics_t));
		}
	}
}
