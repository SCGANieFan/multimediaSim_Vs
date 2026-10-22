#pragma once
#include <stdbool.h>
#include <stdint.h>
#ifndef EXTERNC
#ifndef __cplusplus
#define EXTERNC
#else
#define EXTERNC extern "C"
#endif
#endif

EXTERNC void* smf_heap_init(void* addr, uint32_t size);
EXTERNC void* smf_heap_malloc(void* heap, uint32_t size, uint32_t align);
EXTERNC void* smf_heap_calloc(void* heap, uint32_t num, uint32_t size);
EXTERNC void* smf_heap_realloc(void* heap, void* ptr, uint32_t size);
EXTERNC void smf_heap_free(void*heap, void* ptr);

