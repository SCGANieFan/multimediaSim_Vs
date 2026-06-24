#pragma once
#include "BackGround.h"
#include "string.hh"
#include "IO.h"

namespace smf {
	class ShmLogService :public BackGround::Item {
	public:
		using Base = BackGround::Item;
		ShmLogService();
		virtual ~ShmLogService();
	protected:
		virtual bool set(uint32_t keys, void* vals)override;
		//virtual bool get(uint32_t keys, void* vals) const override;
		virtual bool run(void*)override;
	protected:
		IO* _io = 0;
		string _url;
		uint32_t _duration = 0;
		uint64_t _tp = 0;
	protected:
		bool updateIO();
	};
}
