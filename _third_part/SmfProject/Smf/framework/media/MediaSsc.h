#pragma once
#include "MediaAudio.h"
#include "smf_codec_ssc.h"
namespace smf {	
	class MediaSsc :public MediaAudio, public smf_media_ssc_def_t {
	public:
		MediaSsc();
	protected:
		//virtual bool setX(uint32_t keys, void* vals)override;
		//virtual bool getX(uint32_t keys, void* vals) const override;
	};
}