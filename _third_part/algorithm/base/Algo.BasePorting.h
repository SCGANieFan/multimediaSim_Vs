#pragma once

#include<stdint.h>
typedef void* (*ALGO_Malloc_t)(int32_t size);
typedef void* (*ALGO_Realloc_t)(void* block, int32_t size);
typedef void* (*ALGO_Calloc_t)(int32_t count, int32_t size);
typedef void  (*ALGO_Free_t)(void* block);

typedef struct {
	void* (*Malloc)(int32_t size);
	void* (*Realloc)(void* block, int32_t size);
	void* (*Calloc)(int32_t count, int32_t size);
	void  (*Free)(void* block);
	void (*print_cb)(const char* fmt, ...);
}AlgoBasePorting_t;


class AlgoBasePorting_c {
public:
	AlgoBasePorting_c() {}
	~AlgoBasePorting_c() {}
public:
	virtual void* Malloc(int32_t size) = 0;
	virtual void Free(void* block) = 0;
public:
	void (*print_cb)(const char* fmt, ...);
};






