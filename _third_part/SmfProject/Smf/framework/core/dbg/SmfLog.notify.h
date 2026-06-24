#pragma once
#include "BackGround.h"

namespace smf {
	class BGNotify :public BackGround::Item {
	public:
		using Base = BackGround::Item;
		BGNotify();
		virtual ~BGNotify();
	protected:
		virtual bool set(uint32_t keys, void* vals)override;
		//virtual bool get(uint32_t keys, void* vals)const override;
		virtual bool run(void*)override;
	protected:
		bool _notifyEnable = true;
		uint32_t _notify = 0;
		uint32_t _notifyInterval = 1;
		uint32_t _notifyIndex = 0;
		char _buff[1024 * 4];
	protected:
		uint8_t _pauseCnt = 0;
		uint8_t _pauseMax = 8;
		bool _pauseBg = false;
	protected:
		bool _sysfreqRequest = true;
		bool _sysfreqAdjust = false;
		bool _sysfreqRecord = true;
	protected:
		uint32_t _powerOffTimestamp[8];
		uint32_t _powerCtrlDelay = 2000;
		uint8_t _powerCtrl = 0;
	protected:
		void Process();
	protected:
		void NotifyProcess();
		void Notify();
	protected:
		void SysFreqRequest(uint32_t freq);
		void SysFreqRecordProcess();
		void SysFreqAdjustProcess();
	protected:
		void PowerCtrlProcess();
		void PowerOff(uint8_t cpuid);
		void PowerOn(uint8_t cpuid);
	};
}
