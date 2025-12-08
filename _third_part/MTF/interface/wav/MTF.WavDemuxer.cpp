#include"MTF.WavDemuxer.h"
#include"MTF.Objects.h"
#include"MTF.Porting.h"
#include "WavDemux.h"

using namespace mtf_ns;
void mtf_wav_demuxer_register()
{
	MTF_Objects::Registe<MTF_WavDemuxer>("wav_demuxer");
}	


MTF_WavDemuxer ::MTF_WavDemuxer ()
{

}

MTF_WavDemuxer ::~MTF_WavDemuxer ()
{
	if (_pFile)
		FileClosePorting(_pFile);
	
	if (_oData.Data())
	{
		_oData.Used(_oData._size);
		MTF_FREE(_oData.Data());
	}
	if (_hd)
	{
#if 1
		WavDemux_DeInit(_hd);
		Free(_hd);
		Free(_basePorting);
#endif

	}
}

mtf_i32 MTF_WavDemuxer::Init()
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
	
#if 1
	WavDemuxInitParam initParam;
	MTF_MEM_SET(&initParam, 0, sizeof(WavDemuxInitParam));
#if 1
	_basePorting = Malloc(sizeof(AlgoBasePorting_t));
	AlgoBasePorting_t* basePorting = (AlgoBasePorting_t*)_basePorting;

	basePorting->Malloc = Malloc;
	basePorting->Free = Free;
	initParam.basePorting = basePorting;
#if 0
	initParam.fsHz = _rate;
	initParam.channels = _ch;
	initParam.width = _width;
	initParam.frameSamples = _frameSamples;
	initParam.overlapMs = _overlapMs;
	initParam.decayTimeMs = _decayMs;
#endif
#endif

	_hdSize = WavDemux_GetSize();
	_hd = Malloc(_hdSize);
	MTF_PRINT("_hd=%x,size:%d", (mtf_u32)_hd, _hdSize);
	if (!_hd){
		return -1;
	}

	mtf_i32 ret = WavDemux_Init(_hd, &initParam);

	if (ret < 0)
	{
		return -1;
	}
#endif

#if 1
	const mtf_i32 readDataByte = 1024;
	mtf_u8 readData[readDataByte];
	while (1){
		mtf_i32 readedSize = FileReadPorting(_pFile, readData, readDataByte);
		if (readedSize <= 0)
			return -1;
		
		MTF_Data iData;
		MTF_MEM_SET(&iData, 0, sizeof(MTF_Data));
		iData.Init(readData, readDataByte);
		iData._size += readDataByte;
#if 1
		mtf_i32 ret;
		ret = WavDemux_Run(_hd,
			iData.Data(),
			iData._size);
		if (ret != WAV_DEMUX_RET_SUCCESS){
			return -1;
		}
		iData.Used(iData._size);
		iData.Clear();
#endif
		mtf_u32 hasHead;
		//MAF_Get(_hd, "hasHead", (void**)&hasHead);
#if 1
		void* param[3] = { &hasHead };
		WavDemux_Get(_hd, WAV_DEMUX_GET_CHOOSE_HAS_HEAD, param);
#endif
		if ((mtf_bool)hasHead == true){
			mtf_i32 rate;
			mtf_i32 ch;
			mtf_i32 width;
			mtf_u32 dataPos;
			param[0] = &rate;
			param[1] = &ch;
			param[2] = &width;
			WavDemux_Get(_hd, WAV_DEMUX_GET_CHOOSE_BASIC_INFO, param);
			Set("rate", (void*)rate);
			Set("ch", (void*)ch);
			Set("width", (void*)width);

			WavDemux_Get(_hd, WAV_DEMUX_GET_CHOOSE_DATA_POS, (void**)&dataPos);
			FileSeekPorting(_pFile, dataPos, FILE_PORTING_SEEK_SET);
			break;
		}
	}
	mtf_i32 size = 1024;
	if (_frameBytes)
		size = _frameBytes;
	_oData.Init((mtf_u8*)MTF_MALLOC(size), size);
#endif
	return 0;
}


mtf_i32 MTF_WavDemuxer::generate(MTF_Data*& oData)
{
#if 1
	_oData.Clear();
	mtf_i32 readedSize = FileReadPorting(_pFile, _oData.LeftData(), _oData.LeftSize());
	//MTF_PRINT("%d,%d",_oData.LeftSize(), readedSize);
	if (readedSize <= 0){
		if (_oData._size <= 0)
			_oData._flags |= MTF_DataFlag_EMPTY;
		_oData._flags |= MTF_DataFlag_ESO;
	}
	_oData._size += readedSize;
	oData = &_oData;
	return 0;
#endif

}


mtf_i32 MTF_WavDemuxer ::Set(const char* key, mtf_void* val)
{
	if (MTF_String::StrCompare(key, "url"))
	{
		MTF_PRINT("url,%s", (const char*)val);
		_url = (const char*)val;

		return 0;
	}
	return MTF_AudioDemuxer::Set(key, val);
}
mtf_i32 MTF_WavDemuxer ::Get(const char* key, mtf_void* val)
{
	return MTF_AudioDemuxer::Get(key, val);
}




