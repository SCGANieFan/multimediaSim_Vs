#pragma once
#include "ogg.h"
#include "OggBase.h"
#include "OggMemoryManger_c.h"
namespace ogg_ns {

class OggMuxer_c
{
	enum class Stage_e{
		STAGE_NONE=0,
		STAGE_ID_HEAD,
		STAGE_USER_COMMENT_HEAD,
		STAGE_DATA_HEAD,
	};
public:
	OggMuxer_c() {}
	virtual ~OggMuxer_c() {}
public:
	virtual OggRet_t Init(OggMuxerApiParam_t* param, OggBasePorting_t *basePorting);
	virtual OggRet_t Receive(uint8_t* buf, int32_t len);
	virtual OggRet_t Generate(OggPage_t* page);
	virtual OggRet_t Set(OggMuxerApiSet_e choose, void* val);
	virtual OggRet_t Get(OggMuxerApiGet_e choose, void* val);
	virtual OggRet_t DeInit();

public:
	OggMemoryManger_c _MM;
	ogg_sync_state   _oggSyncS;
	ogg_page         _oggPage;
	ogg_stream_state _oggStreamS;
	ogg_packet       _oggPacket;
	uint8_t _idPageCache[512];
	OggPage_t _idPage;
	uint8_t _userCommentPageCache[512];
	OggPage_t _userCommentPage;
	Stage_e _stage = Stage_e::STAGE_NONE;
	int32_t _page_out_fill_byte = 4096;
	int32_t _byte_per_sample = 0;
	int32_t _acc_byte = 0;
	int32_t _granulepos = 0;
	int32_t _packetno = 0;
	
	bool _isBos = true;
	bool _isEos = false;
	bool _isReceiveEnd = false;
	bool _isGenrateEnd = false;
};
};

