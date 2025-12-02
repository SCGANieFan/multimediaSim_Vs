#include "MTF.OpusDec.h"
#include "MTF.String.h"
#include "MTF.Objects.h"


using namespace mtf_ns;
static const char* type_this = "opus_dec";

void mtf_opus_dec_register()
{
	MTF_Objects::Registe<MTF_OpusDec>(type_this);
}
MTF_OpusDec::MTF_OpusDec()
{
#if 0
	OpusApiMemory_t opusApiMemory;
	opusApiMemory.malloc_cb = Malloc;
	opusApiMemory.realloc_cb = Realloc;
	opusApiMemory.free_cb = Free;
	OpusApi::memory_register(&opusApiMemory);
#endif
}

MTF_OpusDec::~MTF_OpusDec()
{
#if 0
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
#endif
}

mtf_i32 MTF_OpusDec::Init()
{	
#if 0
	//lib init
	const mtf_int8* type = type_this;
	MA_Ret ret;
	ret = MAF_GetHandleSize(type, &_hdSize);
	if (ret != MA_RET_SUCCESS)
		MTF_PRINT("err");
	if (_hdSize < 1)
		MTF_PRINT("err");
	_hd = MTF_MALLOC(_hdSize);
	if (!_hd)
		MTF_PRINT("err");

	OpusApiRet_t ret = OpusApi::create_decoder(&_hd, _rate, _ch);
	if (ret != OPUS_API_RET_SUCCESS) {
		MTF_PRINT("Cannot create decoder: %d\n", ret);
		return false;
	}
	MTF_PRINT("create decoder success\n");

	//io data
	mtf_int32 size = _frameBytes;
	_iData.Init((mtf_uint8*)MTF_MALLOC(size), size);
	_oData.Init((mtf_uint8*)MTF_MALLOC(size), size);
#endif
	return 0;
}

mtf_i32 MTF_OpusDec::receive(MTF_Data& iData)
{
#if 0
	_iData.Append(iData.Data(), iData._size);
	if (iData._flags & MTF_DataFlag_ESO)
		_iData._flags |= MTF_DataFlag_ESO;
	iData.Used(iData._size);
#endif
	return 0;
}

mtf_i32 MTF_OpusDec::generate(MTF_Data*& oData)
{
#if 0
	AA_Data AA_iData;
	MTF_MEM_SET(&AA_iData, 0, sizeof(AA_Data));
	AA_iData.buff = _iData.Data();
	AA_iData.max = AA_iData.size = _iData._size;

	AA_Data AA_oData;
	MTF_MEM_SET(&AA_oData, 0, sizeof(AA_Data));
	AA_oData.buff = _oData.LeftData();
	AA_oData.max = _oData.LeftSize();

	MAF_Run(_hd, &AA_iData, &AA_oData);
	_iData.Used(_iData._size);
	_oData._size += oSample * 2 * _ch;

	if (_iData._flags & MTF_DataFlag_ESO){
		_oData._flags |= MTF_DataFlag_ESO;
	}
	oData = &_oData;
#endif
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
