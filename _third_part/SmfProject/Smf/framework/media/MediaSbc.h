#pragma once
#include "MediaAudio.h"
#include "smf_codec_sbc.h"
#include "SmfFCC.h"
namespace smf {	
	class MediaSbc :public MediaAudio,public smf_media_sbc_def_t {
	public:
		MediaSbc();
	protected:
		virtual bool setX(uint32_t keys, void* vals)override;
		//virtual bool getX(uint32_t keys, void* vals) const override;
	public:
		virtual uint32_t FrameMax() const override { return 4096;}
	};

	class MediaMsbc :public MediaSbc {
	public:
		MediaMsbc();
	};
}