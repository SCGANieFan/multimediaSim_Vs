#include "MTF.H265Dec.h"
#include "MTF.String.h"
#include "MTF.Objects.h"
#include "MAF.h"

void mtf_h265_dec_register()
{
	MTF_Objects::Registe<MTF_H265Dec>("h265_dec");
	MAF_REGISTER(h265_dec);
}
MTF_H265Dec::MTF_H265Dec()
{

}

MTF_H265Dec::~MTF_H265Dec()
{
#if 0
	if (_iData.Data())
	{
		_iData.Used(_iData._size);
		MTF_FREE(_iData.Data());
	}
#endif
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
}

mtf_i32 MTF_H265Dec::Init()
{	
	//lib init
	const char* type = "h265_dec";
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
	};

	const char* script = "type=$0,Malloc=$1,Realloc=$2,Calloc=$3,Free=$4;";
	ret = MAF_Init(_hd, script, param);
	if (ret != MA_RET_SUCCESS)
		MTF_PRINT("err");

	//io data
	mtf_i32 size = 2*1024*1024;
#if 0
	_iData.Init((mtf_u8*)MTF_MALLOC(size), size);
#endif
	_oData.Init((mtf_u8*)MTF_MALLOC(size), size);

	return 0;
}

mtf_i32 MTF_H265Dec::receive(MTF_Data& iData)
{
#if 0
	mtf_i32 appendSize = _iData.LeftSize();
	appendSize = appendSize < iData._size ? appendSize : iData._size;
	_iData.Append(iData.Data(), appendSize);
	if (iData._flags & MTF_DataFlag_ESO)
		_iData._flags |= MTF_DataFlag_ESO;
#endif
	if (iData._flags & MTF_DataFlag_ESO)
		_iFlags |= MTF_DataFlag_ESO;
	AA_Data AA_iData;
	MTF_MEM_SET(&AA_iData, 0, sizeof(AA_Data));
	AA_iData.buff = iData.Data();
	AA_iData.size = AA_iData.max = iData._size;
	MAF_Receive(_hd,&AA_iData);
	iData.Used(iData._size);
	return 0;
}

mtf_i32 MTF_H265Dec::generate(MTF_Data*& oData)
{
	_frames++;
	AA_Data AA_oData;
	MTF_MEM_SET(&AA_oData, 0, sizeof(AA_Data));
	AA_oData.buff = _oData.LeftData();
	AA_oData.max = _oData.LeftSize();

	MAF_Generate(_hd, &AA_oData);
	_oData._size += AA_oData.size;

	if (_iFlags & MTF_DataFlag_ESO){
		_oData._flags |= MTF_DataFlag_ESO;
	}
	oData = &_oData;
	return 0;
}

mtf_i32 MTF_H265Dec::Set(const char* key, mtf_void* val)
{
#if 0
	if (MTF_String::StrCompare(key, "decayMs")) {
		_decayMs = (mtf_i16)val; return 0;
	}
	else if (MTF_String::StrCompare(key, "overlapMs")) {
		_overlapMs = (mtf_i16)val; return 0;
	}
#endif
	return MTF_VideoProcess::Set(key, val);
}
mtf_i32 MTF_H265Dec::Get(const char* key, mtf_void* val)
{
	return MTF_VideoProcess::Get(key, val);
}
