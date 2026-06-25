#pragma once
#include "Sink.h"
namespace smf {
	class DumySink
		: public Sink
	{
	protected:
		virtual bool open(void*)override;
		virtual bool close()override;
		virtual bool receiveFrame(Frame*)override;
		virtual bool set(uint32_t key,void* val)override;
	protected:
		uint32_t _index = 0;
		uint64_t _tp = 0;
		uint32_t _interval = 0;
	};
}
