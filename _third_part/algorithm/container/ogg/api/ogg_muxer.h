#pragma once
#include "gasf.h"
#include "ogg.h"
#include "ogg_common.h"



namespace ogg_ns {

enum class OggMuxerApiMode_e {
	OGG_MUXER_API_MODE_NONONE = 0,
	OGG_MUXER_API_MODE_OPUS,
	OGG_MUXER_API_MODE_MAX,
};

typedef struct {
	uint8_t capture_pattern[4];//"OggS"
	uint8_t streamStructureVersion;
	uint8_t headerTypeFlag;//0x00, new packet; 0x01, the same paget of last packet;0x02 bos; 0x04 eos
	uint8_t granulePosition[8];
	uint8_t serialNumber[4];
	uint8_t pageNumber[4];
	uint8_t crcCbecksum[4];
	uint8_t numberPageSegments;
	//segmentTable
}OggPageHead_t;

typedef struct {
	uint8_t version;
	uint8_t channel;
	uint16_t preSkip;
	uint32_t sampleRate;
	uint16_t outPutGain;//Q7.8 in db
}OggMuxerApiIdParam_t;

typedef struct {
	const char* vendorString;
	int32_t vendorStringLen;
	const char* userCommentString;
	int32_t userCommentStringLen;
}OggMuxerApiUserComment_t;

typedef struct {
	OggPageHead_t* headData;
	int32_t headLen;
	uint8_t* bodyData;
	int32_t bodyLen;
}OggPage_t;

typedef struct
{
	void* (*malloc_cb)(uint32_t);
	void* (*realloc_cb)(void*, uint32_t);
	void (*free_cb)(void*);
	void (*printf_cb)(const char*, ...);
	uint64_t mode;
	OggMuxerApiIdParam_t idParam;
	OggMuxerApiUserComment_t userComment;
	uint32_t page_byte_round;
}OggMuxerApiParam_t;


class OggMuxer_c :public OggBase_c
{
public:
	enum class Stage_e {
		STAGE_NONE = 0,
		STAGE_ID_HEAD,
		STAGE_USER_COMMENT_HEAD,
		STAGE_DATA_HEAD,
	};
	enum class DataFlag_e : uint32_t{
		DATA_FLAG_NONE = OggData_c::CustReserve0,
		DATA_FLAG_GET_DATA_PAGE = OggData_c::CustReserve1,
		DATA_FLAG_MAX = OggData_c::CustReserve2,
	};
public:
	OggMuxer_c();
	virtual ~OggMuxer_c();
public:
	virtual OggRet_t Open()override;
	virtual OggRet_t Set(uint32_t key, void* val)override;
	virtual OggRet_t Get(uint32_t key, void* val)override;
	//virtual OggRet_t Run(OggData_c& iData, OggData_c& oData)override;
	virtual OggRet_t Receive(OggData_c& iData)override;
	virtual OggRet_t Generate(OggData_c& oData)override;
	virtual OggRet_t Close()override;
public:
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
	OggMuxerApiParam_t _param;

	bool _isBos = true;
	bool _isEos = false;
	bool _isReceiveEnd = false;
	bool _isGenrateEnd = false;

};



}
