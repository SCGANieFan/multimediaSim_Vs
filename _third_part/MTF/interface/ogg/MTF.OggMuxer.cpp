#include"MTF.OggMuxer.h"
#include"MTF.Objects.h"
#include"MTF.Porting.h"
#include "ogg_api.h"

#define LOG_OGG MTF_PRINT

void mtf_ogg_muxer_register()
{
	MTF_Objects::Registe<MTF_OggMuxer>("ogg_muxer");
}


MTF_OggMuxer::MTF_OggMuxer()
{
}

MTF_OggMuxer::~MTF_OggMuxer()
{
#if 0
	if (_hd){
		ogg_muxer_api_destory(_hd);
	}
	if (_pFile)
		FileClosePorting(_pFile);

	if (_last_dat.Used(_last_dat._size)) {
		free(_last_dat.Data());
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
	MTF_PRINT();
	if (!_url) {
		MTF_PRINT("error, _url = 0");
		return -1;
	}
	_pFile = FileOpenPorting(_url, "wb+");
	if (!_pFile) {
		MTF_PRINT("error, no such file:%s", _url);
		return -1;
	}
	_bytePerSample = _ch * _width;
	OggMuxerApiParam_t oggMuxerApiParam = { 0 };
	oggMuxerApiParam.malloc_cb = OggMalloc;
	oggMuxerApiParam.realloc_cb = OggRealloc;
	oggMuxerApiParam.free_cb = OggFree;
	oggMuxerApiParam.printf_cb = OggPrint;
	oggMuxerApiParam.mode = OggMuxerApiMode_e::OGG_MUXER_API_MODE_OPUS;
	oggMuxerApiParam.idParam.version = 1;
	oggMuxerApiParam.idParam.channel = _ch;
	oggMuxerApiParam.idParam.preSkip = 0;
	oggMuxerApiParam.idParam.sampleRate = _rate;
	oggMuxerApiParam.idParam.outPutGain = 0;
	uint8_t vendorString[] = "Lavf60.16.100";
	uint8_t userComment[] = "encoder=Lavc60.31.102 libopus";
	oggMuxerApiParam.userComment.vendorString = vendorString;
	oggMuxerApiParam.userComment.vendorStringLen = sizeof(vendorString) - 1;
	oggMuxerApiParam.userComment.userCommentString = userComment;
	oggMuxerApiParam.userComment.userCommentStringLen = sizeof(userComment) - 1;
	oggMuxerApiParam.page_byte_round = _page_byte_round;
	OggRet_t ret;
	ret = ogg_muxer_api_create(&oggMuxerApiParam, &_hd);
	if (ret != OGG_RET_SUCCESS) {
		MTF_PRINT();
	}
	
	mtf_i32 buff_len = 4 * 1024;
	mtf_u8* buff = (mtf_u8*)malloc(buff_len);
	_last_dat.Init(buff, buff_len);
#endif
	return 0;
}
mtf_i32 MTF_OggMuxer::receive(MTF_Data& iData)
{
#if 0
	uint32_t _enc_frame_0p1ms= 200;
	uint32_t frameSample = 48 * _enc_frame_0p1ms / 10;
	static uint32_t frameSampleAcc = 0;
	frameSampleAcc += frameSample;
	OggRet_t ret = OGG_RET_SUCCESS;
	ogg_muxer_api_set(_hd, OggMuxerApiSet_e::OGG_MUXER_API_SET_GRANULEPOS, (void*)frameSampleAcc);
	if (iData._flags & MTF_DataFlag_ESO) {
		ogg_muxer_api_set(_hd, OggMuxerApiSet_e::OGG_MUXER_API_SET_IS_EOS, (void*)1);
	}
	if (iData._size) {
		ret = ogg_muxer_api_receive(_hd, (uint8_t*)iData.Data(), iData._size);
		if (ret != OGG_RET_SUCCESS) {
			LOG_OGG("%d", ret);
			return -1;
		}
		iData.Used(iData._size);
	}

	while (1) {
		OggPage_t oggPage;
		ret = ogg_muxer_api_generate(_hd, &oggPage);
		if (ret == OGG_RET_SUCCESS) {
			FileWritePorting(_pFile, oggPage.headData, oggPage.headLen);
			FileWritePorting(_pFile, oggPage.bodyData, oggPage.bodyLen);
			LOG_OGG("%d", oggPage.headLen + oggPage.bodyLen);
		}
		else
			break;
	}
	if (iData._flags & MTF_DataFlag_ESO) {
		return -1;
	}
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



