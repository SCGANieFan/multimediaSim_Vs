#pragma once

#include<stdint.h>
typedef void* (*ALGO_Malloc_t)(int32_t size);
typedef void* (*ALGO_Realloc_t)(void* block, int32_t size);
typedef void* (*ALGO_Calloc_t)(int32_t count, int32_t size);
typedef void  (*ALGO_Free_t)(void* block);

typedef struct {
	ALGO_Malloc_t Malloc;
	ALGO_Realloc_t Realloc;
	ALGO_Calloc_t Calloc;
	ALGO_Free_t Free;
}AlgoBasePorting;








