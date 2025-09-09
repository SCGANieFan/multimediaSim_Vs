#include <stdio.h>
#include "MTF.ApeDemux.h"
#include "MTF.String.h"
#include "MTF.Objects.h"
#include "MAF.h"

#define MIN(a,b) (a)<(b)?(a):(b)

mtf_void mtf_ape_demux_register()
{
	MTF_Objects::Registe<MTF_ApeDemux>("ape_demux");
	MAF_REGISTER(ape_demux);
}
MTF_ApeDemux::MTF_ApeDemux()
{

}

MTF_ApeDemux::~MTF_ApeDemux()
{
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
	if (_pFile)
		fclose((FILE*)_pFile);
}

mtf_i32 MTF_ApeDemux::Init()
{	
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

	//lib init
	const char* type = "ape_demux";
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

#if 1
	MTF_MEM_SET(&_extraInfo, 0, sizeof(ExtraInfo_t));

	//apeHeader
	const mtf_i32 readDataLen = 512;
	mtf_u8 readData[readDataLen];
	while (1){
		fread(readData, 1, readDataLen, (FILE*)_pFile);
		AA_Data AA_iData;
		MTF_MEM_SET(&AA_iData, 0, sizeof(AA_Data));
		AA_iData.buff = readData;
		AA_iData.size = readDataLen;
		MAF_Run(_hd, &AA_iData, 0);
		mtf_u32 isRunFinish;
		MAF_Get(_hd, "isRunFinish", (mtf_void**)&isRunFinish);
		if (isRunFinish) {
			MAF_Get(_hd, "apeHeader", (mtf_void**)&_extraInfo.apeHeader);
			break;
		}
	}
	//apeHeader
	mtf_i32 seekTablePos;
	mtf_i32 seekTableSizeByte;
	mtf_void *param0[]={&seekTablePos,&seekTableSizeByte};
	MAF_Get(_hd, "seekTable", (mtf_void**)param0);
	SeekTableManger seekTableManger;
	seekTableManger.Init(seekTablePos,seekTableSizeByte,_pFile);
	_startPos = 150 * 1024;
	mtf_u32 _startPosTmp;
	while (1) {
	seekTableManger.UpdataSeektable();
	if (seekTableManger.GetValidSeekTableNum() == 0)
			break;
		mtf_void* param1[] = { (mtf_void*)seekTableManger.GetValidSeekTable(),(mtf_void*)seekTableManger.GetValidSeekTableByte() };
		MAF_Set(_hd, "seekTable", (mtf_void**)param1);
		_startPosTmp = _startPos;
		mtf_u32 seekTableNumOffset;
		mtf_void* param2[3] = { &_startPosTmp, &seekTableNumOffset ,&_extraInfo.skip };
		if (MAF_Get(_hd, "startInfoFromPos", (mtf_void**)param2)== MA_RET_SUCCESS){
			_extraInfo.startFrame += seekTableNumOffset;
			if (_extraInfo.startFrame == 0)
				_extraInfo.startFrame = 1;
			break;
		}
		_extraInfo.startFrame += seekTableManger.GetValidSeekTableNum();
		seekTableManger.Used();
	}
	_startPos = _startPosTmp;
	MTF_PRINT("startFrame:%d, startPos:%d,_startSkip:%d", _extraInfo.startFrame, _startPos, _extraInfo.skip);
	fseek((FILE*)_pFile, _startPos, SEEK_SET);
	_isFirstFrame = true;
#endif
	mtf_u32 rate;
	mtf_u32 ch;
	mtf_u32 width;
	mtf_void* param3[3] = { &rate,&ch, &width };
	MAF_Get(_hd, "audioInfo", (mtf_void**)param3);
	MTF_AudioDemuxer::Set("rate", (mtf_void*)rate);
	MTF_AudioDemuxer::Set("ch", (mtf_void*)ch);
	MTF_AudioDemuxer::Set("width", (mtf_void*)width);
	MTF_AudioDemuxer::Set("fMs", (mtf_void*)20);

	//io data
	mtf_i32 size = _frameBytes;
	_oData.Init((mtf_u8*)MTF_MALLOC(size), size);

	return 0;
}

mtf_i32 MTF_ApeDemux::generate(MTF_Data*& oData)
{
	if (!(_oData._flags & MTF_DataFlag_ESO))
	{
		if (_isFirstFrame)
		{
			_isFirstFrame = false;
			_oData._flags |= MTF_DataFlag_EXTRA_INFO;
			MTF_MEM_CPY(_oData.LeftData(), &_extraInfo, sizeof(ExtraInfo_t));
			_oData._size += sizeof(ExtraInfo_t);
			oData = &_oData;
			return 0;
		}

		mtf_i32 readedSize = fread(_oData.LeftData(), 1, _oData.LeftSize(), (FILE*)_pFile);
		if (readedSize < _oData.LeftSize()) {
			_oData._flags |= MTF_DataFlag_ESO;
		}
		_oData._size += readedSize;
	}
	oData = &_oData;
	return 0;
}

mtf_i32 MTF_ApeDemux::Set(const char* key, mtf_void* val)
{
	if (MTF_String::StrCompare(key, "url"))
	{
		MTF_PRINT("url,%s", (const char*)val);
		_url = (const char*)val;

		return 0;
	}
	return MTF_AudioDemuxer::Set(key, val);
}
mtf_i32 MTF_ApeDemux::Get(const char* key, mtf_void* val)
{
	return MTF_AudioDemuxer::Get(key, val);
}
