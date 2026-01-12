#include "MTF.ApeDemux.h"
#include "MTF.String.h"
#include "MTF.Objects.h"
//#include "MTF.Porting.h"
#include "ApeDemux.h"
using namespace mtf_ns;
#define MIN(a,b) (a)<(b)?(a):(b)

mtf_void mtf_ape_demux_register()
{
	MTF_Objects::Registe<MTF_ApeDemux>("ape_demux");
}

static mtf_void* MallocLocal(int32_t size)
{
	static mtf_i32 sizeTotal = 0;
	sizeTotal += size;
	mtf_void* ptr = Malloc(size);
	MTF_PRINT("malloc, ptr:%x, size:%d, sizeTotal:%d,", (mtf_u32)ptr, size, sizeTotal);
	return ptr;
}

static mtf_void FreeLocal(mtf_void* block)
{
	MTF_PRINT("free, ptr:%x", (mtf_u32)block);
	return Free(block);
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
#if 0
		MAF_Deinit(_hd);
		MTF_FREE(_hd);
#else
		MTF_PRINT();
		ApeDemux_DeInit(_hd);
		if (_hd)
			Free(_hd);
		if (_basePorting)
			Free(_basePorting);
#endif
	}
	if (_pFile)
		FileClosePorting(_pFile);
}

mtf_i32 MTF_ApeDemux::Init()
{	
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

	//lib init
	MTF_PRINT();
	_hdSize = ApeDemux_GetSize();
	_hd = Malloc(_hdSize);
	MTF_PRINT("_hd=%x,size:%d", (mtf_u32)_hd, _hdSize);
	if (!_hd) {
		return -1;
	}
	mtf_i32 ret = ApeDemux_Init(_hd);
	if (ret < 0) {
		return -1;
	}

#if 1
	MTF_MEM_SET(&_extraInfo, 0, sizeof(ExtraInfo_t));

	//apeHeader
	const mtf_i32 readDataLen = 512;
	mtf_u8 readData[readDataLen];
	while (1){
		FileReadPorting(_pFile, readData, readDataLen);
#if 1
		//MAF_Run(_hd, &AA_iData, 0);
		ApeDemux_Run(_hd, readData, readDataLen);
#else
		mtf_i32 ret;
		ret = ApeDemux_Run(_hd, readData, readDataLen);
		if (ret < 0) {
			return -1;
		}
#endif
		mtf_u32 isRunFinish;
		ApeDemux_Get(_hd, ApeDemuxGet_e::APE_DEMUX_GET_IS_RUN_FINISH, (mtf_void**)&isRunFinish);
		if (isRunFinish) {
			ApeDemux_Get(_hd, ApeDemuxGet_e::APE_DEMUX_GET_HEADE, (mtf_void**)&_extraInfo.apeHeader);
			break;
		}
	}
	//apeHeader
	mtf_i32 seekTablePos;
	mtf_i32 seekTableSizeByte;
	mtf_void *param0[]={&seekTablePos,&seekTableSizeByte};
	ApeDemux_Get(_hd, ApeDemuxGet_e::APE_DEMUX_GET_SEEK_TABLE, (mtf_void**)param0);
	SeekTableManger seekTableManger;
	seekTableManger.Init(seekTablePos,seekTableSizeByte,_pFile);
	_startPos = 0;// 150 * 1024;
	mtf_u32 _startPosTmp = 0;
	while (1) {
	seekTableManger.UpdataSeektable();
	if (seekTableManger.GetValidSeekTableNum() == 0)
			break;
		mtf_void* param1[] = { (mtf_void*)seekTableManger.GetValidSeekTable(),(mtf_void*)seekTableManger.GetValidSeekTableByte() };
		int32_t ret = ApeDemux_Set(_hd, ApeDemuxSet_e::APE_DEMUX_SET_SEEK_TABLE, (mtf_void**)param1);
		if (ret != APERET_SUCCESS) return 0;
		_startPosTmp = _startPos;
		mtf_u32 seekTableNumOffset;
		mtf_void* param2[3] = { &_startPosTmp, &seekTableNumOffset ,&_extraInfo.skip };
		if (ApeDemux_Get(_hd, ApeDemuxGet_e::APE_DEMUX_GET_START_INFO_FROM_POS, (mtf_void**)param2) == APERET_SUCCESS){
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
	FileSeekPorting(_pFile, _startPos, FileSeekPorting_e::FILE_PORTING_SEEK_SET);
	_isFirstFrame = true;
#endif
	mtf_u32 rate;
	mtf_u32 ch;
	mtf_u32 width;
	mtf_void* param3[3] = { &rate,&ch, &width };
	ApeDemux_Get(_hd, ApeDemuxGet_e::APE_DEMUX_GET_AUDIO_INFO, (mtf_void**)param3);
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

		_oData.Clear();
		mtf_i32 readedSize = FileReadPorting(_pFile, _oData.LeftData(), _oData.LeftSize());
		
		if (readedSize <= 0) {
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
