#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#ifndef EXTERNC
#ifndef __cplusplus
#define EXTERNC
#else
#define EXTERNC extern "C"
#endif
#endif

EXTERNC void* Mp4Malloc(uint32_t size) {
	return malloc(size);
}
EXTERNC void* Mp4Realloc(void* ptr, uint32_t size) {
	return realloc(ptr,size);
}
EXTERNC void* Mp4Calloc(uint32_t num, uint32_t size) {
	return calloc(num, size);
}
EXTERNC void Mp4Free(void* ptr) {
	free(ptr);
}
