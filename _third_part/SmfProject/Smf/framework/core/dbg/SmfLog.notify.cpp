#include "SmfLog.notify.h"
#include "IPool.h"
#include "SmfICS.h"
#include "SmfDef.h"
#include "SmfDef.gbl.h"
#include "SmfFCC.h"
#include "SmfPorting.h"
#include "ThreadList.h"
#include "smf_error.h"
#include "smf_debug.h"
#include "chip.h"
using namespace smf;
EXTERNC void smf_notify_bg_register() {
	BGNotify::Register<BGNotify>("notify");
}

#define dbg_chn_svc dbg_chn_note
#define dbg_svc_printf(...) dbgChnEntry(svc,##__VA_ARGS__)
#define dbgSvcX(...) dbgChnX(svc,##__VA_ARGS__)
#define dbgSvcXL(...) dbgChnXL(svc,##__VA_ARGS__)
#define dbgSvcPXL(...) dbgChnPXL(svc,##__VA_ARGS__)
#define dbgSvcDump(...) dbgChnDump(svc,##__VA_ARGS__)
#define dbgSvcOutput(d,s) d[s]='\n';dbgOutput(dbg_chn_note,d,s+1)
EXTERNC const char* smf_version();

BGNotify::BGNotify() {
	Global<BGNotify>(eGlobal::bgnotify, this);
	_id = fcc64("notify");
	memset(_powerOffTimestamp, 0, sizeof(_powerOffTimestamp));
	//Invoke(0, 1000);
}
BGNotify::~BGNotify() {
	Global<BGNotify>(eGlobal::bgnotify, 0);
}
bool BGNotify::set(uint32_t keys, void* vals) {
	switch (keys) {
	//disable Background property.
	case Hash("invoke")://compatible backward
	case Hash("delay"):
	case Hash("loop"): _notifyInterval = (uint32_t)vals/1000; Invoke(1000); return true;
	//ctrl
	case Hash("pauseBg"):_pauseBg = (uint32_t)vals; return true;
	case Hash("pauseBgMax"):_pauseMax = (uint32_t)vals; return true;
	//notify
	case Hash("notifyEnable"):_notifyEnable = (uint32_t)vals; return true;
	case Hash("notifyInterval"):_notifyInterval = (uint32_t)vals; return true;
	//sysfreq record/adjust
	case Hash("sysfreqRequestEnable"):_sysfreqRequest = (uint32_t)vals; return true;
	case Hash("sysfreqRequest"): SysFreqRequest((uint32_t)vals); return true;
	case Hash("sysfreq"):
	case Hash("sysfreqAdjust"):_sysfreqAdjust = (uint32_t)vals; return true;
	case Hash("sysfreqRecord"):_sysfreqRecord = (uint32_t)vals; return true;
	//cpu power on/off
	case Hash("powerCtrl"):if (vals)_powerCtrl |= 1u << (uint32_t)vals; else _powerCtrl = 0; return true;
	case Hash("powerOffDelay"):_powerCtrlDelay = (uint32_t)vals; return true;
	case Hash("powerOff"):PowerOff((uint32_t)vals); return true;
	case Hash("powerOn"):PowerOn((uint32_t)vals); return true;
	}
	return Base::set(keys, vals);
}

bool BGNotify::run(void*para) {
	ThreadList::List().Update();
	Process();
	//
	NotifyProcess();
	SysFreqRecordProcess();
	SysFreqAdjustProcess();
	PowerCtrlProcess();
	return true;
}

