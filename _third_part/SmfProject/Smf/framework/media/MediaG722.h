#pragma once
#include "MediaAudio.h"
#include "smf_codec_g722.h"
#include "SmfFCC.h"
namespace smf {
	class MediaG722 :public MediaAudio ,public smf_media_g722_def_t{
	public:
		MediaG722() {
			_codec = fcc32("g722");
			_size = sizeof(MediaG722);
		}
	protected:
		//virtual bool setX(uint32_t keys, void* vals)override;
		//virtual bool getX(uint32_t keys, void* vals) const override;
	public:
	};
}