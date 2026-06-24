#include <memory>
#include <stdio.h>
#include "IO.h"
#include "smf_debug.h"
#include "smf_api_param.h"
using namespace smf;
//
static int dbgDump0(unsigned ch, void* data, int size) {
	if (!data || !size)
		return 0;
	unsigned char* ptr = (unsigned char*)data;	
	if (size > 64)size = 64;
	auto rst = size;
	char buff[200];
	char* pbuf = buff;
	pbuf += sprintf(pbuf, "%04x:", size);
	while(size--)
		pbuf+=sprintf(pbuf, "%02x ", *ptr++);
	//
	*pbuf++ = '\n';
	*pbuf++ = 0;
	//int len = strlen(buff);
	int len = pbuf - buff;
	dbgOutput(ch, buff, len);
	return rst;
}
EXTERNC void dbgDumpFile(unsigned ch,const char* url) {
	IO* io = IO::Create<IO>("file");
	returnIfErrC0(!io);
	std::unique_ptr<IO> io0(io);
	//
	smf_io_param_t fp;
	fp.url = url;
	fp.para = "rb";
	returnIfErrCS0(!io->Open(&fp), "%s", url);
	returnIfErrC0(!io->IsOpen());
	char buff[64];
	while (1) {
		int size = io->Read(buff, sizeof(buff));
		returnIfErrC0(size <= 0);
		dbgDump0(ch, buff, size);
	}
}