void BGNotify::Process() {
	//background ctrl
	if (_pauseBg && !(_sysfreqAdjust && ics::GetFreqMax())) {
		_pauseCnt++;
		if (_pauseCnt > _pauseMax) {
			_pauseCnt = 0;
			((BackGround*)Parent())->Pause(true);
			_pauseBg = false;
			if (_sysfreqRecord) {
				ics::SetFreq(0);
			}
		}
	}
	else {
		_pauseCnt = 0;
	}
	//rtc sync
	if (Root().Flags().Check(IS_Master) && ((_index & 3) == 3)) {
		ics::RtcSync();
	}
	//ics check
	ics::Check();
}
void BGNotify::NotifyProcess() {
	if (_notifyEnable) {
		if (_index >= _notify + _notifyInterval) {
			Notify();
			_notify = _index;
		}
	}
}
void BGNotify::Notify() {
	auto buff = _buff;
	auto ptr = buff;
	auto end = buff + sizeof(_buff);
	auto usage = get_cpu_usage();
	auto freq = get_cpu_freq();
	auto load = get_workloads();
	uint32_t used = (uint64_t)freq * usage / (1000000 * 10000ull);
	uint32_t used_smf = (uint64_t)freq * load / (1000000 * 10000ull);
	ptr = snprintf(ptr, end, "\n%u[smf][3][%u,%u]%u%%,%u/%u/%umhz,", get_timestamp(), get_cpu_idx(), get_thread_idx()
		, usage / 100
		, used_smf, used, freq / 1000000
	);
	auto ptr0 = ptr;
	switch (_notifyIndex & 3) {
	case 0:ptr = Root().Print(ptr, end); break;
	case 1:ptr = ThreadList::List().Print(ptr, end); break;
	case 2:ptr = Pools().Print(ptr, end); break;
	case 3:
		switch (_notifyIndex & 12) {
		case 0: ptr = rtc_printf(ptr, end); break;
		case 4: ptr = snprintf(ptr, end, "%s", smf_version()); break;
		case 8: break;
		case 12:break;
		default:break;
		}
		break;
	default: break;
	}
	if (ptr != ptr0) {
		if (Root().Error().err & SMF_ERROR_MASK_CHIP_CPU)buff[0] = '0';
		dbgSvcOutput(buff, ptr - buff);
	}
	_notifyIndex++;
}

void BGNotify::SysFreqRequest(uint32_t freq) {
	if (_sysfreqRequest) {
		set_cpu_freq(freq);
	}
}
void BGNotify::SysFreqRecordProcess() {
	if (_sysfreqRecord) {
		auto usage = get_cpu_usage();
		auto freq = get_cpu_freq();
		uint32_t used = (uint64_t)freq * usage / (1000000 * 10000ull);
		ics::SetFreq(used);
	}
}
void BGNotify::SysFreqAdjustProcess() {
	if (_sysfreqAdjust) {
		set_cpu_freq(ics::GetFreqMax()*1000000);
	}
}
void BGNotify::PowerCtrlProcess() {
	if (_powerCtrl) {
		auto bg = (BackGround*)Parent();
		unique_lock<mutex> lck(bg->Mutex());
		auto timestamp = get_timestamp();
		auto delay = _powerCtrlDelay;
		for (int i = 0; i < 8; i++) {
			auto& tp = _powerOffTimestamp[i];
			if (tp && tp + delay < timestamp) {
				//dbgTestPXL("%u,%u+%u<%u",i,tp,delay,timestamp);
				cpu_close(i);
				ics::SetFreq(i, 0);
				tp = 0;
			}
		}
	}
}
void BGNotify::PowerOn(uint8_t cpuid) {
	//dbgTestPXL("%08x,%u,%u", _powerCtrl, cpuid, _powerOffTimestamp[cpuid]);
	if (_powerCtrl & (1u << cpuid)) {
		auto bg = (BackGround*)Parent();
		unique_lock<mutex> lck(bg->Mutex());
		if (_powerOffTimestamp[cpuid]) {
			_powerOffTimestamp[cpuid] = 0;
		}
		else if(!ics::IsPowerOn(cpuid)){
			cpu_open(cpuid);
		}
	}
}
void BGNotify::PowerOff(uint8_t cpuid) {
	//dbgTestPXL("%08x,%u", _powerCtrl, cpuid);
	if (_powerCtrl & (1u << cpuid)) {
		auto bg = (BackGround*)Parent();
		unique_lock<mutex> lck(bg->Mutex());
		_powerOffTimestamp[cpuid] = get_timestamp();
	}
}
