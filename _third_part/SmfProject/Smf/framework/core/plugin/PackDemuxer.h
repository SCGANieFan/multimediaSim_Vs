#pragma once
#include "IOSource.h"
namespace smf{
namespace audio{
	class PackDemuxer
		:public IOSource
	{
	public:
		PackDemuxer();
		virtual ~PackDemuxer();
	protected:
		virtual bool open(void*)override;
		virtual bool close()override;
		//virtual bool seek(int pos) override;
		virtual bool generateFrame(Frame*&)override;
	};
}
}