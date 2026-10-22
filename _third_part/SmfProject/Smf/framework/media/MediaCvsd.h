#pragma once
#include "MediaAudio.h"
#include "smf_codec_cvsd.h"
namespace smf {	
	class MediaCvsd :public MediaAudio,  public smf_media_cvsd_def_t {
	public:
		MediaCvsd();
	protected:
		//virtual bool setX(uint32_t keys, void* vals)override;
		//virtual bool getX(uint32_t keys, void* vals) const override;
	};
}