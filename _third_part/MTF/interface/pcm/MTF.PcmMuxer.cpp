#include "MTF.PcmMuxer.h"
#include "MTF.String.h"
#include "MTF.Objects.h"
#include "MTF.Porting.h"

void mtf_pcm_muxer_register()
{
	MTF_Objects::Registe<MTF_PcmMuxer>("pcm_muxer");
}

MTF_PcmMuxer::MTF_PcmMuxer()
{

}

MTF_PcmMuxer::~MTF_PcmMuxer()
{
	if (_pFile)
		FileClosePorting(_pFile);
	if (_iData.Data())
	{
		_iData.Used(_iData._size);
		MTF_FREE(_iData.Data());
	}
	
}


mtf_i32 MTF_PcmMuxer::Init()
{
	MTF_PRINT();
	if (!_url) {
		MTF_PRINT("error, _url = 0");
		return -1;
	}
	_pFile = FileOpenPorting(_url, "wb+");
	if (!_pFile) {
		MTF_PRINT("error, no such file:%s", _url);
		return -1;
	}

	mtf_i32 size = _frameBytes;
	_iData.Init((mtf_u8*)MTF_MALLOC(size), size);
	return 0;
}

mtf_i32 MTF_PcmMuxer::receive(MTF_Data& iData)
{
	if (iData._flags & MTF_DataFlag_ESO)
		return -1;
	FileWritePorting(_pFile, iData.Data(), iData._size);
	iData.Used(iData._size);
	return 0;
}

mtf_i32 MTF_PcmMuxer::Set(const char* key, mtf_void* val)
{
	if (MTF_String::StrCompare(key, "url"))
	{
		MTF_PRINT("url,%s", (const char*)val);
		_url = (const char*)val;
		return 0;
}
	return MTF_Sink::Set(key, val);
}
mtf_i32 MTF_PcmMuxer::Get(const char* key, mtf_void* val)
{
	return MTF_Sink::Get(key, val);
}



