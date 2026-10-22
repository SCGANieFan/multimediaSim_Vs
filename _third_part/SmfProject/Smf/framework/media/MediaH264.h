#pragma once
#include "MediaVideo.h"
#include "smf_codec_h264.h"

namespace smf {
	class MediaH264 :public MediaVideo, public smf_media_h264_def_t {
	public:
		MediaH264();
	protected:
		virtual bool setX(uint32_t keys, void* vals)override;
		virtual bool getX(uint32_t keys, void* vals) const override;
	public:
		virtual bool SetExtraData(void* data, uint32_t size) override;
		virtual bool GetExtraData(void*& data, uint32_t& size) override;
	public:
		bool ParseExtraData();
		bool ParseSPS();
		bool ParsePPS();
		bool JumpToData(uint8_t*& data, int& size);
	public:
		static bool JumpToData(uint8_t*& data, int& size, uint8_t pack);
		static bool JumpToData(uint8_t*& data, int& size, const char* pack);
		static bool JumpToDataAVC(uint8_t*& data, int& size);
		static bool JumpToDataBitStream(uint8_t*& data, int& size);
	};
}