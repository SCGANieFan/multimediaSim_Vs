#include "LogRecord.h"
#include "smf_debug.h"
using namespace smf;

LogRecord::LogRecord(int max, int ch)
	: _ch(ch)
	, _max(max)
{
}
LogRecord::~LogRecord() {
	Clear();
}
void LogRecord::Set(void* buff, int size) {
	_begin = (char*)buff;
	_end = _begin + size - 4;// sizeof(_buff);
	_ptr = _begin;
}
void LogRecord::Append(const char* fmt, va_list& ap) {
	returnIfErrC0(!_max);
	if (!_begin) {
		if (!_buff) {
			_buff = (char*)Alloc(_max);
		}
		returnIfErrC0(!_buff);
		Set(_buff, _max);
	}

	_ptr += vsnprintf(_ptr, _end - _ptr, fmt, ap);
	if (_ptr >= _end)_ptr = _end;

}
void LogRecord::Append(const char* fmt, ...) {
	va_list ap;
	va_start(ap, fmt);
	Append(fmt, ap);
	va_end(ap);
}
void LogRecord::Output() {
	if (!_begin)return;
	if (!_ptr)return;
	*_ptr++ = '\n';
	*_ptr++ = 0;
	dbgOutput(_ch, _begin, _ptr - _begin - 1);
	_ptr = _begin;
}
void LogRecord::Clear() {
	if(_buff)
		Free((void*&)_buff);
	_begin = _end = _ptr = _buff = 0;
}

