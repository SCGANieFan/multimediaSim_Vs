#include "URL.h"
#include "SmfFCC.h"
#include "SmfHash.h"
#include "SmfPorting.h"
#include "SmfPortingFs.h"
#include <string.h>
#include "smf_api_param.h"
#include "smf_debug.h"
using namespace smf;

URL::URL(const char* url) {
	Parse(url);
}
URL::~URL() {
	Free(_buff);
}
void URL::Reset() {
	Free(_buff);
	_buff = 0;
	_url = 0;
	_type = 0;
	_path = 0;
	_ext = 0;
}
bool URL::Parse(const char* url) {
	returnIf(false, !url);
	//auto path = url;
	auto type = "file";
	//dbgTestPSL(url);
	//file://d:/audio.mp3##wb
	auto len = strlen(url);
	auto size = (len + 1) * 3 + 512;
	_buff = (char*)Alloc(size);
	returnIfErrC(false, !_buff);
	memcpy(_buff, url, len);
	_buff[len] = 0;
	memcpy(_buff + len + 1, url, len);
	_buff[len + 1 + len] = 0;
	auto buff = _buff + len + 1;
	auto end = _buff + size;
	auto path = buff;
	// returnIfErrC(false, !tmp);
#if 0
	if (fcc32(path) == fcc32("file")) {
		path += 7;
	}
#else
	auto tmp = strchr(buff, ':');
	if(tmp){
		if (tmp[1] == '/' && tmp[2] == '/') {
			if (tmp - buff != 1) {//skip window disk (d://...)
				*tmp = 0;
				tmp += 3;
				path = tmp;
				type = buff;
				//
				tmp = path;
				do {
					tmp = strchr((char*)tmp, '#');//##
					if (!tmp) break;
					if (!strncmp(tmp, "#para:", 6)) {
						*tmp = 0;
						tmp += 6;
						_para = tmp;
					}
					else if (!strncmp(tmp, "#script:", 8)) {
						*tmp = 0;
						tmp += 8;
						_script = tmp;
					}
					else {
						tmp++;
					}
				} while (1);
			}
		}
	}
#endif
	//
	switch (Hash(type)) {
	case Hash("fileI"): {
		char* xx = end - 256;
		strcpy(xx, path);
		sprintf((char*)path, xx, UniqueID());
		type = "file";
		break;
	}
	case Hash("fileD"): {
		char* xx = end - 256;
		strcpy(xx, path);
		rtc_printf((char*)path, end, xx);
		type = "file";
		break;
	}
	case Hash("fileX"): {
		char* rtc = end - 256;
		char* pathx = rtc - 256;
		rtc_printf(rtc, end, "%04d%02d%02d_%02d%02d%02d_%03d");
		strcpy(pathx, path);
		sprintf((char*)path, pathx, rtc);
		type = "file"; 
		break;
	}
	case Hash("http"):
	case Hash("https"): {
		path = _buff;
		dbgTestPSL(path);
		break;
	}
	}
	//
	_url = _buff;
	_type = type;
	_path = path;
	_ext = 0;

	//if (fcc32(type) == fcc32("file")) {
		tmp = strrchr((char*)path, '.');
		if (tmp) {
			_ext = tmp + 1;
		}
	//}
	return true;
}

IO* URL::Create(const char* para) const {
	returnIfErrC(0, !_type);
	auto io = IO::Create<IO>("io",_type);
	returnIfErrC(0, !io);
	std::unique_ptr<IO> iox(io);
	smf_io_param_t fp;
	fp.url = _path; dbgTestPXL("%s", _path);
	fp.para = para ? para : _para;
	fp.other = _other;
	returnIfErrC(0, !io->Open(&fp));
	return iox.release();
}

bool URL::Read(void* data, int& size, int& offset) const {
	auto io = Create("rb");
	returnIfErrC(false, !io);
	unique_ptr<IO> iox(io);
	if(!size)
		size = io->GetSize();
	returnIfErrC(false, !size);
	if (offset)
		returnIfErrC(false, !io->Seek(offset, Position::front));
	auto rsize = io->Read(data, size);
	//returnIfErrC(false, rsize != size);
	offset = io->GetOffset();
	size = rsize;
	return true;
}

bool URL::Read(void* data, int& size) const {
	int offset = 0;
	return Read(data,size, offset);
}

bool URL::Read(VoidBuffer& buff) const {
	auto io = Create("rb");
	returnIfErrC(false, !io);
	unique_ptr<IO> iox(io);
	auto size = io->GetSize();
	returnIfErrC(false, !size);
	buff.reset(size);
	auto rsize = io->Read(buff.get(), size);
	returnIfErrC(false, rsize != size);
	return true;
}

bool URL::Write(const void* data, int size, int offset) const {
	returnIfErrC(false, !size);
	auto io = Create(offset ? "ab" : "wb");
	returnIfErrC(false, !io);
	unique_ptr<IO> iox(io);
	if (offset == 0) {
	}
	else if (offset < 0) {
		returnIfErrC(false, !io->Seek(0, Position::end));
	}
	else {//offset>0
		returnIfErrC(false, !io->Seek(offset, Position::front));
	}
	auto rsize = io->Write((void*)data, size);
	returnIfErrC(false, rsize != size);
	return true;
}

IO* URL::Create(const char* url, const char* para) {
	URL urlx(url);
	return urlx.Create(para);
}

bool URL::Read(const char* url, void* data, int& size) {
	URL urlx(url);
	return urlx.Read(data, size);
}

bool URL::Read(const char* url, void* data, int& size, int& offset) {
	URL urlx(url);
	return urlx.Read(data, size, offset);
}

bool URL::Read(const char* url, VoidBuffer& buff) {
	URL urlx(url);
	return urlx.Read(buff);
}

bool URL::Write(const char* url, const void* data, int size, int offset) {
	URL urlx(url);
	return urlx.Write(data, size, offset);
}

bool URL::Exist(const char* url) {
	return file_exist(url);
}