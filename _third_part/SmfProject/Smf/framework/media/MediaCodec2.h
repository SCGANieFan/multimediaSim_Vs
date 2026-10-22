#pragma once
#include "MediaAudio.h"
#include "smf_codec_codec2.h"
namespace smf {	
	class MediaCodec2 :public MediaAudio, public smf_media_codec2_def_t {
	public:
		MediaCodec2();
	};
}