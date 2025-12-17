#include "MTF.Data.h"
#include "MTF.String.h"
using namespace mtf_ns;
MTF_Data::MTF_Data()
{
}
MTF_Data::~MTF_Data()
{
}

mtf_i32 MTF_Data::Init(mtf_u8* buff, mtf_i32 len)
{
	_size = 0;
	_flags = 0;
	_buff = buff;
	_off = 0;
	_max = len;
	return 0;
}
mtf_i32 MTF_Data::DeInit()
{
	_size = 0;
	_flags = 0;
	_buff = 0;
	_off = 0;
	_max = 0;
	return 0;
}
mtf_i32 MTF_Data::Append(mtf_u8* buff, mtf_i32 len)
{
	MTF_MEM_CPY((mtf_i8*)Data(), (mtf_i8*)buff, len);
	_size += len;
	return 0;
}

mtf_void MTF_Data::Clear(mtf_i32 len)
{
	mtf_i32 off = _off > len ? len : _off;
	if (off > 0){
		MTF_MEM_MOVE((mtf_i8*)_buff, (mtf_i8*)_buff + off, _off +_size - off);
		_off -= off;
	}
}
mtf_void MTF_Data::Clear()
{
	Clear(_off);
}
mtf_u8* MTF_Data::Data()
{
	return _buff + _off;
}
mtf_u8* MTF_Data::LeftData()
{
	return _buff + _off + _size;
}
mtf_i32 MTF_Data::LeftSize()
{
	return _max - _off - _size;
}
mtf_i32 MTF_Data::Used(mtf_i32 size)
{
	size = size < _size ? size : _size;
	_off += size;
	_size -= size;
	return 0;
}
mtf_u8* MTF_Data::Buff() {
	return _buff;
}
