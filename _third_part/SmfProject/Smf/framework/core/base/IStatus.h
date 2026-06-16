#pragma once
#include "smf_common.h"
#include "smf_api_def.h"
#include "SmfFCC.h"
namespace smf {
	enum class EStatus {
		null = SMF_STS_Null,
		ready = SMF_STS_Ready,
		play = SMF_STS_Play,
		pause = SMF_STS_Pause,
		max = SMF_STS_Max,
	};
	enum class EStatusChange {
		null = 0,
		up = fcc32("up"),
		down = fcc32("down"),
	};
#if 0
	/// <summary>
	///    ==Open==>   ===Start==> ===Pause==>
	/// None       Ready        Play        Pause
	///   <==Close==  <==Stop===  <==Resume==
	/// </summary>
	class IStatus {		
	protected:
		EStatus _sts = EStatus::null;
		EStatus _sts0 = EStatus::null;
	protected:
		virtual bool open(void*);
		virtual bool close();
		virtual bool start();
		virtual bool stop();
		virtual bool pause();
		virtual bool resume();
		virtual bool statusChangeStep(EStatus);
		bool statusChange(EStatus target);
	public:
		bool Open(void* param) ;
		bool Close();
		//bool Start();
		//bool Stop() ;
		//bool Pause();
		//bool Resume();
	public:
		bool Status(EStatus target, EStatusChange chg = EStatusChange::null);
		EStatus Status()const;
		EStatus StatusNext()const;
	public:
		bool IsNone()const;
		bool IsOpen()const;
		bool IsPlaying()const;
		bool IsPaused()const;
	protected:
		void* _openParam = 0;
		int _openParamSize = 0;
	};
#endif
}
