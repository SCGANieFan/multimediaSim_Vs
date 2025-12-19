#include <string.h>
#include <stdlib.h>
#include <stdarg.h>
#include <stdio.h>
#include "hal_trace.h"
#include "heap_api.h"
#include "gadf_porting_api.h"

#ifdef  WIN32

//thread
#include <thread>
#include <mutex>
#include <chrono>
using mutex = std::mutex;
static mutex printMtx;
void* GadfThreadStart(const char* name, void* threadParam, void(*func)(void*), void* funcParam, uint32_t stackSize) {
	auto threadId = new std::thread([name, func, funcParam]() {
		func(funcParam);
		});
	threadId->detach();
	return threadId;
}

unsigned int GadfThreadId() {
#if 0
	std::thread::id tid = std::this_thread::get_id();
	size_t tid_hash = std::hash<std::thread::id>{}(tid);
	return (unsigned int)tid_hash;
#else
	return (unsigned int)0;
#endif
}

void* GadfMutexCreate(void* mutex_def) {
	return (void*)new mutex();
}
int GadfMutexWait(void* mutex_id, uint32_t millisec) {
	((mutex*)mutex_id)->lock();
	return 0;
}
int GadfMutexRelease(void* mutex_id) {
	if (!mutex_id)return 0;
	((mutex*)mutex_id)->unlock();
	return 0;
}
int GadfMutexDelete(void* mutex_id) {
	delete (mutex*)mutex_id;
	return 0;
}

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

void sleep_for(uint32_t ms) {
	std::this_thread::sleep_for(std::chrono::milliseconds(ms));
}

void GadfPrint(const char* fmt, ...) {
	static char buf[256];
	va_list ap;
	va_start(ap, fmt);
	vsprintf(buf, fmt, ap);
	va_end(ap);
	TRACE(0, "%s", buf);
}

#else
#include "mutex.h"
#include "thread.h"
#include "SmfPorting.h"
#include "smf_debug.h"
#include <new>
//using namespace smf;
using mutex = smf::mutex;
void* GadfFileOpen(const char* url, const char* mode) {
	return (void*)fopen(url, mode);
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

void* GadfMutexCreate(void* mutex_def) {
	return (void*)new mutex();
}
int GadfMutexWait(void* mutex_id, uint32_t millisec) {
	((mutex*)mutex_id)->lock();
	return 0;
}
int GadfMutexRelease(void* mutex_id) {
	if (!mutex_id)return 0;
	((mutex*)mutex_id)->unlock();
	return 0;
}
int GadfMutexDelete(void* mutex_id) {
	delete (mutex*)mutex_id;
	return 0;
}


void* GadfThreadStart(const char* name, void* threadParam, void(*func)(void*), void* funcParam, uint32_t stackSize) {
	smf::thread thr;
	bool ret = thr.Start(name, func, funcParam, stackSize);
	if (!ret) return 0;
	thr.detach();
	return 0;
}

unsigned int GadfThreadId() {
	return smf::get_thread_idx();
}

void sleep_for(uint32_t ms) {
	smf::sleep_for(ms);
}

void GadfPrint(const char* fmt, ...) {
	static uint8_t printMtx[(sizeof(mutex) + 7) >> 3 << 3];
	static bool isPrintMtxInit = false;
	if (!isPrintMtxInit) {
		isPrintMtxInit = true;
		new(printMtx) mutex();
	}
	GadfMutexWait(printMtx, 0xffffffff);
	static char buf[256];
	va_list ap;
	va_start(ap, fmt);
	vsprintf(buf, fmt, ap);
	va_end(ap);
	TRACE(0, "%s", buf);
	GadfMutexRelease(printMtx);
}
#endif



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

