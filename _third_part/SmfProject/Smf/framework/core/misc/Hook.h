#pragma once
#include "IHook.h"
#include "Frame.h"
namespace smf {
	class HookCb :public IHook {
	public:
		HookCb(CbFrame cb,void* priv):_cb(cb), _priv(priv){}
	public:
		virtual bool Hook(Frame*&frm) override {
			return _cb ? _cb((smf_frame_t**)&frm,_priv) : false;
		}
	protected:
		CbFrame _cb = 0;
		void* _priv = 0;
	};

	class HookOutput :public IHook {
	protected:
		Frame** _frm = 0;
	public:
		HookOutput(Frame*& frm) :_frm(&frm) {}
		virtual bool Hook(Frame*& frm) override {
			*_frm = frm;
			return true;
		}
	};

	class HookInput :public IHook {
	protected:
		Frame* _frm = 0;
	public:
		HookInput(Frame* frm) :_frm(frm) {}
		virtual bool Hook(Frame*& frm) override {
			frm = _frm;
			return true;
		}
	};
}