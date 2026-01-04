#pragma once
#include "ogg.h"
#include "ogg_api_private.h"

namespace ogg_api_ns {
using namespace ogg_gaapi_ns;

struct OggHead_t {
	uint32_t magic; //"OggS"
	uint8_t version;  //stream structure version
	uint8_t packetFlag; //continued packet flag
	uint8_t granulePos[8]; //64 bits of PCM position
	uint8_t serialno[4]; //32 bits of stream serial number
	uint8_t pageCounter[4]; //32 bits of page counter
	uint8_t crc[4]; //crc
	uint8_t segmentNumber; //segment number
	uint8_t segment[255];
};

struct OggPage2_t {
	OggHead_t head;
	uint32_t headLen;
	uint8_t *body;
	uint32_t bodyLen;
};

struct IdOpusBody_t
{
	uint8_t magic[8];
	uint8_t version;
	uint8_t channel;
	uint8_t preSkip[2];
	uint8_t sampleRate[4];
	uint8_t outPutGain[2];//Q7.8 in db
	uint8_t channelMappingFamily;
};

struct IdVorbisBody_t
{
	uint8_t magic[7];
	uint8_t version[4];
	uint8_t channel;
	uint8_t sampleRate[4];
	uint8_t bitrateMax[4];
	uint8_t bitrateNominal[4];
	uint8_t bitrateMin[4];
	uint8_t blocksize0;
	uint8_t blocksize1;
	//uint8_t framingFlag;
};

enum OggCodecType_e
{
	OGG_CODEC_TYPE_OPUS=0,
	OGG_CODEC_TYPE_VORBIS,
	OGG_CODEC_TYPE_MAX,
};

struct OggIdPage_t {
	OggPage2_t page;
	union {
		IdOpusBody_t idOpusBody;
		IdVorbisBody_t idVorbisBody;
	};
	OggCodecType_e codecType;
	uint32_t bodyLenRem;
};

struct OggUserCommentPage_t {
	OggPage2_t page;
	uint8_t userBody[4];
	OggCodecType_e codecType;
	uint32_t bodyLenRem;
};

struct OggDataPage_t {
	OggPage2_t page;
	uint32_t packByte;
	uint32_t bodyLenRem;
	int16_t segmentIndex;
	uint8_t packIndex;
};

class OggDeMuxer_c:public GaapiGaf_c{
	using Base_c = GaapiGaf_c;
public:
	enum class Stage_e {
		STAGE_ID_PAGE_HEAD = 0,
		STAGE_ID_PAGE_BODY,
		STAGE_USER_COMMENT_HEAD,
		STAGE_USER_COMMENT_BODY,
		STAGE_DATA_HEAD,
		STAGE_DATA_BODY,
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
protected:
	uint8_t* SyncString(uint8_t* buf, uint32_t len, const char* str);
	OggRet_t DemuxIdPageHead(GaapiData_c& oData);
	OggRet_t DemuxIdPageBody(GaapiData_c& oData);
	OggRet_t DemuxUserCommentHead(GaapiData_c& oData);
	OggRet_t DemuxUserCommentBody(GaapiData_c& oData);
	OggRet_t DemuxDataHead(GaapiData_c& oData);
	OggRet_t DemuxDataBody(GaapiData_c& oData);
	OggRet_t DemuxEos(GaapiData_c& oData);
public:
	ogg_memory_t _memory;
	Stage_e _stage = Stage_e::STAGE_ID_PAGE_HEAD;
	const uint8_t _headByteCom = 27;
	GaapiData_c _iCache;
	OggIdPage_t _idPage2;
	OggUserCommentPage_t _userPage2;
	OggDataPage_t _dataPage2;
	uint32_t _rate = 0;
	uint32_t _ch = 0;
};


};



