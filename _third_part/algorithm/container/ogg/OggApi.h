#pragma once

#include"Ogg.h"

#ifndef EXTERNC
#ifdef __cplusplus
#define EXTERNC extern "C" 
#else
#define EXTERNC
#endif
#endif

#define OGG_RET_SUCCESS 0
#define OGG_RET_FAIL -1
#define OGG_RET_MORE_DATA -2
#define OGG_RET_GENERATE_NOT_FINISH -3
#define OGG_RET_OUT_BUFF_NOT_ENOUGH -4
#define OGG_RET_INPUT_FAIL -5
#define OGG_RET_INNER_ERROR -6
#define OGG_RET_FINISH -7

typedef int32_t OggRet_t;

enum class OggMuxerApiSet_e {
	OGG_MUXER_API_SET_NONONE = 0,
	OGG_MUXER_API_SET_IS_EOS,
	OGG_MUXER_API_SET_MAX,
};

enum class OggMuxerApiGet_e {
	OGG_MUXER_API_GET_NONONE = 0,
	OGG_MUXER_API_GET_DATA_PAGE,
	OGG_MUXER_API_GET_MAX,
};


enum class OggMuxerApiMode_e {
	OGG_MUXER_API_MODE_NONONE = 0,
	OGG_MUXER_API_MODE_OPUS,
	OGG_MUXER_API_MODE_MAX,
};

typedef struct {
	uint8_t version;
	uint8_t channel;
	uint16_t preSkip;
	uint32_t sampleRate;
	uint16_t outPutGain;//Q7.8 in db
}OggMuxerApiIdParam_t;

typedef struct{
	uint8_t* vendorString;
	int32_t vendorStringLen;
	uint8_t* userCommentString;
	int32_t userCommentStringLen;
}OggMuxerApiUserComment_t;

typedef struct
{
	void* (*malloc_cb)(int32_t);
	void* (*calloc_cb)(int32_t, int32_t);
	void* (*realloc_cb)(void *, int32_t);
	void (*free_cb)(void*);
	void (*printf_cb)(const char*, ...);
	OggMuxerApiMode_e mode;
	OggMuxerApiIdParam_t idParam;
	OggMuxerApiUserComment_t userComment;
}OggMuxerApiParam_t;

enum class OggDeMuxerApiSet_e {
	OGG_DEMUXER_API_SET_NONONE = 0,
	OGG_DEMUXER_API_SET_MAX,
};

enum class OggDeMuxerApiGet_e {
	OGG_DEMUXER_API_GET_NONONE = 0,
	OGG_DEMUXER_API_GET_RECEIVE_INFO,
	OGG_DEMUXER_API_GET_HAS_ID_PAGE,
	OGG_DEMUXER_API_GET_ID_PAGE,
	OGG_DEMUXER_API_GET_HAS_USER_PAGE,
	OGG_DEMUXER_API_GET_USER_PAGE,
	OGG_DEMUXER_API_GET_MAX,
};

enum class OggDeMuxerApiMode_e {
	OGG_MUXER_API_MODE_NONONE = 0,
	OGG_MUXER_API_MODE_OPUS,
	OGG_MUXER_API_MODE_MAX,
};

typedef struct
{
	void* (*malloc_cb)(int32_t size);
	void* (*calloc_cb)(int32_t, int32_t);
	void* (*realloc_cb)(void*, int32_t);
	void (*free_cb)(void* ptr);
	void (*printf_cb)(const char* fmt, ...);
}OggDeMuxerApiParam_t;


typedef struct
{
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


EXTERNC OggRet_t OggMuxerApiCreate(OggMuxerApiParam_t* oggMuxerApiParam, void** pHd);
EXTERNC OggRet_t OggMuxerApiReceive(void* hd, uint8_t* buf, int32_t len);
EXTERNC OggRet_t OggMuxerApiGenerate(void* hd);
EXTERNC OggRet_t OggMuxerApiSet(void* hd, OggMuxerApiSet_e choose, void* val);
EXTERNC OggRet_t OggMuxerApiGet(void* hd, OggMuxerApiGet_e choose, void* val);
EXTERNC OggRet_t OggMuxerApiDestory(void* hd);



EXTERNC OggRet_t OggDeMuxerApiCreate(OggDeMuxerApiParam_t* oggDeMuxerApiParam, void** pHd);
EXTERNC OggRet_t OggDeMuxerApiReceive(void* hd, int32_t len);
EXTERNC OggRet_t OggDeMuxerApiGenerate(void* hd, uint8_t* buf, int32_t* len);
EXTERNC OggRet_t OggDeMuxerApiSet(void* hd, OggDeMuxerApiSet_e choose, void* val);
EXTERNC OggRet_t OggDeMuxerApiGet(void* hd, OggDeMuxerApiGet_e choose, void* val);
EXTERNC OggRet_t OggDeMuxerApiDestory(void* hd);


