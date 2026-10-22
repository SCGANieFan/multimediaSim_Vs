#pragma once
#include "MediaVideo.h"
#include "smf_codec_png.h"

namespace smf {
	class MediaPng :public MediaVideo, public smf_media_png_def_t {
	public:
		MediaPng();
	protected:
		virtual bool setX(uint32_t keys, void* vals)override;
		virtual bool getX(uint32_t keys, void* vals) const override;
	};
}