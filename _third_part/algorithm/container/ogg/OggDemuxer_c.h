#pragma once
#include "OggBase.h"
#include "ogg.h"
#include "OggMemoryManger_c.h"
namespace ogg_ns {
class OggDeMuxer_c
{
public:
	enum class Stage_e {
		STAGE_ID_HEAD = 0,
		STAGE_USER_COMMENT_HEAD1,
		STAGE_USER_COMMENT_HEAD2,
		STAGE_AUDIO_DATA,
		STAGE_EOS,
	};
public:
	OggDeMuxer_c() {}
	~OggDeMuxer_c() {}
public:
	virtual OggRet_t Init(OggBasePorting_t *basePort);
	virtual OggRet_t Receive(int32_t len);
	virtual OggRet_t Generate(uint8_t* buf, int32_t *len);
	virtual OggRet_t Set(OggDeMuxerApiSet_e choose, void* val);
	virtual OggRet_t Get(OggDeMuxerApiGet_e choose, void* val);
	virtual OggRet_t DeInit();
public:
	OggMemoryManger_c _MM;
	void (*_printf_cb)(const char*, ...);
	ogg_sync_state   _oggSyncS;
	ogg_page         _oggPage;
	ogg_stream_state _oggStreamS;
	uint8_t _idMagic[8];
	OggMuxerApiIdParam_t _idParam;
	Stage_e _stage = Stage_e::STAGE_ID_HEAD;
	uint8_t _idPageCache[512];
	OggPage_t _idPage;
	uint8_t _userPageCache[512];
	OggPage_t _userPage;
	uint8_t _audioDataHeadCache[512];
	OggPageHead_t *_audioDataHead;
	int32_t _oPacketNum = 0;
	int32_t _oPacketNumMax = 0;
	uint8_t _oBufCache[512];
	ogg_packet _oggPacketOld;
	OggDeMuxerApiReceiveInfo_t _receiveInfo;
	bool _GenerateFinish = true;
	bool _isLastPackNotComplete = false;
};
};



