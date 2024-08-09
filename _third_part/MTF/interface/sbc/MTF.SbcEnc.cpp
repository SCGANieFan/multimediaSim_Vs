#include "MTF.SbcEnc.h"
#include "MTF.String.h"
#include "MTF.Objects.h"
#include "MAF.h"

void mtf_sbc_enc_register()
{
	MTF_Objects::Registe<MTF_SbcEnc>("sbc_enc");
	MAF_REGISTER(sbc_enc);
}
MTF_SbcEnc::MTF_SbcEnc()
{

}

MTF_SbcEnc::~MTF_SbcEnc()
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

mtf_int32 MTF_SbcEnc::Init()
{
	//lib init
	const mtf_int8* type = "sbc_enc";
	MA_Ret ret;
	ret = MAF_GetHandleSize(type, &_hdSize);
	if (ret != MA_RET_SUCCESS)
		MTF_PRINT("err");
	if (_hdSize < 1)
		MTF_PRINT("err");
	_hd = MTF_MALLOC(_hdSize);
	if (!_hd)
		MTF_PRINT("err");
#if 0
	if (MAF_String::StrCompare(key, "bitPool")) {
	else if (MAF_String::StrCompare(key, "blocks")) {
	else if (MAF_String::StrCompare(key, "subBands")) {
	else if (MAF_String::StrCompare(key, "channelMode")) {
	else if (MAF_String::StrCompare(key, "allocMethod")) {
	else if (MAF_String::StrCompare(key, "sbcMode")) {

	mtf_uint8 _bitPool = 0;
	mtf_uint8 _blocks = 0;
	mtf_uint8 _subBands = 0;
	mtf_uint8 _channelMode = 0;
	mtf_uint8 _allocMethod = 0;
	mtf_uint8 _sbcMode = 0;
#endif
	mtf_void* param[] = {
	(mtf_void*)type,
	(mtf_void*)MTF_Memory::Malloc,
	(mtf_void*)MTF_Memory::Realloc,
	(mtf_void*)MTF_Memory::Calloc,
	(mtf_void*)MTF_Memory::Free,
	(mtf_void*)_rate,
	(mtf_void*)_ch,
	(mtf_void*)_width,
	(mtf_void*)_bitPool,
	(mtf_void*)_blocks,
	(mtf_void*)_subBands,
	(mtf_void*)_channelMode,
	(mtf_void*)_allocMethod,
	(mtf_void*)_sbcMode,
	};

	const mtf_int8* script = "type=$0,Malloc=$1,Realloc=$2,Calloc=$3,Free=$4"\
							 ",rate=$5,ch=$6,width=$7"\
							 ",bitPool=$8,blocks=$9,subBands=$10,channelMode=$11,allocMethod=$12,sbcMode=$13;";
	ret = MAF_Init(_hd, script, param);
	if (ret != MA_RET_SUCCESS)
		MTF_PRINT("err");

	//io data
	mtf_int32 size = _frameBytes;
	_iData.Init((mtf_uint8*)MTF_MALLOC(size), size);
	_oData.Init((mtf_uint8*)MTF_MALLOC(size), size);

	return 0;
}

mtf_int32 MTF_SbcEnc::receive(MTF_Data& iData)
{
	mtf_int32 appendSize = _iData.LeftSize();
	appendSize = appendSize < iData._size ? appendSize : iData._size;
	_iData.Append(iData.Data(), appendSize);
	if (iData._flags & MTF_DataFlag_ESO)
		_iData._flags |= MTF_DataFlag_ESO;

	iData.Used(appendSize);
	return 0;
}


mtf_int32 MTF_SbcEnc::generate(MTF_Data*& oData)
{
#if 0
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

	if (_iData._flags & MTF_DataFlag_ESO){
		_oData._flags |= MTF_DataFlag_ESO;
	}
	oData = &_oData;
	return 0;
#else
	AA_Data AA_iData;
	MTF_MEM_SET(&AA_iData, 0, sizeof(AA_Data));
	AA_iData.buff = _iData.Data();
	AA_iData.max = AA_iData.size = _iData._size;

	_frames++;
	AA_Data AA_oData;
	MTF_MEM_SET(&AA_oData, 0, sizeof(AA_Data));
	AA_oData.buff = _oData.LeftData();
	AA_oData.max = _oData.LeftSize();

	MAF_Run(_hd, &AA_iData, &AA_oData);
	_iData.Used(_iData._size - AA_iData.size);
	_oData._size += AA_oData.size;

	if (_iData._flags & MTF_DataFlag_ESO){
		_oData._flags |= MTF_DataFlag_ESO;
	}
	oData = &_oData;
	return 0;
#endif
}

mtf_int32 MTF_SbcEnc::Set(const mtf_int8* key, mtf_void* val)
{
#if 1
	if (MTF_String::StrCompare(key, "bitPool")) {
		_bitPool = (mtf_int16)val; return 0;
	}
	else if (MTF_String::StrCompare(key, "blocks")) {
		_blocks = (mtf_int16)val; return 0;
	}
	else if (MTF_String::StrCompare(key, "subBands")) {
		_subBands = (mtf_int16)val; return 0;
	}
	else if (MTF_String::StrCompare(key, "channelMode")) {
		_channelMode = (mtf_int16)val; return 0;
	}
	else if (MTF_String::StrCompare(key, "allocMethod")) {
		_allocMethod = (mtf_int16)val; return 0;
	}
	else if (MTF_String::StrCompare(key, "sbcMode")) {
		_sbcMode = (mtf_int16)val; return 0;
	}
#endif
	return MTF_AudioProcess::Set(key, val);
}
mtf_int32 MTF_SbcEnc::Get(const mtf_int8* key, mtf_void* val)
{
	return MTF_AudioProcess::Get(key, val);
}
