#include "MTF.AudioResample.h"
#include "MTF.String.h"
#include "MTF.Objects.h"
#include "MAF.h"


void mtf_auio_resample_register()
{
	MTF_Objects::Registe<MTF_AudioResample>("auio_resample");
	MAF_REGISTER(auio_resample);

}
MTF_AudioResample::MTF_AudioResample()
{

}

MTF_AudioResample::~MTF_AudioResample()
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

mtf_i32 MTF_AudioResample::Init()
{	
	//lib init
#if 1
	const char* type = "auio_resample";

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
	(mtf_void*)_oFs,
	};

	const char* script = "type=$0,Malloc=$1,Realloc=$2,Calloc=$3,Free=$4"\
							 ",rate=$5,ch=$6,width=$7,oFs=$8;";
	ret = MAF_Init(_hd, script, param);
	if (ret != MA_RET_SUCCESS)
		MTF_PRINT("err");

	//io data
	mtf_i32 size = _frameBytes;
	_iData.Init((mtf_u8*)MTF_MALLOC(size), size);
	size = 6* size;
	_oData.Init((mtf_u8*)MTF_MALLOC(size), size);
#endif
	return 0;
}

mtf_i32 MTF_AudioResample::receive(MTF_Data& iData)
{
	_iData.Append(iData.Data(), iData._size);
	iData.Used(iData._size);
	return 0;
}

#define FRAMES_LOST 5
#define FRAMES_TOTAL 50
mtf_i32 MTF_AudioResample::generate(MTF_Data*& oData)
{
	AA_Data AA_iData;
	MTF_MEM_SET(&AA_iData, 0, sizeof(AA_Data));
	AA_iData.buff = _iData.Data();
	AA_iData.max = AA_iData.size = _iData._size;

	_frames++;
	if (_frames % FRAMES_TOTAL > (FRAMES_TOTAL - FRAMES_LOST))
	{
		AA_iData.flags |= AA_DataFlag_FRAME_IS_EMPTY;
	}
	else
		AA_iData.flags &= ~AA_DataFlag_FRAME_IS_EMPTY;

	AA_Data AA_oData;
	MTF_MEM_SET(&AA_oData, 0, sizeof(AA_Data));
	AA_oData.buff = _oData.LeftData();
	AA_oData.max = _oData.LeftSize();

	MAF_Run(_hd, &AA_iData, &AA_oData);
	_iData.Used(_iData._size);
	_oData._size += AA_oData.size;

	oData = &_oData;
	return 0;
}

mtf_i32 MTF_AudioResample::Set(const char* key, mtf_void* val)
{
#if 1
	if (MTF_String::StrCompare(key, "oFs")) {
		_oFs = ((mtf_i32)val); return 0;
	}
#endif
	return MTF_AudioProcess::Set(key, val);
}
mtf_i32 MTF_AudioResample::Get(const char* key, mtf_void* val)
{
	return MTF_AudioProcess::Get(key, val);
}
