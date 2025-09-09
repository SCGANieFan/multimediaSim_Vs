#include <stdio.h>
#include "MTF.PcmDemuxer.h"
#include "MTF.String.h"
#include "MTF.Objects.h"

void mtf_pcm_demuxer_register()
{
	MTF_Objects::Registe<MTF_PcmDemuxer>("pcm_demuxer");
}

MTF_PcmDemuxer ::MTF_PcmDemuxer ()
{

}

MTF_PcmDemuxer ::~MTF_PcmDemuxer ()
{
	if (_pFile)
		fclose((FILE*)_pFile);

	if (_oData.Data())
	{
		_oData.Used(_oData._size);
		MTF_FREE(_oData.Data());
	}
	
}

mtf_i32 MTF_PcmDemuxer::Init()
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

	mtf_i32 size = _frameBytes;
	_oData.Init((mtf_u8*)MTF_MALLOC(size), size);
	
	
	return 0;
}


mtf_i32 MTF_PcmDemuxer::generate(MTF_Data*& oData)
{
	if (!(_oData._flags & MTF_DataFlag_ESO))
	{
		mtf_i32 readedSize = fread(_oData.LeftData(), 1, _oData.LeftSize(), (FILE*)_pFile);
		if (readedSize < _oData.LeftSize()) {
			_oData._flags |= MTF_DataFlag_ESO;
		}
		_oData._size += readedSize;
		if (_oData._size <= 0)
			_oData._flags |= MTF_DataFlag_EMPTY;
		oData = &_oData;
	}

	return 0;
}


mtf_i32 MTF_PcmDemuxer ::Set(const char* key, mtf_void* val)
{
	if (MTF_String::StrCompare(key, "url"))
	{
		MTF_PRINT("url,%s", (const char*)val);
		_url = (const char*)val;

		return 0;
	}
	return MTF_AudioDemuxer::Set(key, val);
}
mtf_i32 MTF_PcmDemuxer ::Get(const char* key, mtf_void* val)
{
	return MTF_AudioDemuxer::Get(key, val);
}




