#include "IOCallback.h"
#include "smf_debug.h"
#include "SmfPorting.h"
using namespace smf;
//
IOCallback::IOCallback(FOpen fopen_
	, FClose fclose_
	, FRead fread_
	, FWrite fwrite_
	, FSeek fseek_
	, FTell ftell_
	, FLength flength_	
	//, FEof feof_
	//, FError ferror_
)
	: _hd(0)
	
{
	_fopen = fopen_;
	_fclose = fclose_;
	_fread = fread_;
	_fwrite = fwrite_;
	_ftell = ftell_;
	_flength = flength_;
	_fseek = fseek_;
	//_ticks = 0;
	//_tickTotal = 0;
	//_feof(feof_)
	//_ferror(ferror_)
}
IOCallback::~IOCallback() {
	if (_hd) {
		//dbgTestPXL("%d", GetOffset());
		Close();
	}
	//dbgTestPXL("%uticks,%ums",_tickTotal, ticks_to_ms(_tickTotal));
}
void IOCallback::initialize(Reflection::item_t*item) {
	Object::initialize(item);
	if (item->param) {
		auto cb = (callback_t*)item->param;
		_fopen = cb->_fopen;
		_fclose = cb->_fclose;
		_fread = cb->_fread;
		_fseek = cb->_fseek;
		_fwrite = cb->_fwrite;
		_ftell = cb->_ftell;
		_flength = cb->_flength;
	}
}
//status ctrl
bool IOCallback::Open(void* param0) {
	returnIfErrC(false, _hd);
	OpenParam* param = (OpenParam*)param0;
	returnIfErrC(false, !param->url);
	returnIfErrC(false, !param->para);
	returnIfErrC(false, !_fopen);
	_hd = _fopen(param->url,param->para);
	returnIfErrC(false, !_hd);
	//dbgIOPXL("%p[%s]%s", _hd, param->para, param->url);
	dbgIOPXL("%p[%s]%s,%d", _hd, param->para, param->url, GetSize());
	return IO::Open(param0);
}
bool IOCallback::Close() {
	returnIfErrC(false, !_hd);
	dbgIOPXL(_hd);
	returnIfErrC(false, !_fclose);
	returnIfErrC(false, _fclose(_hd));
	_hd = 0;
	return IO::Close();
}
//status ctrl
bool IOCallback::Seek(int offset, Position pos) {
	returnIfErrC(false, !_hd);
	returnIfErrC(false, !_fseek);
	return !_fseek(_hd, offset, (int)pos);
}
unsigned IOCallback::GetSize() const {
	returnIfErrC(0, !_hd);
	if (_flength) {
		return _flength(_hd);
	}
	unsigned pos = GetOffset();
	returnIfErrC(0, !((IO*)this)->Seek(0, Position::end));
	unsigned size = GetOffset();
	returnIfErrC(0, !((IO*)this)->Seek(pos, Position::front));
	return size;
}
unsigned IOCallback::GetOffset() const {
	returnIfErrC(0, !_hd);
	returnIfErrC(0, !_ftell);
	return _ftell(_hd);
}
unsigned IOCallback::Read(void* buff, unsigned size) {

	returnIfErrC(false, !_hd);
	returnIfErrC(false, !_fread);//dbgTestPXL("%p,%p",GetOffset(),size);
	auto rst = _fread(buff, 1,size, _hd);//dbgTestPXL("%p,%p",rst,size);
    return rst;
}
unsigned IOCallback::Write(void* buff, unsigned size) {
	returnIfErrC(false, !_hd);
	returnIfErrC(false, !_fwrite);
	return _fwrite(buff, 1,size,_hd);
}