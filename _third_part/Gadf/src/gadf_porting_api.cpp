#include <string.h>
#include <stdlib.h>
#include <stdarg.h>
#include <stdio.h>
#include "hal_trace.h"
#include "heap_api.h"
#include "gadf_porting_api.h"

#define LOG_GADF_ORI(fmt,...)    TRACE(0, fmt, ##__VA_ARGS__)
#define LOG_GADF(fmt,...)        TRACE(0, "(%s)[%d]" fmt, __func__, __LINE__, ##__VA_ARGS__)


#ifdef  WIN32
void* GadfFileOpen(const char* url, const char* mode) {
	return fopen(url, mode);
}
uint32_t GadfFileWrite(void* hd, void* buf, uint32_t size) {
	return fwrite(buf, 1, size, (FILE*)hd);
}
uint32_t GadfFileRead(void* hd, void* buf, uint32_t size) {
	return fread(buf, 1, size, (FILE*)hd);
}
int32_t GadfFileSeek(void* hd, int32_t offset, uint32_t origin) {
	return fseek((FILE*)hd, offset, origin);
}
int32_t GadfFileTell(void* hd) {
	return ftell((FILE*)hd);
}
void GadfFileClose(void* hd) {
	fclose((FILE*)hd);
}
#else
void* GadfFileOpen(const char* url, const char* mode) {
	return (void*)1;
}
uint32_t GadfFileWrite(void* hd, void* buf, uint32_t size) {
	return size;
}
uint32_t GadfFileRead(void* hd, void* buf, uint32_t size) {
	return 0;
}
int32_t GadfFileSeek(void* hd, int32_t offset, uint32_t origin) {
	return 0;
}
int32_t GadfFileTell(void* hd) {
	return 0;
}
void GadfFileClose(void* hd) {
}
#endif


void GadfPrint(const char* fmt, ...){
	static char buf[256];
	va_list ap;
	va_start(ap, fmt);
	vsprintf(buf, fmt, ap);
	va_end(ap);
	LOG_GADF_ORI("%s", buf);
}


void* GadfHheapRegister(char* buf, int size) {
	return heap_register(buf, size);
}
void* GadfHeapMalloc(void* heap, int size) {
	return heap_malloc((multi_heap_handle_t)heap, size);
}
void* GadfHheapRealloc(void* heap, void* rmem, int newsize) {
	return heap_realloc((multi_heap_handle_t)heap, rmem, newsize);
}
void GadfHeapFree(void* heap, void* rmem) {
	heap_free((multi_heap_handle_t)heap, rmem);
}

//thread
#ifdef  WIN32
#include <thread>
void* GadfThreadStart(const char* name, void* threadParam, void(*func)(void*), void* funcParam, uint32_t stackSize) {
	auto threadId = new std::thread([name, func, funcParam]() {
		func(funcParam);
		});
	return threadId;
}
#else	
void* GadfTh; eadStart(void* threadParam, void(*func)(void* param), void* param) {
	return 0l
}
#endif