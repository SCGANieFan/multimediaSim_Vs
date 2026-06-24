#pragma once
#include "SmfRing.h"
#include "SmfFifoBuffer.h"
#include "smf_debug.h"
namespace smf {
	template<class Base>
	class TRingBuffer : public TRing<Base> {
	public:
		uint32_t WriteWithCache(const void* data, uint32_t size, bool all = false) { return Base::WriteWithCache(data, size, all, true); }
		uint32_t Fill(char chr, uint32_t count, bool cache = false) { return Base::Fill(chr, count, cache, true); }
		uint32_t FillWithCache(char chr, uint32_t count) { return Base::FillWithCache(chr, count, true); }
	};

	using SmfRingBuffer = TRingBuffer<SmfFifoBuffer>;
	template<int _max, int _align = 0>
	using SmfRingBufferS = TRingBuffer<SmfFifoBufferS<_max, _align>>;
	using SmfRingBufferD = TRingBuffer<SmfFifoBufferD>;
}
