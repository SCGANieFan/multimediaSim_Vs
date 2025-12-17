#pragma once
#include <stdint.h>

//file
#ifdef  WIN32
#define GADF_FILE_SEEK_SET    0
#define GADF_FILE_SEEK_CUR    1
#define GADF_FILE_SEEK_END    2
#else
#define GADF_FILE_SEEK_SET    0
#define GADF_FILE_SEEK_CUR    1
#define GADF_FILE_SEEK_END    2
#endif

void* GadfFileOpen(const char* url, const char* mode);
uint32_t GadfFileWrite(void* hd, void* buf, uint32_t size);
uint32_t GadfFileRead(void* hd, void* buf, uint32_t size);
int32_t GadfFileSeek(void* hd, int32_t offset, uint32_t origin);
int32_t GadfFileTell(void* hd);
void GadfFileClose(void* hd);

//print
void GadfPrint(const char* fmt, ...);

//heap
void* GadfHheapRegister(char* buf, int size);
void* GadfHeapMalloc(void* heap, int size);
void* GadfHheapRealloc(void* heap, void* rmem, int newsize);
void GadfHeapFree(void* heap, void* rmem);

//thread
void* GadfThreadStart(const char* name, void* threadParam, void(*func)(void*), void* funcParam, uint32_t stackSize);
unsigned int GadfThreadId();

//mutex
void* GadfMutexCreate(void* mutex_def);
int GadfMutexWait(void* mutex_id, uint32_t millisec);
int GadfMutexRelease(void* mutex_id);
int GadfMutexDelete(void* mutex_id);
