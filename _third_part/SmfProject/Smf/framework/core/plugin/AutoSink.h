#pragma once
#include "AutoElement.h"
#include "Sink.h"
#include "string.hh"
namespace smf {
	class AutoSink
		:public Sink
	{
	protected:
		AutoElement _ele{ this };
		string _url;
	protected:
		virtual bool set(uint32_t key, void* val)override;
		virtual bool open(void*)override;
		virtual bool receiveFrame(Frame*)override { return false; }
	};
}