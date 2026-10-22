#pragma once
#include "MediaAudio.h"
#include "smf_codec_aac.h"

namespace smf {
	class MediaAac :public MediaAudio, public smf_media_aac_def_t {
	public:
		MediaAac();
	protected:
		virtual bool setX(uint32_t keys, void* vals)override;
		virtual bool getX(uint32_t keys, void* vals) const override;
	public:
		virtual bool SetExtraData(void* data, uint32_t size) override;
		virtual bool GetExtraData(void*& data, uint32_t& size) override;
	public:
		uint8_t RateIndex()const;
		bool ParseASC();
	public:
		virtual uint32_t FrameMax() const override { return 1500; }
	};
}