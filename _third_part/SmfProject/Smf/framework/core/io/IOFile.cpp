#include "IOFile.h"
#include "SmfFCC.h"
#include "smf_debug.h"
using namespace smf;
EXTERNC void smf_io_file_register() {
	IOFile::Register<IOFile>("io-file");
}
EXTERNC void smf_io_fs_register() {
	smf_io_file_register();
}
EXTERNC void smf_fs_register() {
	smf_io_file_register();
}
EXTERNC void smf_std_fs_register() {
	smf_io_file_register();
}
//
IOFile::IOFile()
	: _hd(0)
{
	_id = fcc64("iofile");
}
IOFile::~IOFile() {
	if(_hd)
		Close();
}
//status ctrl
bool IOFile::Open(void* param0) {
	OpenParam* param = (OpenParam*)param0;
	returnIfErrC(false, !param->url);
	returnIfErrC(false, !param->para);
	_hd = fopen(param->url,param->para);
	dbgIOPXL(" %p[%s]%s", _hd, param->para, param->url);
	returnIfErrCS(false, !_hd, "%s,%s", param->para, param->url);
	return IO::Open(param0);
}
bool IOFile::Close() {
	if (_hd) {
		dbgIOPXL("%p", _hd);
		_error.err = fclose(_hd);
		_hd = 0;
	}
	return IO::Close();
}
//status ctrl
bool IOFile::Seek(int offset, Position pos) {
	returnIfErrC(false, !_hd);
	return !fseek(_hd, offset, (int)pos);
}
unsigned IOFile::GetSize() const {
	returnIfErrC(0, !_hd);
	unsigned pos = GetOffset();
	returnIfErrC(0, !((IO*)this)->Seek(0, Position::end));
	unsigned size = GetOffset();
	returnIfErrC(0, !((IO*)this)->Seek(pos, Position::front));
	return size;
}
unsigned IOFile::GetOffset() const {
	returnIfErrC(0, !_hd);
	return ftell(_hd);
}
unsigned IOFile::Read(void* buff, unsigned size) {
	returnIfErrC(false, !_hd);
	return fread(buff, 1,size, _hd);
}
unsigned IOFile::Write(void* buff, unsigned size) {
	returnIfErrC(false, !_hd);
	return fwrite(buff, 1,size,_hd);
}

void IOFile::dbgErrProcess(const char* file, unsigned line, unsigned error)const {
	_error.err64 = 0;
	error = SMF_ERROR_IO_FAIL | (_hd ? (ferror(0) << 16) : 0);
	IO::dbgErrProcess(file, line, error);
}
