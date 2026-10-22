#pragma once
#include "MediaVideo.h"
#include "smf_codec_h265.h"

namespace smf {
	class MediaH265 :public MediaVideo, public smf_media_h265_def_t {
	public:
		MediaH265();
	protected:
		virtual bool setX(uint32_t keys, void* vals)override;
		virtual bool getX(uint32_t keys, void* vals) const override;
	};
}