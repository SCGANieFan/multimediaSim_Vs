#include "IOCallback.h"
#include "smf_api_fs.h"

using namespace smf;

EXTERNC void smf_fs_callback_register(
	CbFOpen fopen
	, CbFClose fclose
	, CbFRead fread
	, CbFWrite fwrite
	, CbFSeek fseek
	, CbFTell ftell
	, CbFLength flength
) {
	smf_io_callback_register("io-file", fopen, fclose, fread, fwrite, fseek, ftell, flength);
}

EXTERNC void smf_io_callback_register(const char* name
	, CbFOpen fopen
	, CbFClose fclose
	, CbFRead fread
	, CbFWrite fwrite
	, CbFSeek fseek
	, CbFTell ftell
	, CbFLength flength
) {
	auto cb = new IOCallback::callback_t();
	cb->_fopen = fopen;
	cb->_fclose = fclose;
	cb->_fread = fread;
	cb->_fwrite = fwrite;
	cb->_fseek = fseek;
	cb->_ftell = ftell;
	cb->_flength = flength;
	dbgTestPXL("alloc:%p,%u", cb, sizeof(IOCallback::callback_t));
	Object::Reflect().Register(name, []() {
		return (Object*)new IOCallback();
		}, cb);
}
