#include <stdio.h>
#include "MTF.IOSink.h"
#include "MTF.String.h"
#include "MTF.Objects.h"

void mtf_io_sink_register()
{
	MTF_Objects::Registe<MTF_IOSink>("io_sink");
}

MTF_IOSink::MTF_IOSink()
{

}

MTF_IOSink::~MTF_IOSink()
{
	if (_pFile)
		fclose((FILE*)_pFile);
#if 0
	if (_iData.Data())
	{
		_iData.Used(_iData._size);
		MTF_FREE(_iData.Data());
	}
#endif
}


mtf_i32 MTF_IOSink::Init()
{
	MTF_PRINT();
	if (!_url) {
		MTF_PRINT("error, _url = 0");
		return -1;
	}
	_pFile = fopen(_url, "wb+");
	if (!_pFile) {
		MTF_PRINT("error, no such file:%s", _url);
		return -1;
	}
#if 0
	_iData.Init((mtf_u8*)MTF_MALLOC(_iPortMax), _iPortMax);
#endif
	return 0;
}

mtf_i32 MTF_IOSink::receive(MTF_Data& iData)
{
	if (iData._flags & MTF_DataFlag_ESO)
		return -1;
	fwrite(iData.Data(), 1, iData._size, (FILE*)_pFile);
	iData.Used(iData._size);
	return 0;
}

mtf_i32 MTF_IOSink::Set(const char* key, mtf_void* val)
{
	if (MTF_String::StrCompare(key, "url"))
	{
		MTF_PRINT("url,%s", (const char*)val);
		_url = (const char*)val;
		return 0;
	}
	else if (MTF_String::StrCompare(key, "iPortMax"))
	{
		_iPortMax = (mtf_u32)val;
		if(_iPortMin>_iPortMax)
			_iPortMin = _iPortMax;
		return 0;
	}
	return MTF_Sink::Set(key, val);
}
mtf_i32 MTF_IOSink::Get(const char* key, mtf_void* val)
{
	return MTF_Sink::Get(key, val);
}



