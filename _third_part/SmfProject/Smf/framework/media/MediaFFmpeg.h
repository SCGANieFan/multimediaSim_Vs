#pragma once
#include "MediaVideo.h"
#include "smf_codec_ffmpeg.h"

namespace smf {
	class MediaFFmpeg :public MediaVideo, public smf_media_ffmpeg_def_t {
	public:
		MediaFFmpeg();
	protected:
		virtual bool setX(uint32_t keys, void* vals)override;
		virtual bool getX(uint32_t keys, void* vals) const override;
	};
}