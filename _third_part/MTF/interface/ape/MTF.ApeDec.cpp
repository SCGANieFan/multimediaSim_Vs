#include "MTF.ApeDec.h"
#include "MTF.String.h"
#include "MTF.Objects.h"
#include "MAF.h"

void mtf_ape_dec_register()
{
	MTF_Objects::Registe<MTF_ApeDec>("ape_dec");
	MAF_REGISTER(ape_dec);
}

MTF_ApeDec::MTF_ApeDec()
{

}

MTF_ApeDec::~MTF_ApeDec()
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
		MAF_Deinit(_hd);
		MTF_FREE(_hd);
	}
}

mtf_i32 MTF_ApeDec::Init()
{	
	//lib init
	const char* type = "ape_dec";
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
	mtf_i32 size = _frameBytes;
	_iData.Init((mtf_u8*)MTF_MALLOC(size), size);
	size = 10 * size;
	_oData.Init((mtf_u8*)MTF_MALLOC(size), size);

	return 0;
}

mtf_i32 MTF_ApeDec::receive(MTF_Data& iData)
{
	_iData.Append(iData.Data(), iData._size);
	if (iData._flags & MTF_DataFlag_ESO)
	{
		_iData._flags = MTF_DataFlag_ESO;
	}
	if (iData._flags & MTF_DataFlag_EXTRA_INFO)
	{
		iData._flags &= ~MTF_DataFlag_EXTRA_INFO;
		_iData._flags = MTF_DataFlag_EXTRA_INFO;
	}
	iData.Used(iData._size);
	return 0;
}

#define FRAMES_LOST 5
#define FRAMES_TOTAL 50
mtf_i32 MTF_ApeDec::generate(MTF_Data*& oData)
{
	AA_Data AA_iData;
	MTF_MEM_SET(&AA_iData, 0, sizeof(AA_Data));
	AA_iData.buff = _iData.Data();
	AA_iData.max = AA_iData.size = _iData._size;
	if (_iData._flags & MTF_DataFlag_ESO)
	{
		AA_iData.flags = AA_DataFlag_FRAME_IS_EOS;
	}
	if (_iData._flags & MTF_DataFlag_EXTRA_INFO)
	{
		_iData._flags &= ~AA_DataFlag_FRAME_IS_EXTRA_INFO;
		AA_iData.flags = AA_DataFlag_FRAME_IS_EXTRA_INFO;
	}
	_frames++;

	AA_Data AA_oData;
	MTF_MEM_SET(&AA_oData, 0, sizeof(AA_Data));
	AA_oData.buff = _oData.LeftData();
	AA_oData.max = _oData.LeftSize();

	MAF_Run(_hd, &AA_iData, &AA_oData);
	_iData.Used(_iData._size);
	_oData._size += AA_oData.size;

	//exit check
	if ((_iData._flags & MTF_DataFlag_ESO)
		&& _oData._size <= 0)
	{
		return -1;
	}
	oData = &_oData;
	return 0;
}

mtf_i32 MTF_ApeDec::Set(const char* key, mtf_void* val)
{
#if 0
	if (MTF_String::StrCompare(key, "decayMs")) {
		_decayMs = (mtf_i16)val; return 0;
	}
	else if (MTF_String::StrCompare(key, "overlapMs")) {
		_overlapMs = (mtf_i16)val; return 0;
	}
#endif
	return MTF_AudioProcess::Set(key, val);
}
mtf_i32 MTF_ApeDec::Get(const char* key, mtf_void* val)
{
	return MTF_AudioProcess::Get(key, val);
}
