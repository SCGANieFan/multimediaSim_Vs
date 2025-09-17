#include "MTF.IOSource.h"
#include "MTF.String.h"
#include "MTF.Objects.h"
#include "MTF.Porting.h"
using namespace mtf_ns;
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
		FileClosePorting(_pFile);

	if (_oData.Data())
	{
		_oData.Used(_oData._size);
		MTF_FREE(_oData.Data());
	}
	
}

mtf_i32 MTF_IOSource::Init()
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
	_oData.Init((mtf_u8*)MTF_MALLOC(_oPortMax), _oPortMax);
	return 0;
}


mtf_i32 MTF_IOSource::generate(MTF_Data*& oData)
{
	if (!(_oData._flags & MTF_DataFlag_ESO))
	{
		mtf_i32 readedSize = FileReadPorting(_pFile, _oData.LeftData(), _oData.LeftSize());
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


mtf_i32 MTF_IOSource ::Set(const char* key, mtf_void* val)
{
	if (MTF_String::StrCompare(key, "url"))
	{
		MTF_PRINT("url,%s", (const char*)val);
		_url = (const char*)val;
		return 0;
	}
	else if (MTF_String::StrCompare(key, "oPortMax"))
	{
		_oPortMax = (mtf_u32)val;
		if(_oPortMin>_oPortMax)
			_oPortMin = _oPortMax;
		return 0;
	}
	return MTF_Source::Set(key,val);
}
mtf_i32 MTF_IOSource ::Get(const char* key, mtf_void* val)
{
	return MTF_Source::Get(key,val);
}




