#pragma once
#include "gasf.h"
#include "ogg.h"
#include "ogg_common.h"

using namespace GASF_NAME_SPACE;

namespace ogg_ns {
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
		uint8_t* body;
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
		OGG_CODEC_TYPE_OPUS = 0,
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
		uint32_t bodyLenRem;
		uint8_t* packet[8];
		uint32_t packetLen[8];
		uint32_t packetNum;
		//uint8_t *_tag;
		//uint8_t _tagLen;
		uint8_t* _vendorString;
		uint32_t _vendorStringLen;
		uint32_t _commentListLength;
		//uint32_t _commentListNow;
		uint8_t* _title;
		uint32_t _titleLen;
		uint8_t* _artist;
		uint32_t _artistLen;
		uint8_t* _album;
		uint8_t _albumLen;
	};

	struct OggDataPage_t {
		OggPage2_t page;
		uint32_t packByte;
		uint32_t packNum;
		uint32_t bodyLenRem;
		int16_t segmentIndex;
		uint8_t packIndex;
	};

	class OggDemuxer_c :public Gasf_c
	{
		using Base_c = Gasf_c;
		enum class Stage_e {
			STAGE_ID_PAGE_HEAD = 0,
			STAGE_ID_PAGE_BODY,
			STAGE_USER_COMMENT_HEAD,
			STAGE_USER_COMMENT_BODY,
			STAGE_DATA_FIRST_HEAD,
			STAGE_DATA_HEAD,
			STAGE_DATA_BODY,
			STAGE_EOS,
		};
	public:
		OggDemuxer_c();
		virtual ~OggDemuxer_c();
	public:
		virtual OggRet_t Open()override;
		virtual OggRet_t Set(uint32_t key, void* val)override;
		virtual OggRet_t Get(uint32_t key, void* val)override;
		//virtual OggRet_t Run(GasfData_c& iData, GasfData_c& oData)override;
		virtual OggRet_t Receive(GasfData_c& iData)override;
		virtual OggRet_t Generate(GasfData_c& oData)override;
		virtual OggRet_t Close()override;
	protected:
		bool Str2Low(uint8_t* str, uint32_t strLen);
		bool StrLen(uint8_t* str, uint32_t* strLen, uint32_t searchLenMax = 4096);
		bool SyncString(uint8_t* searchStr, uint32_t searchStrLen, uint8_t* syncStr, uint8_t** oStr = 0, uint32_t* oStrLen = 0);
		bool SyncString(uint8_t* searchStr, uint32_t searchStrLen, uint8_t* syncStr, uint32_t syncStrLen, uint8_t** oStr = 0, uint32_t* oStrLen = 0);
		OggRet_t DemuxIdPageHead(GasfData_c& oData);
		OggRet_t DemuxIdPageBody(GasfData_c& oData);
		OggRet_t DemuxUserCommentHead(GasfData_c& oData);
		OggRet_t DemuxUserCommentBody(GasfData_c& oData);
		OggRet_t DemuxDataHead(GasfData_c& oData);
		OggRet_t DemuxDataBody(GasfData_c& oData);
		OggRet_t DemuxEos(GasfData_c& oData);
	public:
	public:
		ogg_memory_t _memory;
		Stage_e _stage = Stage_e::STAGE_ID_PAGE_HEAD;
		uint8_t _headByteCom = 27;
		GasfData_c _iCache;
		OggIdPage_t _idPage;
		OggUserCommentPage_t _userPage;
		OggDataPage_t _dataPage;
		uint32_t _rate = 0;
		uint32_t _ch = 0;
		const char* _syncStr = "OggS";
		uint32_t _syncStrLen = 4;
	};
}
