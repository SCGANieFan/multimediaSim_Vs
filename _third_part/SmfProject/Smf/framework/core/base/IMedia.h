#pragma once
#include "smf_common.h"
#include "smf_media_def.h"
#include "ObjectStatic.h"
#include "SmfHash.h"
namespace smf {
	class Element;
	class IMedia :public ObjectStatic,public smf_media32_def_t {
	public:
		IMedia();
		virtual ~IMedia();
	public:
		bool Is(const char*codec)const;
		bool Is(uint64_t codec)const;
		bool IsAudio()const;
		bool IsVideo()const;
		bool IsOther()const;
		bool IsError()const;
		void SetError();
		void InitMedia(Element*ele);
		uint64_t Codec()const;
		const char* CodecString()const;
		void Codec(uint64_t codec);
		void Codec(const char* codec);
		uint64_t CodecType()const;
		uint32_t CodecTypeHash()const;
		bool FromMedia(IMedia*, bool check = true);
		bool FromMedia(smf_media_def_t*, bool check = true);
		smf_media_def_t* ToMedia()const;
		IMedia* Clone() const;
		bool UpdateVTable();
		void MediaChange(bool);
		bool MediaChanged()const;
	public:
		bool SetX(const char* keys, void* vals);
		bool SetX(uint32_t keys, void* vals);
		bool GetX(const char* keys, void* vals) const;
		bool GetX(uint32_t keys, void* vals) const;
		bool SetX(const char* keys, int vals) { return SetX(keys, (void*)vals); }
		bool SetX(uint32_t keys, int vals) { return SetX(keys, (void*)vals); }
		bool GetX(const char* keys, int vals) const { return GetX(keys, (void*)vals); }
		bool GetX(uint32_t keys, int vals) const { return GetX(keys, (void*)vals); }
	protected:
		virtual bool setX(uint32_t keys, void* vals);
		virtual bool getX(uint32_t keys, void* vals) const;
	public:
		virtual uint32_t FrameMax() const { return 0; }
		virtual uint32_t FrameDurationUs()const { return 0; }
		virtual smf_fraction_t BytePerSecond()const { return { (int)_bitrate << 3,1 }; }
		virtual uint32_t SizeAlign() const { return 0; }
		virtual bool SetExtraData(void* data, uint32_t size);
		virtual bool GetExtraData(void*& data, uint32_t& size);
	public:
		virtual char* Print(char* ptr, char* end) const;
	public:
		uint32_t Bitrate()const;
		uint32_t FrameDurationMs()const { return FrameDurationUs() / 1000; }
	public:
		uint64_t convertByteToUs(uint32_t size)const;
		uint32_t convertByteToMs(uint32_t size)const { return (uint32_t)(convertByteToUs(size) / 1000); }
		uint32_t convertUsToByte(uint64_t us)const;
	public:
		static IMedia* Create(const char* codec) { return Create(Hash(codec)); }
		static IMedia* Create(uint64_t codec) { return Create(Hash(codec)); }
		static IMedia* Create(uint32_t codec);
		static IMedia* Create(smf_media_def_t*);
		static IMedia* CreateScript(const char* script);
		static bool IsAudio(uint64_t);
		static bool IsVideo(uint64_t);
		static bool IsOther(uint64_t);
		
		static uint64_t CodecType(uint64_t codec) { return CodecType(Hash(codec)); }
		static uint64_t CodecType(uint32_t codec);
		static uint64_t CodecType(const char* codec) { return CodecType(Hash(codec)); }
		
		static uint32_t CodecTypeHash(uint64_t codec) { return CodecTypeHash(Hash(codec)); }
		static uint32_t CodecTypeHash(uint32_t codec);
		static uint32_t CodecTypeHash(const char* codec) { return CodecTypeHash(Hash(codec)); }
	};

//#define DefineFromMedia(x) x=media->x
}