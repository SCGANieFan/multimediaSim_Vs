#include "smf_error.h"
#include "smf_debug.h"
#include "smf_common.h"
#include "SmfHash.h"
#include "SmfDef.h"
#include <stdio.h>
#include <string.h>
using namespace smf;
EXTERNC void smf_error_print(smf_error_t*err) {
	if (!err) return;
	if (!err->err64) return;
	const char* head = 0;
	switch (err->errid){
	case SMF_ERROR_IO_EOS:
		head = "EOS";
		break;
	default:
		head = "ERR";
		break;
	}
	dbgNoteX("[%s]0x%08X%08X,err=0x%04X,sub=0x%04X,file=0x%04X,line=%u\n"
		, head
		, err->err32[1]
		, err->err32[0]
		, err->err, err->sub
		, err->file 
		, err->line
	);
}
///dumy function
//static smf_error_t _smf_err;
extern "C" void dbgErrProcess(const char* file, unsigned line, unsigned error) {
	auto v = Hash(file);
	auto& _smf_err = Error();
	_smf_err.file = (uint16_t)((v>>16) ^ (v & 0xffff));//dbgFileNameHash(file);
	_smf_err.line = line;
	_smf_err.err = error & 0xffff;
	_smf_err.sub = error >> 16;
}
extern "C" uint64_t smf_get_last_error() {
	auto& _smf_err = Error();
	return _smf_err.err64;
}
extern "C" uint16_t smf_get_last_error16() {
	auto& _smf_err = Error();
	return _smf_err.err;
}