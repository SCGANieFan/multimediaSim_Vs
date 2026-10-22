#pragma once
#include "MediaAudio.h"
#include "smf_codec_g711.h"
#include "SmfFCC.h"
namespace smf {
	class MediaG711 :public MediaAudio, public smf_media_g711_def_t {
	public:
		MediaG711() { 
			_codec = fcc32("g711");
			_size = sizeof(MediaG711);
		}
	protected:
		//virtual bool setX(uint32_t keys, void* vals)override;
		//virtual bool getX(uint32_t keys, void* vals) const override;
	public:
	};
}