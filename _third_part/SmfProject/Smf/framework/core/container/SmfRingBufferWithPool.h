#pragma once
#include "SmfRingBuffer.h"
#include "SmfFifoBufferWithPool.h"
namespace smf {
	using SmfRingBufferWithPool = TRingBuffer<SmfFifoBufferWithPool>;
}
