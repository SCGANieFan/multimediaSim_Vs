#pragma once
#include "OggBase.h"
#include "OggDemuxer_c.h"
namespace ogg_ns {
	class OggDeMuxerApi_c
	{
	public:
		OggDeMuxerApi_c() {}
		~OggDeMuxerApi_c() {}
	public:
		static OggRet_t Create(OggDeMuxerApiParam_t* oggDeMuxerApiParam, void** pHd);
		static OggRet_t Receive(void* hd, int32_t len);
		static OggRet_t Generate(void* hd, uint8_t* buf, int32_t* len);
		static OggRet_t Set(void* hd, OggDeMuxerApiSet_e choose, void* val);
		static OggRet_t Get(void* hd, OggDeMuxerApiGet_e choose, void* val);
		static OggRet_t Destory(void* hd);
	public:
		OggBasePorting_t _basePorting;
		OggDeMuxer_c _oggDeMuxer;
	};
};