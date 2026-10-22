#pragma once
#include "MediaAudio.h"
#include "smf_codec_amr.h"
namespace smf {	
	class MediaAmr :public MediaAudio, public smf_media_amr_def_t {
	public:
		MediaAmr();
		MediaAmr(uint64_t codec);
	public:
		//uint32_t _bandWidth = 0;
		//uint32_t _bitrate = 0;
	protected:
		//virtual bool setX(uint32_t keys, void* vals)override;
		//virtual bool getX(uint32_t keys, void* vals) const override;
	};
}