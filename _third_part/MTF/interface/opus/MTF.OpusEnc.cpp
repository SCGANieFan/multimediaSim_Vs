#include "MTF.OpusEnc.h"
#include "MTF.String.h"
#include "MTF.Objects.h"
#include "opus_api.h"
#include <stdarg.h>
#include <stdio.h>


using namespace mtf_ns;
static const char* type_this = "opus_enc";
void mtf_opus_enc_register()
{
	MTF_Objects::Registe<MTF_OpusEnc>(type_this);
	OpusEncoderNormalRegister();
}
MTF_OpusEnc::MTF_OpusEnc()
{

}

MTF_OpusEnc::~MTF_OpusEnc()
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
	if (_hd)
	{
		MTF_FREE(_hd);
	}
	if (_enc)
	{
		opus_api_close_encoder(_enc);
		opus_api_destory_encoder(_enc);
		_enc = 0;
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

mtf_i32 MTF_OpusEnc::Init()
{	
#if 1
	//lib init
	_enc = 0;
	uint32_t rate = _rate;
	uint16_t channels = _ch;
	uint16_t width = _width;
	uint32_t bitrate = _bitrate;
	uint32_t frameDMs = 10 * _frameMs;
	MTF_PRINT("(%u,%u,%u),(%u,%u)",
		rate, channels, width, bitrate, frameDMs);
	if (channels > 2
		|| width != 2) {
		return false;
	}

	bool haveHead = false;
	OpusApi_CreateEncParam_t param;
	param.basePort.malloc_cb = opus_malloc;
	param.basePort.realloc_cb = opus_realloc;
	param.basePort.free_cb = opus_free;
	param.basePort.print_cb = opus_print;
	//OpusApiRet_t ret = opus_api_create_encoder(&_enc, &opusApiBasePort, rate, channels, haveHead, OPUS_API_ENC_CHOOSE_NORMAL);
	OpusApiRet_t ret = opus_api_create_encoder(&_enc, &param);
	if (ret != OPUS_API_RET_SUCCESS) {
		MTF_PRINT("opus create fail, %d,(%p,%d,%d,%d)", ret, _enc, rate, channels, haveHead);
		return false;
	}
	ret |= opus_api_encoder_set(_enc, "fs", (void*)rate);
	ret |= opus_api_encoder_set(_enc, "ch", (void*)(uint32_t)channels);
	ret |= opus_api_encoder_set(_enc, "choose", (void*)OPUS_API_ENC_CHOOSE_NORMAL);
	ret |= opus_api_encoder_set(_enc, "bitrate", (void*)bitrate);
	ret |= opus_api_encoder_set(_enc, "f0p1ms", (void*)frameDMs);
	ret |= opus_api_encoder_set(_enc, "vbr", (void*)_vbr);
	ret |= opus_api_encoder_set(_enc, "cpx", (void*)_complexity);
	//range (-1000,1000,1001,1002), each means AUTO,SILK_ONLY,HYBRID,CELT_ONLY. default -1000
	ret |= opus_api_encoder_set(_enc, "encmode", (void*)-1000);
	//ret |= opus_api_encoder_set(_enc, "encmode", (void*)1002);
	//range (2048,2049,2051), each means VOIP,AUDIO,RESTRICTED_LOWDELAY. default 2049
	ret |= opus_api_encoder_set(_enc, "app", (void*)2049);
	ret |= opus_api_encoder_set(_enc, "stk48k1ch", (void*)(40 * 1024));
	if (ret != OPUS_API_RET_SUCCESS) { MTF_PRINT("opus set fail, %d", ret); return false; }

	ret = opus_api_open_encoder(_enc);
	if (ret != OPUS_API_RET_SUCCESS) { MTF_PRINT("opus open fail, %d", ret); return false; }

	_pcmFrameSample = frameDMs * _rate / 10000;
	_pcmFrameByte = _pcmFrameSample * _ch * _width;
	//io data
	mtf_i32 size = _frameBytes;
	_iData.Init((mtf_u8*)MTF_MALLOC(size), size);
	_oData.Init((mtf_u8*)MTF_MALLOC(size), size);
#endif
	return 0;
}

mtf_i32 MTF_OpusEnc::receive(MTF_Data& iData)
{
	_iData.Append(iData.Data(), iData._size);
	if (iData._flags & MTF_DataFlag_ESO)
		_iData._flags |= MTF_DataFlag_ESO;
	iData.Used(iData._size);
	return 0;
}

mtf_i32 MTF_OpusEnc::generate(MTF_Data*& oData)
{
	if (_iData._flags & MTF_DataFlag_ESO){
		_oData._flags |= MTF_DataFlag_ESO;
		oData = &_oData;
		return 0;
	}
	mtf_u8* iBuff = (mtf_u8*)_iData.Data();
	mtf_i32 iByte = _pcmFrameByte;
	mtf_u8* oBuff = _oData.LeftData();
	mtf_i32 oByte = _oData.LeftSize();
	//OpusApiRet_t opus_api_encoder_run(void* hd, unsigned char* pcm, int* pcmByte, unsigned char* encodedFrame, int* encodedFrameByte) {
	OpusApiRet_t ret = opus_api_encoder_run(_enc, iBuff, &iByte, oBuff, &oByte);
	if (ret != OPUS_API_RET_SUCCESS) { MTF_PRINT("opus run fail, %d", ret); return -1; }
	_iData.Used(_pcmFrameByte);
	_oData._size += oByte;
	if (_oData._flags & MTF_DataFlag_ESO) {
		return -1;
	}
	oData = &_oData;
	return 0;
}

mtf_i32 MTF_OpusEnc::Set(const char* key, mtf_void* val)
{
#if 1
	if (MTF_String::StrCompare(key, "bitrate")) {
		_bitrate = (mtf_i32)val; return 0;
	}
	else if (MTF_String::StrCompare(key, "cpmplexity")) {
		_complexity = (mtf_i32)val; return 0;
	}
	else if (MTF_String::StrCompare(key, "vbr")) {
		_vbr = !!(mtf_i32)val; return 0;
	}
#endif
	return MTF_AudioProcess::Set(key, val);
}
mtf_i32 MTF_OpusEnc::Get(const char* key, mtf_void* val)
{
	return MTF_AudioProcess::Get(key, val);
}
