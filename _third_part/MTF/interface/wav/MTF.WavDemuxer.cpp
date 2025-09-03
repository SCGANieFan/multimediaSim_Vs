#include<stdio.h>
#include"MTF.WavDemuxer.h"
#include"MTF.Objects.h"
#include"MAF.h"

void mtf_wav_demuxer_register()
{
	MTF_Objects::Registe<MTF_WavDemuxer>("wav_demuxer");
	MAF_REGISTER(wav_demux);
}	


MTF_WavDemuxer ::MTF_WavDemuxer ()
{

}

MTF_WavDemuxer ::~MTF_WavDemuxer ()
{
	if (_pFile)
		fclose((FILE*)_pFile);
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

mtf_int32 MTF_WavDemuxer::Init()
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
	
#if 1
	const mtf_int8* type = "wav_demux";
	MA_Ret ret;
	ret = MAF_GetHandleSize(type, &_hdSize);
	if (ret != MA_RET_SUCCESS)
		MTF_PRINT("err");
	if (_hdSize < 1)
		MTF_PRINT("err");
	_hd = MTF_MALLOC(_hdSize);
	if (!_hd)
		MTF_PRINT("err");
#if 1
	mtf_void* param[] = {
	(mtf_void*)type,
	(mtf_void*)MTF_Memory::Malloc,
	(mtf_void*)MTF_Memory::Realloc,
	(mtf_void*)MTF_Memory::Calloc,
	(mtf_void*)MTF_Memory::Free,
	};

	const mtf_int8* script = "type=$0,Malloc=$1,Realloc=$2,Calloc=$3,Free=$4"\
		";";
	ret = MAF_Init(_hd, script, param);
	if (ret != MA_RET_SUCCESS)
		MTF_PRINT("err");

#endif

#if 1
	const mtf_int32 readDataByte = 1024;
	mtf_uint8 readData[readDataByte];
	while (1){
		mtf_int32 readedSize = fread(readData, 1, readDataByte, (FILE*)_pFile);
		if (readedSize <= 0)
			return -1;

		AA_Data AA_iData;
		MTF_MEM_SET(&AA_iData, 0, sizeof(AA_Data));
		AA_iData.buff = readData;
		AA_iData.max = AA_iData.size = readDataByte;

		MAF_Run(_hd, &AA_iData, 0);

		mtf_uint32 hasHead;
		MAF_Get(_hd, "hasHead", (void**)&hasHead);
		if ((mtf_bool)hasHead == true){
			mtf_uint32 rate;
			mtf_uint32 ch;
			mtf_uint32 width;
			mtf_uint32 dataPos;

			MAF_Get(_hd, "rate", (void**)&rate);
			MAF_Get(_hd, "ch", (void**)&ch);
			MAF_Get(_hd, "width", (void**)&width);
			MAF_Get(_hd, "dataPos", (void**)&dataPos);

			Set("rate", (void*)rate);
			Set("ch", (void*)ch);
			Set("width", (void*)width);

			fseek((FILE*)_pFile, dataPos, SEEK_SET);
			break;
		}
	}

#endif
	mtf_int32 size = 1024;
	if (_frameBytes)
		size = _frameBytes;
	_oData.Init((mtf_uint8*)MTF_MALLOC(size), size);
#endif
	return 0;
}


mtf_int32 MTF_WavDemuxer::generate(MTF_Data*& oData)
{
#if 1
	mtf_int32 readedSize = fread(_oData.LeftData(), 1, _oData.LeftSize(), (FILE*)_pFile);
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


mtf_int32 MTF_WavDemuxer ::Set(const mtf_int8* key, mtf_void* val)
{
	if (MTF_String::StrCompare(key, "url"))
	{
		MTF_PRINT("url,%s", (const mtf_int8*)val);
		_url = (const mtf_int8*)val;

		return 0;
	}
	return MTF_AudioDemuxer::Set(key, val);
}
mtf_int32 MTF_WavDemuxer ::Get(const mtf_int8* key, mtf_void* val)
{
	return MTF_AudioDemuxer::Get(key, val);
}




