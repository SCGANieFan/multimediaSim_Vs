#include "IMedia.h"
#include "SmfLayout.h"
#include "smf_debug.h"
#include "SmfFCC.h"
#include "SmfHash.h"
#include "MediaAudio.h"
#include "MediaAac.h"
#include "MediaAlac.h"
#include "MediaAmr.h"
#include "MediaApe.h"
#include "MediaCvsd.h"
#include "MediaFlac.h"
#include "MediaLdac.h"
#include "MediaMp3.h"
#include "MediaLc3.h"
#include "MediaOpus.h"
#include "MediaSbc.h"
#include "MediaSilk.h"
#include "MediaSsc.h"
#include "MediaVorbis.h"
#include "MediaG711.h"
#include "MediaG722.h"
#include "MediaADPcm.h"
#include "MediaVideo.h"
#include "MediaH264.h"
#include "MediaH265.h"
#include "MediaJpeg.h"
#include "MediaFFmpeg.h"
#include "MediaGif.h"
using namespace smf;

struct pair_t{
	uint32_t keys;
	IMedia* (*create)();
};
template<class T>static IMedia* TCreate() { return new T(); }
template<class T, uint64_t codec>static IMedia* TCreate1() { return new T(codec); }
static constexpr pair_t _pairs[]{
	{0,0},//invalide

	{Hash("other"),&TCreate<IMedia>},
	{Hash("user"),&TCreate<IMedia> },

	{Hash("audio"),&TCreate<MediaAudio>},
	{Hash("pcm"),&TCreate<MediaPcm>},
	{Hash("aac") ,&TCreate<MediaAac>},
	{Hash("alac") ,&TCreate<MediaAlac>},
	{Hash("amrnb") ,&TCreate1<MediaAmr,fcc64("amrnb")>},
	{Hash("amrwb") ,&TCreate1<MediaAmr,fcc64("amrwb")>},
	{Hash("amr") ,&TCreate<MediaAmr>},
	{Hash("ape") ,&TCreate<MediaApe>},
	{Hash("cvsd") ,&TCreate<MediaCvsd>},
	{Hash("flac") ,&TCreate<MediaFlac>},
	{Hash("ldac") ,&TCreate<MediaLdac>},
	{Hash("mp3") ,&TCreate<MediaMp3>},
	{Hash("lc3plus"),&TCreate<MediaLc3>},
	{Hash("lc3") ,&TCreate<MediaLc3>},
	{Hash("opus") ,&TCreate<MediaOpus>},
	{Hash("msbc") ,&TCreate<MediaMsbc>},
	{Hash("sbc") ,&TCreate<MediaSbc>},
	{Hash("silk") ,&TCreate<MediaSilk>},
	{Hash("ssc") ,&TCreate<MediaSsc>},
	{Hash("g711") ,&TCreate<MediaG711>},
	{Hash("g722") ,&TCreate<MediaG722>},
	{Hash("adpcm") ,&TCreate<MediaADPcm>},
	{Hash("vorbis") ,&TCreate<MediaVorbis>},
	
	{Hash("video") ,&TCreate<MediaVideo>},
	{Hash("yuv") ,&TCreate<MediaYuv>},
	{Hash("rgb") ,&TCreate<MediaRgb>},
	{Hash("jpeg") ,&TCreate<MediaJpeg>},
	{Hash("fpeg") ,&TCreate<MediaFFmpeg>},
	{Hash("h264") ,&TCreate<MediaH264>},
	{Hash("h265") ,&TCreate<MediaH265>},
	{Hash("pal8") ,&TCreate<MediaVideo>},
	{Hash("gif") ,&TCreate<MediaGif>},
};
constexpr inline int xFind(uint32_t hash) {
	for (auto& pair : _pairs) {
		if (pair.keys == hash)
			return &pair - _pairs;
	}
	return 0;
}

static constexpr int8_t _aIdx = xFind(Hash("audio"));
static constexpr int8_t _vIdx = xFind(Hash("video"));

IMedia* IMedia::Create(uint32_t codec) {
	auto idx = xFind(codec);
	return idx ? (_pairs[idx].create ? _pairs[idx].create() : 0) : 0;
}
IMedia* IMedia::Create(smf_media_def_t* media) {
	return ((IMedia*)media)->Clone();
}
IMedia* IMedia::CreateScript(const char* script) {
	dbgTestPXL(script);
	Layout layout{ script,false,true,false };
	IMedia* media = 0;
	//dbgTestPXL("%p,%p", &media, media);
	layout.Parse([](void* priv, const char* keys, void* vals, char rst) {
		IMedia*& media = *(IMedia**)priv;
		//dbgTestPXL("%p,%p,%s=%p", priv, media, keys, vals);
		auto hash = Hash(keys);
		switch (hash) {
		case Hash("codec"):
			media = Create((const char*)vals);
			//dbgTestPXL("%p", media);
			break;
		default:
			returnIfErrC0(!media);
			media->SetX(hash, vals); break;
		}
		}, &media);
	//dbgTestPXL("%p,%p", &media, media);
	return media;
}

bool IMedia::IsAudio(uint64_t codec) {
	return CodecTypeHash(codec) == Hash("audio");
}
bool IMedia::IsVideo(uint64_t codec) {
	return CodecTypeHash(codec) == Hash("video");
}
bool IMedia::IsOther(uint64_t codec) {
	return CodecTypeHash(codec) == Hash("other");
}

uint64_t IMedia::CodecType(uint32_t codec) {
	auto idx = xFind(codec);
	if(idx >= _vIdx)
		return fcc64("video");
	else if (idx >= _aIdx)
		return fcc64("audio");
	else
		return fcc64("other");
}
uint32_t IMedia::CodecTypeHash(uint32_t codec) {
	auto idx = xFind(codec);
	if (idx >= _vIdx)
		return Hash("video");
	else if (idx >= _aIdx)
		return Hash("audio");
	else
		return Hash("other");
}
