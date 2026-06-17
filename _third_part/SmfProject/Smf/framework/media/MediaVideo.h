#pragma once
#include "IMedia.h"
#include "smf_codec_yuv.h"
#include "smf_codec_rgb.h"
namespace smf {
	class MediaVideo:public IMedia,public smf_media_video_def_t {
	public:
		MediaVideo();
	protected:
		virtual bool setX(uint32_t keys, void* vals)override;
		virtual bool getX(uint32_t keys, void* vals) const override;
	public:
		virtual char* Print(char* ptr, char* end) const override;
	};

	class MediaVideoRaw :public MediaVideo, public smf_media_video_raw_def_t {
	public:
		MediaVideoRaw();
	protected:
		virtual bool setX(uint32_t keys, void* vals)override;
		virtual bool getX(uint32_t keys, void* vals) const override;
	public:
		uint32_t FrameSize()const;
		static uint32_t FrameSize(uint16_t w, uint16_t h, uint8_t fmt);
		virtual char* Print(char* ptr, char* end) const override;
	};

	class MediaYuv : public MediaVideoRaw, public smf_media_yuv_def_t {
	public:
		MediaYuv();
	protected:
		virtual bool setX(uint32_t keys, void* vals)override;
		virtual bool getX(uint32_t keys, void* vals) const override;
	};

	class MediaRgb : public MediaVideoRaw,public smf_media_rgb_def_t {
	public:
		MediaRgb();
	protected:
		virtual bool setX(uint32_t keys, void* vals)override;
		virtual bool getX(uint32_t keys, void* vals) const override;
	public:
		virtual char* Print(char* ptr, char* end) const override;
	};
}