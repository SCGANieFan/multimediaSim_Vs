#include <string.h>
#include <stdarg.h>
#include <stdio.h>
#include "gadf.h"
#include "gadf_base.h"
#include "gadf_private.h"
#include "gadf_porting_api.h"

#if WIN32
#define LOG_GADF(fmt,...) GadfPrint("<%s>[%s](%d)" fmt "\n", strrchr(__FILE__,'\\') + 1,__func__, __LINE__, ##__VA_ARGS__)
#else
#define LOG_GADF(fmt,...) GadfPrint("<%s>[%s](%d)" fmt "\n", strrchr(__FILE__,'/') + 1,__func__, __LINE__, ##__VA_ARGS__)
#endif

template<class K, class V>
struct KeyValue_t {
	K key;
	V val;
};

template<class V, uint32_t Num>
struct GadfInfoTable_t {
	KeyValue_t<const char*, V> _info[Num];
	uint32_t _readIndex;
	uint32_t _num;
	uint32_t _numMax;
	bool _isFirst;
	GadfInfoTable_t() : _readIndex(0), _num(0), _numMax(Num), _isFirst(true) {}
};

GadfInfoTable_t<Gadf_c*, 20> tables;
static void GadfInfoTableAdd(const char* key, Gadf_c* val) {
	if (key) {
		for (uint32_t n = 0; n < tables._numMax; n++) {
			if (tables._info[n].key && !strcmp(tables._info[n].key, key)) {
				tables._info[n].key = key;
				tables._info[n].val = val;
				tables._num++;
				return;
			}
		}
		for (uint32_t n = 0; n < tables._numMax; n++) {
			if (!tables._info[n].key) {
				tables._info[n].key = key;
				tables._info[n].val = val;
				tables._num++;
				LOG_GADF("%d,%s,%p", n, key, val);
				return;
			}
		}
	}
}

Gadf_c* GadfInfoTableGet() {
	for (uint32_t n = tables._readIndex; n < tables._numMax; n++) {
		if (tables._info[n].key) {
			tables._readIndex = n + 1;
			LOG_GADF("%d,%s,%p", n, tables._info[n].key, tables._info[n].val);
			return tables._info[n].val;
		}
	}
	return 0;
}


bool gadf_register_info(const char* key, Gadf_c* info) {
	if (tables._isFirst) {
		uint32_t numMax = tables._numMax;
		for (uint8_t n = 0; n < numMax; n++) {
			tables._info[n].key = 0;
			tables._info[n].val = 0;
		}
		tables._readIndex = 0;
		tables._num = 0;
		tables._numMax = numMax;
		tables._isFirst = false;
	}
	GadfInfoTableAdd(key, info);
	return true;
}

void* gadf_get_register_info() {
	return GadfInfoTableGet();
}

static char heap_pool[80 * 1024];
static void* heap = 0;

static void* GadfMalloc(int size)
{
	void* buf = GadfHeapMalloc(heap, size);
	LOG_GADF("%d,%p", size, buf);
	return buf;
}

static void* GadfRealloc(void* rmem, int newsize)
{
	void* buf = GadfHheapRealloc(heap, rmem, newsize);
	LOG_GADF("%d,%p,%p", newsize, rmem, buf);
	return buf;
}

static void GadfFree(void* rmem)
{
	LOG_GADF("%p", rmem);
	GadfHeapFree(heap, rmem);
	return;
}

void gadf_run_by_info(void* info) {
	LOG_GADF("v1.0.1, %p", info);
	if (!heap) {
		heap = GadfHheapRegister(heap_pool, sizeof(heap_pool));
	}
	Gadf_c* demo = (Gadf_c*)info;
	Gadf_c::BasePort_t bp;
	bp._malloc = GadfMalloc;
	bp._free = GadfFree;
	bp._print = GadfPrint;
	bool ret = true;
	LOG_GADF("%p,%u,%u,%u,%u", demo, ((uint32_t*)demo)[0], ((uint32_t*)demo)[1], ((uint32_t*)demo)[2], ((uint32_t*)demo)[3]);
	ret = demo->Set("basePort", &bp); if (!ret) return;
	ret = demo->Init(); if (!ret) return;
	demo->Run();
	ret = demo->DeInit();
}





