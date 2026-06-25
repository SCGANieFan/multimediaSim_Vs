#pragma once
#include "Source.h"
namespace smf{
    class DumySource
        : public Source
    {
	protected:
		virtual bool open(void*)override;
		virtual bool generateFrame(Frame*& ofrm) override;
		virtual bool set(uint32_t keys,void* vals) override;
	protected:
		uint32_t _intervalMs = 10;
		uint32_t _eosCount = 0;
		uint32_t _ext[4];
		uint64_t _timestamp = 0;
		uint32_t _count = 0;
	};
}