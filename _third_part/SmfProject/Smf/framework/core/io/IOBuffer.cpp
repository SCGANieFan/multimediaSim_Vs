#include "IOBuffer.h"
#include "IOBufferList.h"
#include "SmfHash.h"
#include "smf_api_param.h"
#include "smf_debug.h"
using namespace smf;
EXTERNC void smf_io_buff_register() {
	IOBuffer::Register<IOBuffer>("io-buff");
}
IOBuffer::IOBuffer() 
	: _data(0)
	, _size(0)
	, _offset(0)

{
}
IOBuffer::IOBuffer(void* data, int size)
	: _data((char*)data)
	, _size(size)
	, _offset(0)

{
}
IOBuffer::~IOBuffer() {
	Close();
}
void IOBuffer::Reset(void* data, int size) {
	_data = (char*)data;
	_size = size;
	_offset = 0;
}
//status ctrl
bool IOBuffer::Open(void* param0) {
	auto param = (smf_io_param_t*)param0;
	returnIfErrC(false, !param->url);
	returnIfErrC(false, !param->para);
	char* ptr = (char*)strchr(param->url, '.');
	returnIfErrC(false, !ptr);
	char* endptr = (char*)param->url;
	_data = (char*)strtol(param->url, &endptr, 0);
	returnIfErrC(false, !_data);
	_size = strtol(endptr + 1, 0, 0);
	dbgIOPXL("%p,%d[%s]%s", _data,_size, param->para, param->url);
	// returnIfErrC(false, !_data);
	returnIfErrC(false, !_size);
	_offset = 0;
	return IO::Open(param0);
}
bool IOBuffer::Close() {
	_data = 0;
	_size = 0;
	_offset = 0;
	return IO::Close();
}
//status ctrl
bool IOBuffer::Seek(int offset, Position pos) {
	returnIfErrC(false, !_data);
	returnIfErrC(false, !_size);
	switch (pos) {
	case Position::current:
		_offset += offset;
		break;
	case Position::front:
		_offset = offset;
		break;
	case Position::end:
		_offset = _size - offset;
		break;
	}

	return true;
}
unsigned IOBuffer::GetSize() const {
	return _size;
}
unsigned IOBuffer::GetOffset() const {	
	return _offset;
}
unsigned IOBuffer::Read(void* buff, unsigned size) {
	returnIfErrC(0, !_data);
	returnIfErrC(0, !_size);
	if ((int)size > _size - _offset)
		size = _size - _offset;
	if(size)
		memcpy(buff, _data + _offset, size);
	_offset += size;
	return size;
}
unsigned IOBuffer::Write(void* buff, unsigned size) {
	returnIfErrC(0, !_data);
	returnIfErrC(0, !_size);
	returnIfNoteCS(0, _size == _offset, "EOS,%d",_offset);
	if ((int)size > _size - _offset)
		size = _size - _offset;
	memcpy(_data + _offset, buff, size);
	_offset += size;
	return size;
}
