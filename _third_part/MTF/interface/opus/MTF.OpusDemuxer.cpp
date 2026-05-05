#include "MTF.OpusDemuxer.h"
#include "MTF.String.h"
#include "MTF.Objects.h"
#include "MTF.Porting.h"

static const char* type_this = "opus_demuxer";

void mtf_opus_demuxer_register()
{
	MTF_Objects::Registe<MTF_OpusDemuxer>(type_this);
}
MTF_OpusDemuxer::MTF_OpusDemuxer()
{
}

MTF_OpusDemuxer::~MTF_OpusDemuxer()
{
	if (_oData.Buff())
	{
		MTF_FREE(_oData.Buff());
	}
}

mtf_i32 MTF_OpusDemuxer::Init()
{	
#if 0
	//lib init
	mtf_void* param[] = {
	(mtf_void*)_rate,
	(mtf_void*)_ch,
	(mtf_void*)_width,
	(mtf_void*)_frameSamples,
	};

	const char* script = "type=$0,Malloc=$1,Realloc=$2,Calloc=$3,Free=$4"\
							 ",rate=$5,ch=$6,width=$7,fSamples=$8;";
	//io data
	mtf_i32 size = _frameBytes;
	_iData.Init((mtf_u8*)MTF_MALLOC(size), size);
	_oData.Init((mtf_u8*)MTF_MALLOC(size), size);
#endif
	MTF_PRINT();
	if (!_url) {
		MTF_PRINT("error, _url = 0");
		return -1;
	}
	_pFile = FileOpenPorting(_url, "rb+");
	if (!_pFile) {
		MTF_PRINT("error, no such file:%s", _url);
		return -1;
	}
	if (!_rate) _rate = 16000;
	if (!_ch) _ch = 1;
	if (!_width) _width = 2;
	if (!_frameSamples) _frameSamples = 2;
	Set("rate", (void*)_rate);
	Set("ch", (void*)_ch);
	Set("width", (void*)_width);
	mtf_i32 size = 4096;
	_oData.Init((mtf_u8*)MTF_MALLOC(size), size);
	return 0;
}

mtf_i32 MTF_OpusDemuxer::generate(MTF_Data*& oData)
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
	mtf_i32 readedSize;
	mtf_u8 tmp[8];
	mtf_u32 frameByte;
	readedSize = FileReadPorting(_pFile, tmp, 8);
	if (readedSize != 8) {
		_oData._flags |= MTF_DataFlag_ESO;
		goto exit;
	}
	frameByte = (mtf_u32)tmp[0] << 24 | (mtf_u32)tmp[1] << 16 | (mtf_u32)tmp[2] << 8 | (mtf_u32)tmp[3];
	_oData.Clear();
	if (_oData.LeftSize() < frameByte) {
		MTF_PRINT("err,%d,%d", _oData.LeftSize(), frameByte);
		return -1;
	}
	readedSize = FileReadPorting(_pFile, _oData.LeftData(), frameByte);
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

mtf_i32 MTF_OpusDemuxer::Set(const char* key, mtf_void* val)
{
#if 1
	if (MTF_String::StrCompare(key, "url"))
	{
		MTF_PRINT("url,%s", (const char*)val);
		_url = (const char*)val;

		return 0;
	}
#endif
	return MTF_AudioDemuxer::Set(key, val);
}
mtf_i32 MTF_OpusDemuxer::Get(const char* key, mtf_void* val)
{
	return MTF_AudioDemuxer::Get(key, val);
}
