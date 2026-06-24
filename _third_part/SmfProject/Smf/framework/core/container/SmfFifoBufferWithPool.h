#pragma once
#include "SmfFifoBuffer.h"
#include "TPool.h"
#include "smf_common.h"

namespace smf{
	class SmfFifoBufferWithPool
		: public TPool<SmfFifoBuffer>
	{
	public:
		using Base = TPool<SmfFifoBuffer>;
		SmfFifoBufferWithPool(int max = 0, int align = 0, IPool* pool = 0);
		~SmfFifoBufferWithPool();
	protected:
		using Base::Initialize;
	public:
		bool Initialize(uint32_t max, int align = 0, IPool* pool = 0);
		void Uninit();
	protected:
		fifo_t _fifoD;
	};
}
