#pragma once
#include "MediaAudio.h"
#include "smf_codec_dsd.h"
namespace smf {
	class MediaDsd :public MediaAudio,public smf_media_dsd_def_t {
	public:
		MediaDsd();
	public:
		virtual smf_fraction_t BytePerSecond()const override { return { (int)_rate * _channels >> 3, 1 }; }
		uint32_t Bitrate()const { return BytePerSecond().num << 3; }
	};
}