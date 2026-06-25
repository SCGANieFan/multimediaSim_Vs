#pragma once
#include "IOSink.h"
namespace smf {
namespace audio {
	class PackMuxer
		: public IOSink
	{
	public:
		PackMuxer();
		virtual ~PackMuxer();
	protected:
		virtual bool open(void*)override;
		virtual bool close()override;
		virtual bool receiveFrame(Frame*)override;
	};
}
}
