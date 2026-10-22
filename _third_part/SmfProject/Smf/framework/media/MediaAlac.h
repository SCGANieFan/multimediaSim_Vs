#pragma once
#include "MediaAudio.h"
#include "smf_codec_alac.h"
namespace smf {
	class MediaAlac
		:public MediaAudio
		,public smf_media_alac_def_t
	{
	public:
		MediaAlac();
	protected:
		//virtual bool setX(uint32_t keys, void* vals)override;
		//virtual bool getX(uint32_t keys, void* vals) const override;
	public:
		//uint32_t _maxSamplePerFrame = 0;
		//uint32_t _maxCodedFrameSize = 0;
		//uint8_t _sampleBits = 16;
	public:
		virtual uint32_t FrameMax() const override { return _maxCodedFrameSize; }
	};
}