#include "MTF.OpusDec.h"
#include "MTF.String.h"
#include "MTF.Objects.h"
#include "OpusApi.h"
using namespace OpusApi_ns;


using namespace mtf_ns;
static const char* type_this = "opus_dec";

void mtf_opus_dec_register()
{
	MTF_Objects::Registe<MTF_OpusDec>(type_this);
}
MTF_OpusDec::MTF_OpusDec()
{
	OpusApiMemory_t opusApiMemory;
	opusApiMemory.malloc_cb = Malloc;
	opusApiMemory.realloc_cb = Realloc;
	opusApiMemory.free_cb = Free;
	OpusApi::memory_register(&opusApiMemory);
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
	if (_hd)
	{
		if(_hd)
			OpusApi::destory_decoder(_hd);
		_hd = 0;
	}
}

mtf_i32 MTF_OpusDec::Init()
{	
	MTF_PRINT("channels:%d", _ch);
	MTF_PRINT("frameSamples:%d", _frameSamples);
	MTF_PRINT("fsHz:%d", _rate);

	OpusApiRet_t ret = OpusApi::create_decoder(&_hd, _rate, _ch);
	if (ret != OPUS_API_RET_SUCCESS) {
		MTF_PRINT("Cannot create decoder: %d\n", ret);
		return false;
	}
	MTF_PRINT("create decoder success\n");

	//io data
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
	//MAF_Run(_hd, &AA_iData, &AA_oData);
	mtf_u8* iBuff = (mtf_u8*)_iData.Data();
	mtf_i32 iSize = _iData._size;
	mtf_i16* oBuff = (mtf_i16*)_oData.LeftData();
	mtf_i32 oSample = _oData.LeftSize() / (2 * _ch);
	mtf_bool isPlc = false;
	OpusApiRet_t ret = OpusApi::decoder_run(_hd, iBuff, iSize, oBuff, &oSample, isPlc);
	if (ret != OPUS_API_RET_SUCCESS) {
		return -1;
	}
	_iData.Used(_iData._size);
	_oData._size += oSample * 2 * _ch;

	if (_iData._flags & MTF_DataFlag_ESO){
		_oData._flags |= MTF_DataFlag_ESO;
	}
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
