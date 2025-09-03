#pragma once
#include "OggBase.h"
#include "OggMuxer_c.h"
namespace ogg_ns {

class OggMuxerApi_c
{
public:
	OggMuxerApi_c() {}
	~OggMuxerApi_c() {}
public:
	static OggRet_t Create(OggMuxerApiParam_t* oggMuxerApiParam, void** pHd);
	static OggRet_t Receive(void* hd, uint8_t* buf, int32_t len);
	static OggRet_t Generate(void* hd, OggPage_t* page);
	static OggRet_t Set(void* hd, OggMuxerApiSet_e choose, void* val);
	static OggRet_t Get(void* hd, OggMuxerApiGet_e choose, void* val);
	static OggRet_t Destory(void* hd);
public:
	OggBasePorting_t _basePorting;
	OggMuxer_c _oggMuxer;
};
};

