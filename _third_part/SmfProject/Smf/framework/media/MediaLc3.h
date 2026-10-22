#pragma once
#include "MediaAudio.h"
#include "smf_codec_lc3.h"

namespace smf {	
	class MediaLc3 :public MediaAudio, public smf_media_lc3_def_t {
	public:
		MediaLc3();
	protected:
		virtual bool setX(uint32_t keys, void* vals)override;
		virtual bool getX(uint32_t keys, void* vals) const override;
	public:
		virtual uint32_t FrameMax() const override { return _frameSize; }
	};

}