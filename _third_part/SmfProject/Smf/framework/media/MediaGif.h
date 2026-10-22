#pragma once
#include "MediaVideo.h"
#include "smf_codec_gif.h"

namespace smf {
	class MediaGif :public MediaVideo, public smf_media_gif_def_t {
	public:
		MediaGif();
	protected:
		virtual bool setX(uint32_t keys, void* vals)override;
		virtual bool getX(uint32_t keys, void* vals) const override;
	};
}