#include "MTF.OpusDec.h"
#include "MTF.String.h"
#include "MTF.Objects.h"

static const mtf_int8* type_this = "opus_dec";

void mtf_opus_dec_register()
{
	MTF_Objects::Registe<MTF_OpusDec>(type_this);
}
MTF_OpusDec::MTF_OpusDec()
{

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
		MAF_Deinit(_hd);
		MTF_FREE(_hd);
	}
#endif
}

mtf_int32 MTF_OpusDec::Init()
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

	mtf_void* param[] = {
	(mtf_void*)type,
	(mtf_void*)MTF_Memory::Malloc,
	(mtf_void*)MTF_Memory::Realloc,
	(mtf_void*)MTF_Memory::Calloc,
	(mtf_void*)MTF_Memory::Free,
	(mtf_void*)_rate,
	(mtf_void*)_ch,
	(mtf_void*)_width,
	(mtf_void*)_frameSamples,
	};

	const mtf_int8* script = "type=$0,Malloc=$1,Realloc=$2,Calloc=$3,Free=$4"\
							 ",rate=$5,ch=$6,width=$7,fSamples=$8;";
	ret = MAF_Init(_hd, script, param);
	if (ret != MA_RET_SUCCESS)
		MTF_PRINT("err");

	//io data
	mtf_int32 size = _frameBytes;
	_iData.Init((mtf_uint8*)MTF_MALLOC(size), size);
	_oData.Init((mtf_uint8*)MTF_MALLOC(size), size);
#endif
	return 0;
}

mtf_int32 MTF_OpusDec::receive(MTF_Data& iData)
{
#if 0
	_iData.Append(iData.Data(), iData._size);
	if (iData._flags & MTF_DataFlag_ESO)
		_iData._flags |= MTF_DataFlag_ESO;
	iData.Used(iData._size);
#endif
	return 0;
}

mtf_int32 MTF_OpusDec::generate(MTF_Data*& oData)
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
	_oData._size += AA_oData.size;

	if (_iData._flags & MTF_DataFlag_ESO){
		_oData._flags |= MTF_DataFlag_ESO;
	}
	oData = &_oData;
#endif
	return 0;
}

mtf_int32 MTF_OpusDec::Set(const mtf_int8* key, mtf_void* val)
{
#if 0
	if (MTF_String::StrCompare(key, "decayMs")) {
		_decayMs = (mtf_int16)val; return 0;
	}
	else if (MTF_String::StrCompare(key, "gainMs")) {
		_gainMs = (mtf_int16)val; return 0;
	}
	else if (MTF_String::StrCompare(key, "overlapMs")) {
		_overlapMs = (mtf_int16)val; return 0;
	}
#endif
	return MTF_AudioProcess::Set(key, val);
}
mtf_int32 MTF_OpusDec::Get(const mtf_int8* key, mtf_void* val)
{
	return MTF_AudioProcess::Get(key, val);
}
