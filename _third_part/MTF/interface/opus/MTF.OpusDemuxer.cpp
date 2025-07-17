#include<stdio.h>
#include "MTF.OpusDemuxer.h"
#include "MTF.String.h"
#include "MTF.Objects.h"

static const mtf_int8* type_this = "opus_demuxer";

void mtf_opus_demuxer_register()
{
	MTF_Objects::Registe<MTF_OpusDemuxer>(type_this);
}
MTF_OpusDemuxer::MTF_OpusDemuxer()
{
}

MTF_OpusDemuxer::~MTF_OpusDemuxer()
{
	if (_oData.Data())
	{
		_oData.Used(_oData._size);
		MTF_FREE(_oData.Data());
	}
}

mtf_int32 MTF_OpusDemuxer::Init()
{	
#if 0
	//lib init
	mtf_void* param[] = {
	(mtf_void*)_rate,
	(mtf_void*)_ch,
	(mtf_void*)_width,
	(mtf_void*)_frameSamples,
	};

	const mtf_int8* script = "type=$0,Malloc=$1,Realloc=$2,Calloc=$3,Free=$4"\
							 ",rate=$5,ch=$6,width=$7,fSamples=$8;";
	//io data
	mtf_int32 size = _frameBytes;
	_iData.Init((mtf_uint8*)MTF_MALLOC(size), size);
	_oData.Init((mtf_uint8*)MTF_MALLOC(size), size);
#endif
	MTF_PRINT();
	if (!_url) {
		MTF_PRINT("error, _url = 0");
		return -1;
	}
	_pFile = fopen(_url, "rb+");
	if (!_pFile) {
		MTF_PRINT("error, no such file:%s", _url);
		return -1;
	}
	if (!_rate) _rate = 16000;
	if (!_ch) _ch = 1;
	if (!_width) _width = 2;
	if (!_frameSamples) _frameSamples = 2;
	if(!_rate)Set("rate", (void*)16000);
	if(!_ch)Set("ch", (void*)1);
	if(!_width)Set("width", (void*)2);
	mtf_int32 size = 4096;
	_oData.Init((mtf_uint8*)MTF_MALLOC(size), size);
	return 0;
}

mtf_int32 MTF_OpusDemuxer::generate(MTF_Data*& oData)
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
	return 0;
#endif
#if 1
	mtf_int32 readedSize;
	mtf_uint8 tmp[8];
	mtf_uint32 frameByte;
	readedSize = fread(tmp, 1, 8, (FILE*)_pFile);
	if (readedSize != 8) {
		_oData._flags |= MTF_DataFlag_ESO;
		goto exit;
	}
	frameByte = (mtf_uint32)tmp[0] << 24 | (mtf_uint32)tmp[1] << 16 | (mtf_uint32)tmp[2] << 8 | (mtf_uint32)tmp[3];
	if (_oData.LeftSize() < frameByte) {
		MTF_PRINT("err,%d,%d", _oData.LeftSize(), frameByte);
		return -1;
	}
	readedSize = fread(_oData.LeftData(), 1, frameByte, (FILE*)_pFile);
	if (readedSize < frameByte) {
		_oData._flags |= MTF_DataFlag_ESO;
		goto exit;
	}
	_oData._size += readedSize;
exit:
	if (_oData._size <= 0)
		_oData._flags |= MTF_DataFlag_EMPTY;
	oData = &_oData;
	return 0;
#endif
}

mtf_int32 MTF_OpusDemuxer::Set(const mtf_int8* key, mtf_void* val)
{
#if 1
	if (MTF_String::StrCompare(key, "url"))
	{
		MTF_PRINT("url,%s", (const mtf_int8*)val);
		_url = (const mtf_int8*)val;

		return 0;
	}
#endif
	return MTF_AudioDemuxer::Set(key, val);
}
mtf_int32 MTF_OpusDemuxer::Get(const mtf_int8* key, mtf_void* val)
{
	return MTF_AudioDemuxer::Get(key, val);
}
