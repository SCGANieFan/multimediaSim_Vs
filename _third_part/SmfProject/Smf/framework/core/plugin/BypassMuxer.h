#pragma once
#include "AudioMuxer.h"
#include "MediaAudio.h"

namespace smf {
namespace audio {
	class BypassMuxer
		: public AudioMuxer
	{
	public:
		BypassMuxer();
		virtual ~BypassMuxer();
	protected:
		virtual bool open(void*)override;
		virtual bool close()override;
		virtual bool receiveFrame(Frame*)override;
		virtual bool setParam(void*)override;
	};
}
}
