#pragma once
#include "Object.h"
#include "IStatus.h"
#include "smf_api_def.h"
namespace smf {
	class Service
		: public Object
	{
	public:
		Service();
		virtual ~Service();
	protected:
		virtual bool set(uint32_t key, void* val)override;
		virtual bool get(uint32_t key, void* val)const override;
	protected://IStatus
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
		virtual char* print(char* ptr, char* end) const override;
	private:
		bool statusChange(EStatus target);
	protected:
		bool statusChangeSteps(EStatus sts, Object** list, int N);
	public:
		bool Status(EStatus target, EStatusChange chg = EStatusChange::null);
		EStatus Status()const;
		EStatus StatusNext()const;
		using Object::Register;
	public:
		bool IsNone()const;
		bool IsOpen()const;
		bool IsPlaying()const;
		bool IsPaused()const;
	protected:
		void* _openParam = 0;
	public://for path_smf_api
		bool Open(void* param);
		bool Close();
	protected:
		virtual bool setParam(void*);
	};
}