#pragma once
#include "AutoElement.h"
#include "Source.h"
#include "string.hh"
namespace smf {
	class AutoSource
		:public Source
	{
	protected:
		AutoElement _ele{ this };
		string _url;
	protected:
		virtual bool set(uint32_t key, void* val)override;
		virtual bool get(uint32_t key, void* val)const override;
		virtual bool open(void*)override;
		virtual bool generateFrame(Frame*& ofrm) override final { return false; }
	private:
		Ports _oports{this, false};
	};
}