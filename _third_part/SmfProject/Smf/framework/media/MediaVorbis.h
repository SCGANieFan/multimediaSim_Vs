#pragma once
#include "MediaAudio.h"
#include "smf_codec_vorbis.h"
namespace smf {	
	class MediaVorbis :public MediaAudio, public smf_media_vorbis_def_t {
	public:
		MediaVorbis();
	protected:
		//virtual bool setX(uint32_t keys, void* vals)override;
		//virtual bool getX(uint32_t keys, void* vals) const override;
	};
}