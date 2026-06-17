#pragma once
#include "IMedia.h"
#include "SmfFCC.h"
#include "smf_codec_pcm.h"
namespace smf {
	class MediaAudio:public IMedia, public smf_media_audio_def_t {
	public:
		MediaAudio();
	protected:
		virtual bool setX(uint32_t keys, void* vals)override;
		virtual bool getX(uint32_t keys, void* vals) const override;
	public:
		virtual char* Print(char* ptr, char* end) const override;
	public:
		virtual uint32_t FrameDurationUs()const override;
	public:
		bool FromEncode(smf_media_audio_encode_t*);
	};
	
	class MediaPcm :public MediaAudio, public smf_media_pcm_def_t {
	public:
		MediaPcm();
	protected:
		virtual bool setX(uint32_t keys, void* vals)override;
		virtual bool getX(uint32_t keys, void* vals) const override;
	public:
		virtual bool SetExtraData(void* data, uint32_t size) override;
		virtual bool GetExtraData(void*& data, uint32_t& size) override;
	public:
		virtual char* Print(char* ptr, char* end) const override;
		virtual smf_fraction_t BytePerSecond()const override;
		virtual uint32_t SizeAlign()const override;
	public:
		uint32_t Bitrate()const { return  _rate * _channels * _sampleWidth << 3; }
		uint32_t Byterate()const {return  _rate * _channels * _sampleWidth;}
	};
}