#include <memory>
#include <string.h>
#include "IO.h"
#include "Object.h"
#include "smf_common.h"
#include "smf_error.h"
#include "smf_debug.h"
#include "smf_api_param.h"
#include "SmfFCC.h"
#include "SmfDef.h"
#include "unique_ptr.h"
using namespace smf;
const char* smf_recognize_demuxer3(IO* io,const char*);

EXTERNC const char* smf_recognize_demuxer2(const char* url, void* iopara) {
	IO* io = IO::Create<IO>("io","file");
	returnIfErrC(0, !io);
	std::unique_ptr<IO> io0(io);
	//
	smf_io_param_t fp;
	fp.url = url;
	fp.para = "rb";
	fp.other = iopara;
	returnIfErrCS("open", !io->Open(&fp), "%s Open fail", url);
	returnIfErrCS("open", !io->IsOpen(), "%s IsOpen fail", url);

	return smf_recognize_demuxer3(io,url);
}
const char* smf_recognize_demuxer3(IO*io,const char* url) {
	returnIfErrC(0, !io);
	auto pos = io->GetOffset();
	uint32_t buff[4];
	uint8_t* buff8 = (uint8_t*)buff;
	int size = io->Read(buff, 16);
	returnIfErrCS("read", size != 16, "%s Read fail, size: %d", url, size);
	///parse
	const char* container;
	const char* ext = strrchr(url, '.');
	if (buff[0] == fcc32("RIFF")) {
		container = "wav";
	}
	else if (buff[1] == fcc32("ftyp")) {
		if (buff[2] == Swap32(0x4D344120)) {
			container = "m4a";
		}
		else if (buff[2] == Swap32(0x6D703432)) {
			container = "mp4";
		}
		else {
			container = "mp4";
		}
	}
	else if (buff[0] == fcc32("OggS")) {
		container = "ogg";
	}
	else if (buff[0] == fcc32("fLaC")) {
		container = "flac";
	}
	else if (buff[1] == fcc32("ILK_") || (buff[0] == FCC4('#', '!', 'S', 'I') && buff[1] == FCC4('L', 'K', '_', 'V'))){
		container = "silk";
	}
	else if (buff[0] == fcc32("MAC ")) {
		container = "ape";
	}
	else if ((buff[0] & 0xffffff) == fcc32("ID3")) {
		if(ext && fcc32(ext+1) == fcc32("aac")){
			container = "aac";
		}else{
			int id3leng = (((uint8_t*)&buff[1])[2] << 21) + (((uint8_t*)&buff[1])[3] << 14) + (((uint8_t*)&buff[2])[0] << 7) + ((uint8_t*)&buff[2])[1];
			returnIfErrC(0, !io->Seek(id3leng, Position::current));

			uint32_t buff2[1];
			size = io->Read(buff2, sizeof(buff2));
			returnIfErrCS(0, size != sizeof(buff2), "read error, %d!=%d", size, sizeof(buff2))
			if (buff2[0] == fcc32("fLaC")) {
				container = "flac";
			}
			else if ( (((uint8_t*)&buff2[0])[0] << 8) + (((uint8_t*)&buff2[0])[1] >> 4 << 4)  == 0xfff0) {
				container = "mp3";
			}
			else {
				dbgInfoPXL("unknow demuxer,try to use ext:%s", url);
				returnIfErrCS(0, !ext, "unrecognize demuxer :%s", url);
				container = ext + 1;
			}
		}
	}
	else if (((buff[0] & 0xf0ff) == 0xf0ff) && ((buff[1] & 0x00f00f00) == 0x00f00f00)) {
		returnIfErrCS(0, !ext, "unrecognize demuxer :%s", url)
		container = ext + 1;
		//container = "aac";
	}
	else if (buff[0] == fcc32("#!AM") && (buff[1] & 0x000000ff) == 'R') {
		container = "amr";
	}
	else if (buff8[0] == 0x9C && buff8[1] != 0x9C && buff8[2] != 0x9C && buff8[3] != 0x9C) {
		container = "sbc";
	}
	else if (buff8[0] == 0xAD && buff8[1] == 0 && buff8[2] == 0) {
		container = "msbc";
	}
	else {
		dbgInfoPXL("unknow demuxer,try to use ext:%s", url);
		returnIfErrCS(0, !ext, "unrecognize demuxer :%s", url)
		container = ext + 1;
	}
	dbgTestPXL("%s@%s", container, url);
	io->Seek(pos, Position::front);
	return container;
}

