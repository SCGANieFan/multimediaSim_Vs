#include <stdio.h>
#include "MTF.IOSource.h"
#include "MTF.String.h"
#include "MTF.Objects.h"

void mtf_io_source_register()
{
	MTF_Objects::Registe<MTF_IOSource>("io_source");
}

MTF_IOSource ::MTF_IOSource ()
{

}

MTF_IOSource ::~MTF_IOSource ()
{
	if (_pFile)
		fclose((FILE*)_pFile);

	if (_oData.Data())
	{
		_oData.Used(_oData._size);
		MTF_FREE(_oData.Data());
	}
	
}

mtf_int32 MTF_IOSource::Init()
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
	_oData.Init((mtf_uint8*)MTF_MALLOC(_oPortMax), _oPortMax);
	return 0;
}


mtf_int32 MTF_IOSource::generate(MTF_Data*& oData)
{
	if (!(_oData._flags & MTF_DataFlag_ESO))
	{
		mtf_int32 readedSize = fread(_oData.LeftData(), 1, _oData.LeftSize(), (FILE*)_pFile);
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


mtf_int32 MTF_IOSource ::Set(const mtf_int8* key, mtf_void* val)
{
	if (MTF_String::StrCompare(key, "url"))
	{
		MTF_PRINT("url,%s", (const mtf_int8*)val);
		_url = (const mtf_int8*)val;
		return 0;
	}
	else if (MTF_String::StrCompare(key, "oPortMax"))
	{
		_oPortMax = (mtf_uint32)val;
		if(_oPortMin>_oPortMax)
			_oPortMin = _oPortMax;
		return 0;
	}
	return MTF_Source::Set(key,val);
}
mtf_int32 MTF_IOSource ::Get(const mtf_int8* key, mtf_void* val)
{
	return MTF_Source::Get(key,val);
}




