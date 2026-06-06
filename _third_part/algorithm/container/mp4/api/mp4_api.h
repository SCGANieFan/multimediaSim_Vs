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

#define MP4_API_RET_SUCCESS              (0)
#define MP4_API_RET_FAIL                 (-1)

typedef int32_t Mp4ApiRet_t;

typedef struct {
	void* (*malloc_cb)(uint32_t);
	void* (*realloc_cb)(void*, uint32_t);
	void (*free_cb)(void*);
	void (*printf_cb)(const char*, uint32_t);
}Mp4ApiBasePort_t;

EXTERNC void* mp4_api_demuxer_create(Mp4ApiBasePort_t* bp);
EXTERNC Mp4ApiRet_t mp4_api_demuxer_open(void* hd);
EXTERNC Mp4ApiRet_t mp4_api_demuxer_set(void* hd, const char* choose, void* val);
EXTERNC Mp4ApiRet_t mp4_api_demuxer_get(void* hd, const char* choose, void* val);
EXTERNC // Mp4ApiRet_t mp4_api_demuxer_receive(void* hd, uint8_t* buf, int32_t* bufByte);
EXTERNC Mp4ApiRet_t mp4_api_demuxer_generate(void* hd, uint8_t* buf, int32_t* bufByte);
EXTERNC Mp4ApiRet_t mp4_api_demuxer_close(void* hd);
EXTERNC Mp4ApiRet_t mp4_api_demuxer_destory(void* hd);


