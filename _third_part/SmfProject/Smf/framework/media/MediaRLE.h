#pragma once
#include "MediaVideo.h"
#include "smf_codec_rle.h"

namespace smf {
	class MediaRLE :public MediaVideo, public smf_media_rle_def_t {
	public:
		MediaRLE();
	protected:
		virtual bool setX(uint32_t keys, void* vals)override;
		virtual bool getX(uint32_t keys, void* vals) const override;
	};
}