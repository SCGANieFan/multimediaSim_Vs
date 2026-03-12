#pragma once
#include <stdint.h>
#include <stddef.h>

#ifndef EXTERNC
#ifdef __cplusplus
#define EXTERNC extern "C" 
#else
#define EXTERNC
#endif
#endif

#define OGG_API_RET_SUCCESS              (0)
#define OGG_API_RET_FAIL                 (-1)
#define OGG_API_RET_MORE_DATA            (-2)
#define OGG_API_RET_GENERATE_NOT_FINISH  (-3)
#define OGG_API_RET_OUT_BUFF_NOT_ENOUGH  (-4)
#define OGG_API_RET_INPUT_FAIL           (-5)
#define OGG_API_RET_INNER_ERROR          (-6)
#define OGG_API_RET_FINISH               (-7)
#define OGG_API_RET_NOT_SUPPORT          (-8)
#define OGG_API_RET_INCOMPLETE           (-9)

typedef int32_t OggApiRet_t;

typedef struct {
	void* (*malloc_cb)(uint32_t);
	void* (*realloc_cb)(void*, uint32_t);
	void (*free_cb)(void*);
	void (*printf_cb)(const char*, uint32_t);
}OggApiBasePort_t;

typedef struct {
	uint8_t* headData;
	int32_t headByte;
	uint8_t* bodyData;
	int32_t bodyByte;
}OggApiPage_t;

EXTERNC{
void* ogg_api_muxer_create(OggApiBasePort_t* bp);
OggApiRet_t ogg_api_muxer_open(void* id);
OggApiRet_t ogg_api_muxer_set(void* id, const char* choose, void* val);
OggApiRet_t ogg_api_muxer_get(void* id, const char* choose, void* val);
OggApiRet_t ogg_api_muxer_receive(void* id, uint8_t* buf, int32_t* bufByte);
//OggApiRet_t ogg_api_muxer_generate(void* id, uint8_t* buf, int32_t* bufByte);
OggApiRet_t ogg_api_muxer_generate(void* id, OggApiPage_t* page);
OggApiRet_t ogg_api_muxer_close(void* id);
OggApiRet_t ogg_api_muxer_destory(void* id);


void* ogg_api_demuxer_create(OggApiBasePort_t* bp);
OggApiRet_t ogg_api_demuxer_open(void* id);
OggApiRet_t ogg_api_demuxer_receive(void* id, uint8_t* buf, int32_t* len);
OggApiRet_t ogg_api_demuxer_generate(void* id, uint8_t* buf, int32_t* len);
OggApiRet_t ogg_api_demuxer_set(void* id, const char* choose, void* val);
OggApiRet_t ogg_api_demuxer_get(void* id, const char* choose, void* val);
OggApiRet_t ogg_api_demuxer_close(void* id);
OggApiRet_t ogg_api_demuxer_destory(void* id);


}