EXTERNC const char* smf_recognize_demuxer(const char* url) {
	return smf_recognize_demuxer2(url, 0);
}

EXTERNC bool smf_meta_parse(const char* url, smf_meta_info_t* pmeta, void* tempbuff, uint32_t tempsize) {
	returnIfErrE(false, !url, SMF_ERROR_INVALID_INPUT);
	returnIfErrE(false, !pmeta, SMF_ERROR_INVALID_INPUT);
	returnIfErrE(false, !tempbuff, SMF_ERROR_INVALID_INPUT);
	returnIfErrE(false, !tempsize, SMF_ERROR_INVALID_INPUT);
	//
	memset(pmeta, 0, sizeof(*pmeta));
	dbgApiPXL("%p(%p,%u)%s", pmeta, tempbuff, tempsize, url);
	//
#if 1
	const char* fmt = smf_recognize_demuxer(url);
	returnIfErrES(false, !fmt, SMF_ERROR_IO_FAIL, "file fmt error");
	dbgApiPSL(fmt);
	auto keys = fcc32(fmt);
	returnIfErrE(false, keys==fcc32("open"), SMF_ERROR_IO_OPEN_FAIL);
	returnIfErrE(false, keys==fcc32("read"), SMF_ERROR_IO_READ_FAIL);
	auto obj = Object::Create<Object>("dem", fmt);
	returnIfErrE(false, !obj, SMF_ERROR_UNSUPPORTED_AUDIO_DEMUXER);
	unique_ptr<Object> ptrobj(obj);
	returnIfErrE(false, !obj->Set("url", url), SMF_ERROR_DEMUX);
	returnIfErrE(false, !obj->Set("meta", pmeta), SMF_ERROR_DEMUX);
	returnIfErrE(false, !obj->Set("sts", 1), obj->Error().err);
#else
	const char* script = "_,pl,0{dem,dem,0,url=$0,meta=$1},sts=#1;";
	uint32_t params[]{ (uint32_t)url,(uint32_t)pmeta };
	auto obj = Deserialize(script, params);
	returnIfErrE(false, !obj, SMF_ERROR_FAIL);
	unique_ptr<Object> ptrobj(obj);
	uint32_t sts = 0;
	returnIfErrE(false, !obj->Get("sts",&sts), SMF_ERROR_FAIL);
	returnIfErrE(false, sts!=1, obj->Error().err);
#endif
	//
	auto ptr = (char*)tempbuff;
	auto end = (char*)tempbuff + tempsize;
	//
	pmeta->url = url;
	if (pmeta->timepoint) {
		auto data = pmeta->timepoint;
		auto size = sizeof(smf_progress_t);
		returnIfWarnE(false, ptr + size > end, SMF_ERROR_BUFF_WRITE_OVERFLOW);
		pmeta->timepoint = (smf_progress_t*)ptr;
		*pmeta->timepoint = *data;
		ptr += size;
	}
	if (pmeta->frameindex) {
		auto data = pmeta->frameindex;
		auto size = sizeof(smf_progress_t);
		returnIfWarnE(false, ptr + size > end, SMF_ERROR_BUFF_WRITE_OVERFLOW);
		pmeta->frameindex = (smf_progress_t*)ptr;
		*pmeta->frameindex = *data;
		ptr += size;
	}
	if (pmeta->album.data && pmeta->album.size) {
		auto data = pmeta->album.data;
		auto size = pmeta->album.size;
		returnIfWarnE(false, ptr + size > end, SMF_ERROR_BUFF_WRITE_OVERFLOW);
		pmeta->album.data = ptr;
		pmeta->album.size = size;
		memcpy(ptr, data, size);
		ptr += size;
	}
	if (pmeta->artist.data && pmeta->artist.size) {
		auto data = pmeta->artist.data;
		auto size = pmeta->artist.size;
		returnIfWarnE(false, ptr + size > end, SMF_ERROR_BUFF_WRITE_OVERFLOW);
		pmeta->artist.data = ptr;
		pmeta->artist.size = size;
		memcpy(ptr, data, size);
		ptr += size;
	}
	if (pmeta->title.data && pmeta->title.size) {
		auto data = pmeta->title.data;
		auto size = pmeta->title.size;
		returnIfWarnE(false, ptr + size > end, SMF_ERROR_BUFF_WRITE_OVERFLOW);
		pmeta->title.data = ptr;
		pmeta->title.size = size;
		memcpy(ptr, data, size);
		ptr += size;
	}
	return true;
}
