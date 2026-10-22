#pragma once
#include "MediaAudio.h"
#include "smf_codec_silk.h"
namespace smf {	
	class MediaSilk :public MediaAudio, public smf_media_silk_def_t {
	public:
		MediaSilk();
	protected:
		//virtual bool setX(uint32_t keys, void* vals)override;
		//virtual bool getX(uint32_t keys, void* vals) const override;
	};
}