#include"MTF.OggMuxer.h"
#include"MTF.Objects.h"
#include"MTF.Porting.h"
#include "ogg_api.h"

#define LOG_OGG MTF_PRINT
//#define LOG_OGG(fmt,...)        GadfPrint("(%s)[%d]" fmt, __func__, __LINE__, ##__VA_ARGS__)
void mtf_ogg_muxer_register()
{
	MTF_Objects::Registe<MTF_OggMuxer>("ogg_muxer");
	ogg_api_register_ogg_muxer();
}


MTF_OggMuxer::MTF_OggMuxer()
{
}

MTF_OggMuxer::~MTF_OggMuxer()
{
#if 1
	if (_pFile)
		FileClosePorting(_pFile);

	if (_last_dat.Used(_last_dat._size)) {
		free(_last_dat.Data());
	}
	if (_idMuxer) {
		ogg_api_muxer_close(_idMuxer);
		ogg_api_muxer_destory(_idMuxer);
		_idMuxer = 0;
	}
	if (_oData.Data())
	{
		_oData.Used(_oData._size);
		MTF_FREE(_oData.Data());
	}
#endif
}

#if 1

void* MTF_OggMuxer::OggMalloc(uint32_t size) {
	static int32_t sizeTotal = 0;
#if 1
	sizeTotal += size;
	void* ptr = malloc(size);
	MTF_PRINT("malloc, ptr:%p, size:%d, sizeTotal:%d,", ptr, size, sizeTotal);
	return ptr;
#else
	return ((ALGO_Malloc_t)_malloc)(size);
#endif
}

void* MTF_OggMuxer::OggRealloc(void* ptr, uint32_t size) {
	void* ptrNew = realloc(ptr, size);
	MTF_PRINT("realloc, (%p->%p,%d)", ptr, ptrNew, size);
	return ptrNew;
}

void MTF_OggMuxer::OggFree(void* ptr) {
#if 1
	MTF_PRINT("free, ptr:%p", ptr);
#endif
	return free(ptr);
}
void MTF_OggMuxer::OggPrint(const char* fmt, ...) {
	static char buf[256];
	VaListPorting_t args;
	VaStartPorting(args, fmt);
	VsprintfPorting(buf, fmt, args);
	VaEndPorting(args);
	MTF_PRINTORI("%s",buf);
}
#endif

mtf_i32 MTF_OggMuxer::Init()
{
#if 0

	

	uint32_t id = 0;
	OggApiRet_t ret = OGG_API_RET_SUCCESS;
	OggApiBasePort_t bp;
	bp.malloc_cb = OggMalloc;
	bp.realloc_cb = OggRealloc;
	bp.free_cb = OggFree;
	bp.printf_cb = 0;
	id = ogg_api_muxer_create(&bp);
	if (!id) { LOG_OGG("opus set fail"); return -1; }

	ret |= ogg_api_muxer_set(id, "mode", (void*)"opus");
	ret |= ogg_api_muxer_set(id, "version", (void*)(uint32_t)1);
	ret |= ogg_api_muxer_set(id, "ch", (void*)(uint32_t)channels);
	ret |= ogg_api_muxer_set(id, "preSkip", (void*)(uint32_t)0);
	ret |= ogg_api_muxer_set(id, "fs", (void*)(uint32_t)rate);
	ret |= ogg_api_muxer_set(id, "oGain", (void*)(uint32_t)0);
	ret |= ogg_api_muxer_set(id, "vendor", (void*)(uint32_t)"Lavf60.16.100");
	ret |= ogg_api_muxer_set(id, "comment", (void*)(uint32_t)"encoder=Lavc60.31.102 libopus");
	ret |= ogg_api_muxer_set(id, "pageByte", (void*)(uint32_t)4096);
	if (ret != OGG_API_RET_SUCCESS) { LOG_OGG("ogg set fail, %d", ret); return -1; }

	ret = ogg_api_muxer_open(id);
	if (ret != OGG_API_RET_SUCCESS) { LOG_OGG("ogg open fail, %d", ret); return -1; }

	//_frame_sample = frameDMs * rate / 10000;
	//_frame_sample = frameDMs * 48 / 10;
	//_frame_sample_acc = 0;

	mtf_i32 size = 5 * 1024;
	_oData.Init((mtf_u8*)MTF_MALLOC(size), size);


#endif



	return 0;
}
mtf_i32 MTF_OggMuxer::receive(MTF_Data& iData)
{
#if 0


#if 1
	
	uint8_t* iBuf = iData.Data();
	int32_t iSize = iData._size;
	uint8_t* oBuf = _oData.LeftData();
	int32_t oSize = _oData.LeftSize();
	iData._size = 0;
	if (iData._flags & MTF_DataFlag_ESO) {
		ogg_api_muxer_set((uint32_t)_idMuxer, "eos", (void*)1);
	}
	OggApiRet_t ret = OGG_API_RET_SUCCESS;
	if (iSize) {
		_frame_sample_acc += _frame_sample;
		{
			static uint32_t iSizeAcc = 0;
			static uint32_t ms = 0;
			iSizeAcc += iSize;
			ms += 20;
			LOG_OGG("%d,%d,%dms", _frame_sample_acc, iSizeAcc, ms);
		}
		ogg_api_muxer_set((uint32_t)_idMuxer, "gPos", (void*)(uint32_t)_frame_sample_acc);
		ret = ogg_api_muxer_receive((uint32_t)_idMuxer, iBuf, &iSize);
		if (ret != OGG_API_RET_SUCCESS) return false;
	}
	ret = ogg_api_muxer_generate((uint32_t)_idMuxer, oBuf, &oSize);
	_oData._size += oSize;
#endif
	//OggApiRet_t ret;
	//ret = ogg_api_muxer_receive(_idMuxer, uint8_t *buf, int32_t * bufByte);
	//ret = ogg_api_muxer_generate(_idMuxer, uint8_t *buf, int32_t * bufByte);


#endif

	return 0;
}

mtf_i32 MTF_OggMuxer::Set(const char* key, mtf_void* val)
{
	if (MTF_String::StrCompare(key, "url"))
	{
		MTF_PRINT("url,%s", (const char*)val);
		_url = (const char*)val;
		return 0;
	}
	else if(MTF_String::StrCompare(key, "pagebyte")){
		_page_byte_round = (mtf_u32)val;
		return 0;
	}
	return MTF_Sink::Set(key, val);
}
mtf_i32 MTF_OggMuxer::Get(const char* key, mtf_void* val)
{
	return MTF_Sink::Get(key, val);
}



