#pragma once
#include "MediaAudio.h"
#include "smf_codec_flac.h"
#include "SmfFCC.h"
namespace smf {	
	class MediaFlac :public MediaAudio,public smf_media_flac_def_t {
	public:
		MediaFlac() { 
			_codec = fcc32("flac");
			_size = sizeof(MediaFlac);
			_blkSampMin = 0;
			_blkSampMax = 0;
			_frmSizeMin = 0;
			_frmSizeMax = 0;
			_totalSamples = 0;
			_duration = 0;
			_sampleBits = 0;
		}
	public:
		//uint8_t _sampleBits = 0;
		//uint32_t _blkSampMin = 0;
		//uint32_t _blkSampMax = 0;
		//uint32_t _frmSizeMin = 0;
		//uint32_t _frmSizeMax = 0;
		//uint64_t _totalSamples = 0;
		//uint32_t _duration = 0;
	protected:
		//virtual bool setX(uint32_t keys, void* vals)override;
		//virtual bool getX(uint32_t keys, void* vals) const override;
	public:
		virtual uint32_t FrameMax() const override { return _frmSizeMax; }
		virtual uint32_t FrameDurationUs()const override { return 0; }
	};
}