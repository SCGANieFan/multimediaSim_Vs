#pragma once
#include "ogg.h"
#include "gaapi_gaf.h"
#include "ogg_api_private.h"

using namespace ogg_gaapi_ns;
namespace ogg_api_ns {
class OggDeMuxer_c:public GaapiGaf_c{
	using Base_c = GaapiGaf_c;
public:
	enum class Stage_e {
		STAGE_ID_HEAD = 0,
		STAGE_USER_COMMENT_HEAD1,
		STAGE_USER_COMMENT_HEAD2,
		STAGE_AUDIO_DATA,
		STAGE_EOS,
	};
public:
	OggDeMuxer_c();
	virtual ~OggDeMuxer_c();
public:
	virtual OggRet_t Open()override;
	virtual OggRet_t Set(uint32_t key, void* val)override;
	virtual OggRet_t Get(uint32_t key, void* val)override;
	//virtual OggRet_t Run(GaapiData_c& iData, GaapiData_c& oData)override;
	virtual OggRet_t Receive(GaapiData_c& iData)override;
	virtual OggRet_t Generate(GaapiData_c& oData)override;
	virtual OggRet_t Close()override;
public:
	ogg_sync_state   _oggSyncS;
	ogg_page         _oggPage;
	ogg_stream_state _oggStreamS;
	uint8_t _idMagic[8];
	OggMuxerApiIdParam_t _idParam;
	Stage_e _stage = Stage_e::STAGE_ID_HEAD;
	uint8_t _idPageCache[512];
	OggPage_t _idPage;
	//uint8_t _userPageCache[512];
	uint8_t _userPageCache[10*1024];
	OggPage_t _userPage;
	uint8_t _audioDataHeadCache[512];
	OggPageHead_t *_audioDataHead;
	int32_t _oPacketNum = 0;
	int32_t _oPacketNumMax = 0;
	uint8_t _oBufCache[512];
	ogg_packet _oggPacketOld;
	OggDeMuxerApiReceiveInfo_t _receiveInfo;
	ogg_memory_t _memory;
	int64_t _granulepos = 0;
	int64_t _packetno = 0;
	int64_t _pack10MsStep = 0;
	int64_t _t10Ms = 0;
	bool _GenerateFinish = true;
	bool _isLastPackNotComplete = false;
};


};



