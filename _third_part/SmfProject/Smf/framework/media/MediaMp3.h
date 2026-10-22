#pragma once
#include "MediaAudio.h"
#include "smf_codec_mp3.h"
namespace smf {
	class MediaMp3 :public MediaAudio,public smf_media_mp3_def_t {
	public:
		MediaMp3();
	protected:
		//virtual bool setX(uint32_t keys, void* vals)override;
		//virtual bool getX(uint32_t keys, void* vals) const override;
	public:
		virtual uint32_t FrameMax() const override { return 1706; }
	};
}