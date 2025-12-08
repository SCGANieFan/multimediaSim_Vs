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

typedef int32_t OggApiRet_t;

typedef struct {
	void* (*malloc_cb)(uint32_t);
	void* (*realloc_cb)(void*, uint32_t);
	void (*free_cb)(void*);
	void (*printf_cb)(const char*, ...);
}OggApiBasePort_t;

EXTERNC{
uint32_t ogg_api_muxer_create(OggApiBasePort_t* bp);
OggApiRet_t ogg_api_muxer_open(uint32_t id);
OggApiRet_t ogg_api_muxer_set(uint32_t id, const char* choose, void* val);
OggApiRet_t ogg_api_muxer_get(uint32_t id, const char* choose, void* val);
OggApiRet_t ogg_api_muxer_receive(uint32_t id, uint8_t* buf, int32_t* bufByte);
OggApiRet_t ogg_api_muxer_generate(uint32_t id, uint8_t* buf, int32_t* bufByte);
OggApiRet_t ogg_api_muxer_close(uint32_t id);
OggApiRet_t ogg_api_muxer_destory(uint32_t id);


uint32_t ogg_api_demuxer_create(OggApiBasePort_t* bp);
OggApiRet_t ogg_api_demuxer_open(uint32_t id);
OggApiRet_t ogg_api_demuxer_receive(uint32_t id, int32_t len);
OggApiRet_t ogg_api_demuxer_generate(uint32_t id, uint8_t* buf, int32_t* len);
OggApiRet_t ogg_api_demuxer_set(uint32_t id, const char* choose, void* val);
OggApiRet_t ogg_api_demuxer_get(uint32_t id, const char* choose, void* val);
OggApiRet_t ogg_api_demuxer_close(uint32_t id);
OggApiRet_t ogg_api_demuxer_destory(uint32_t id);

void ogg_api_register_ogg_muxer();
void ogg_api_register_ogg_demuxer();

}


