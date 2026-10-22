#pragma once
#include "MediaAudio.h"
#include "smf_codec_opus.h"
namespace smf {	
	class MediaOpus :public MediaAudio, public smf_media_opus_def_t {
	public:
		MediaOpus();
	protected:
		virtual bool setX(uint32_t keys, void* vals)override;
		//virtual bool getX(uint32_t keys, void* vals) const override;
	};

}