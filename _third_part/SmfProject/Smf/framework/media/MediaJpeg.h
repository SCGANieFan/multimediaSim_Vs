#pragma once
#include "MediaVideo.h"
#include "smf_codec_jpeg.h"

namespace smf {
	class MediaJpeg :public MediaVideo, public smf_media_jpeg_def_t {
	public:
		MediaJpeg();
	protected:
		virtual bool setX(uint32_t keys, void* vals)override;
		virtual bool getX(uint32_t keys, void* vals) const override;
	};
}