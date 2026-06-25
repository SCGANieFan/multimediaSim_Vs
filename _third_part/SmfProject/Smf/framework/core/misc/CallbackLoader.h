#pragma once
#include "ILoader.h"
#include "ObjectStatic.h"
#include "smf_common.h"
namespace smf {
	class CallbackLoader
		:public ObjectStatic
		,public ILoader 
	{
	public:
		CallbackLoader(smf_cb_para start, smf_cb_para finish)
			:_entry(start), _exit(finish) 
		{}
	public:
		virtual bool Load(void* para) override {
			return _entry ? _entry(para) : 0;
		}
		virtual bool Unload(void* para) override {
			return _exit ? _exit(para) : 0;
		}
	public:
		smf_cb_para _entry = 0;
		smf_cb_para _exit = 0;
	};
}
