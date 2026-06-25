#include "URL.h"
#include "smf_api.h"
using namespace smf;

EXTERNC bool testIOBuffer() {
	smf_init();
	SMF_REGISTER(io_buff);
	char buff[64];
	char name[32];
	sprintf(name, "buff://0x%08x.%d.wav", (uint32_t)buff,(int)sizeof(buff));
	dbgTestPSL(name);
	URL url;
	returnIfErrC(false,!url.Parse(name));
	dbgTestPSL(url._url);
	dbgTestPSL(url._type);
	dbgTestPSL(url._path);
	dbgTestPSL(url._ext);
	dbgTestPSL(url._para);

	auto io = url.Create("rb");
	returnIfErrC(false, !io);

	char data[16];
	auto str = (char*)"12345678";
	returnIfErrC(false, io->Write(str, 4)!=4);
	returnIfErrC(false, !io->Seek(0,Position::front));
	returnIfErrC(false, io->Read(data, 4)!=4);

	returnIfErrC(false, !io->Seek(12, Position::front));
	returnIfErrC(false, io->Write(str, 8) != 8);
	returnIfErrC(false, !io->Seek(12, Position::front));
	returnIfErrC(false, io->Read(data, 8) != 8);

	return true;
}

EXTERNC bool testUrlParse() {
	smf_init();
	URL url;
	returnIfErrC(false, !url.Parse("file://D://work/audio/48kx2x1s.wav"));
	dbgTestPSL(url._url);
	dbgTestPSL(url._type);
	dbgTestPSL(url._path);
	dbgTestPSL(url._ext);
	dbgTestPSL(url._para);

	returnIfErrC(false, !url.Parse("D://work/audio/48kx2x1s.wav"));
	dbgTestPSL(url._url);
	dbgTestPSL(url._type);
	dbgTestPSL(url._path);
	dbgTestPSL(url._ext);
	dbgTestPSL(url._para);

	returnIfErrC(false, !url.Parse("/work/audio/48kx2x1s.wav"));
	dbgTestPSL(url._url);
	dbgTestPSL(url._type);
	dbgTestPSL(url._path);
	dbgTestPSL(url._ext);
	dbgTestPSL(url._para);

	returnIfErrC(false, !url.Parse("file:///work/audio/48kx2x1s.wav"));
	dbgTestPSL(url._url);
	dbgTestPSL(url._type);
	dbgTestPSL(url._path);
	dbgTestPSL(url._ext);
	dbgTestPSL(url._para);

	returnIfErrC(false, !url.Parse("buff://0x12345678.128.wav"));
	dbgTestPSL(url._url);
	dbgTestPSL(url._type);
	dbgTestPSL(url._path);
	dbgTestPSL(url._ext);
	dbgTestPSL(url._para);

	return true;
}

EXTERNC bool testUrl() {
	testUrlParse();
	testIOBuffer();
	return true;
}