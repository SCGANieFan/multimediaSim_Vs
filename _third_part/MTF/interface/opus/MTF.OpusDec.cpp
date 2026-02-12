#include "MTF.OpusDec.h"
#include "MTF.String.h"
#include "MTF.Objects.h"
#include "opus_api.h"
#include <stdarg.h>
#include <stdio.h>

using namespace mtf_ns;

void mtf_opus_dec_register()
{
	MTF_Objects::Registe<MTF_OpusDec>("opus_dec");
	OpusDecoderNormalRegister();
}
MTF_OpusDec::MTF_OpusDec()
{
}

MTF_OpusDec::~MTF_OpusDec()
{
	if (_iData.Data())
	{
		_iData.Used(_iData._size);
		MTF_FREE(_iData.Data());
	}
	if (_oData.Data())
	{
		_oData.Used(_oData._size);
		MTF_FREE(_oData.Data());
	}
	if (_dec)
	{
		opus_api_close_decoder(_dec);
		opus_api_destory_decoder(_dec);
		_dec = 0;
	}
}

static void* opus_malloc(int size)
{
	void* buf = MTF_MALLOC(size);
	MTF_PRINT("%d,%p", size, buf);
	return buf;
}

static void* opus_realloc(void* rmem, int newsize)
{
	void* buf = MTF_REALLOC(rmem, newsize);
	MTF_PRINT("%d,%p,%p", newsize, rmem, buf);
	return buf;
}

static void opus_free(void* rmem)
{
	MTF_PRINT("%p", rmem);
	MTF_FREE(rmem);
	return;
}

static void opus_print(const char* fmt, ...)
{
	static char buf[256];
	va_list ap;
	va_start(ap, fmt);
	vsprintf(buf, fmt, ap);
	va_end(ap);
	printf("%s\n", buf);
}

mtf_i32 MTF_OpusDec::Init()
{	
	OpusApi_CreateDecParam_t param;
	param.basePort.malloc_cb = opus_malloc;
	param.basePort.realloc_cb = opus_realloc;
	param.basePort.free_cb = opus_free;
	param.basePort.print_cb = opus_print;
	OpusApiRet_t ret = opus_api_create_decoder(&_dec, &param);
	if (ret != OPUS_API_RET_SUCCESS) {
		MTF_PRINT("Cannot create decoder: %d\n", ret);
		return -1;
	}
	ret |= opus_api_decoder_set(_dec, "choose", (void*)OpusApi_DecChoose_e::OPUS_API_DEC_CHOOSE_NORMAL);
	ret |= opus_api_decoder_set(_dec, "fs", (void*)_rate);
	ret |= opus_api_decoder_set(_dec, "ch", (void*)(uint32_t)_ch);
	if (ret != OPUS_API_RET_SUCCESS) {
		MTF_PRINT("set fail"); return 0;
	}
	ret = opus_api_open_decoder(_dec);
	if (ret != OPUS_API_RET_SUCCESS) {
		MTF_PRINT("set fail"); return 0;
	}
	mtf_i32 size = _frameBytes;
	_iData.Init((mtf_u8*)MTF_MALLOC(size), size);
	_oData.Init((mtf_u8*)MTF_MALLOC(size), size);
	return 0;
}

mtf_i32 MTF_OpusDec::receive(MTF_Data& iData)
{
	_iData.Append(iData.Data(), iData._size);
	if (iData._flags & MTF_DataFlag_ESO)
		_iData._flags |= MTF_DataFlag_ESO;
	iData.Used(iData._size);
	return 0;
}

mtf_i32 MTF_OpusDec::generate(MTF_Data*& oData)
{
	{
		static uint32_t cnt = 0;
		++cnt;
		//MTF_PRINT("[%u]", cnt);
		if (cnt == 2001)
			int a = 1;
	}
	if (_iData._flags & MTF_DataFlag_ESO) {
		_oData._flags |= MTF_DataFlag_ESO;
		oData = &_oData;
		return 0;
	}
	mtf_u8* encodedOneFrame = (mtf_u8*)_iData.Data();
	mtf_i32 encodedOneFrameByte = _iData._size;
	mtf_u8* decodecPcm = (mtf_u8*)_oData.LeftData();
	mtf_i32 decodecPcmByte = _oData.LeftSize();
	OpusApiRet_t ret = opus_api_decoder_run(_dec, encodedOneFrame, encodedOneFrameByte, decodecPcm, &decodecPcmByte, false);
	if (ret != OPUS_API_RET_SUCCESS) {
		MTF_PRINT("opus run fail, %d", ret); return false;
	}
	_iData.Used(_iData._size);
	_oData._size += decodecPcmByte;
	oData = &_oData;
	return 0;
}

mtf_i32 MTF_OpusDec::Set(const char* key, mtf_void* val)
{
#if 0
	if (MTF_String::StrCompare(key, "decayMs")) {
		_decayMs = (mtf_i16)val; return 0;
	}
	else if (MTF_String::StrCompare(key, "gainMs")) {
		_gainMs = (mtf_i16)val; return 0;
	}
	else if (MTF_String::StrCompare(key, "overlapMs")) {
		_overlapMs = (mtf_i16)val; return 0;
	}
#endif
	return MTF_AudioProcess::Set(key, val);
}
mtf_i32 MTF_OpusDec::Get(const char* key, mtf_void* val)
{
	return MTF_AudioProcess::Get(key, val);
}
