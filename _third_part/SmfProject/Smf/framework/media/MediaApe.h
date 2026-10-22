#pragma once
#include "MediaAudio.h"
#include "smf_codec_ape.h"
namespace smf {
	class MediaApe :public MediaAudio, public smf_media_ape_def_t {
	public:
		MediaApe();
	protected:
		//virtual bool setX(uint32_t keys, void* vals)override;
		//virtual bool getX(uint32_t keys, void* vals) const override;
	};
}