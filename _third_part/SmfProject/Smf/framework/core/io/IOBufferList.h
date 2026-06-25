#pragma once
#include "IOBuffer.h"
#include "SimpleTable.h"
namespace smf {
	class IOBufferX :public IOBuffer {
	public:///status ctrl
		virtual bool Open(void* para)override;
	};

	class IOBufferList :public Object {
	public:
		SimpleTableC<Tuple2<void*, uint32_t>, 8> _table;
	};
}
