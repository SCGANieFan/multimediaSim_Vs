#pragma once
#include "MediaAudio.h"
#include "SmfFCC.h"
#include "smf_codec_ldac.h"
namespace smf {	
	class MediaLdac :public MediaAudio, public smf_media_ldac_def_t {
	public:
		MediaLdac() {
			_codec = fcc32("ldac");
			_size = sizeof(MediaLdac);
			_cm = 0;
			_eqmid = 0;
			_fmt = 0;
		}
	protected:
		//virtual bool setX(uint32_t keys, void* vals)override;
		//virtual bool getX(uint32_t keys, void* vals) const override;
	};

}