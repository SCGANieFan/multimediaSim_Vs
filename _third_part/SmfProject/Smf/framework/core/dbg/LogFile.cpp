#include "LogFile.h"
#include <string.h>
#include <stdlib.h>
#include "smf_api_param.h"
using namespace smf;
LogFile::LogFile() {
	memset(_ios, 0, sizeof(_ios));
	_io=0;
	_path=0;
}
LogFile::~LogFile() {
	for (int i = 0; i < 16; i++) {
		IO* io = _ios[i];
		if (io) {
			delete io;
		}
	}
	if (_io) {
		delete _io;
	}
}
int LogFile::Output(const char* str, int size, int ch) {
	if (ch > 16 || ch < 0)return 0;
	output(_ios[ch],str,size,ch);
	output(_io, str, size, 0xff);
	return 0;
}
int LogFile::output(IO*&io,const char* str, int size, int ch) {
	if (!io) {
		char fname[128];
		snprintf(fname, sizeof(fname), "%s/test_%x.log", _path, ch);
		io = IO::Create<IO>("file");
		if (io) {
			smf_io_param_t para;
			para.url = fname;
			para.para = "wb";
			if (!io->Open(&para)) {
				delete io;
				io = 0;
			}
		}
	}
	if (io) {
		return io->Write((void*)str, size);
	}
	return 0;
}