#pragma once
#include "ogg_api.h"
#include "gaapi.h"
#include "gaapi_memory.h"
#include "gaapi_gaf.h"

using namespace gaapi_ns;

namespace ogg_api_ns {
#define OGG_VERSION "2.0.0"
#define LOG_OGG(fmt,...)		   LOG_GAAPI(fmt,##__VA_ARGS__)

using OggRet_t = OggApiRet_t;

typedef struct{
	uint8_t* buf;
	int32_t bufMax;
}OggDeMuxerApiReceiveInfo_t;

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
	OggPageHead_t* headData;
	int32_t headLen;
	uint8_t* bodyData;
	int32_t bodyLen;
}OggPage_t;

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

enum class OggMuxerApiMode_e {
	OGG_MUXER_API_MODE_NONONE = 0,
	OGG_MUXER_API_MODE_OPUS,
	OGG_MUXER_API_MODE_MAX,
};

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



class OggDeMuxerApi_c :public Gaapi_c
{
	using Base_c = Gaapi_c;
public:
	OggDeMuxerApi_c();
	virtual ~OggDeMuxerApi_c();
public:
	virtual OggRet_t Open()override;
	virtual OggRet_t Set(const char* choose, void* val)override;
	virtual OggRet_t Get(const char* choose, void* val)override;
	//virtual OggRet_t Run(GaapiData_c& iData, GaapiData_c& oData)override;
	virtual OggRet_t Receive(GaapiData_c& iData)override;
	virtual OggRet_t Generate(GaapiData_c& oData)override;
	virtual OggRet_t Close()override;
public:
	GaapiGaf_c* _oggDeMuxer;
};


class OggMuxerApi_c :public Gaapi_c
{
	using Base_c = Gaapi_c;
public:
	OggMuxerApi_c();
	virtual ~OggMuxerApi_c();
public:
	virtual OggRet_t Open()override;
	virtual OggRet_t Set(const char* choose, void* val)override;
	virtual OggRet_t Get(const char* choose, void* val)override;
	//virtual OggRet_t Run(GaapiData_c& iData, GaapiData_c& oData)override;
	virtual OggRet_t Receive(GaapiData_c& iData)override;
	virtual OggRet_t Generate(GaapiData_c& oData)override;
	virtual OggRet_t Close()override;
public:
	GaapiGaf_c* _oggMuxer = 0;
	OggMuxerApiParam_t _param;
};



}
