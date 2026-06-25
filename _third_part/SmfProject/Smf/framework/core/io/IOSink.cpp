#include "IOSink.h"
#include "Source.h"
#include "SmfHash.h"
#include "smf_debug.h"
#include "smf_muxer.h"

using namespace smf;
///
EXTERNC void smf_io_sink_register() {
	IOSink::Register<IOSink>("sink-io");
}
///
IOSink::IOSink() {
}
IOSink::~IOSink() {
	Status(EStatus::null);
	if (_io)delete _io;
}
/// 
bool IOSink::open(void*) {
	if (!_io) {
		_io = _url.Create("wb");
	}
	returnIfErrC(false, !_io);
	_io->Parent(this);
	//smf_io_param_t fp;
	//fp.url = _url._path;
	//fp.para = "wb";
	//returnIfErrC(false, !_io->Open(&fp));
	//returnIfErrC(false, !_io->IsOpen());
	//
	//_writer.Parent(this);
	//IOCacheReader::OpenParam op{ _io,_ioReadCacheLen, _ioReadAlign, true };
	//returnIfErrC(false, !_writer.Open(&op));
	return true;
}
bool IOSink::close() {
	//_writer.Close();
	if (_io) { delete _io; _io = 0; }
	return true;
}
bool IOSink::receiveFrame(Frame*frm) {
	int size = frm->size;
	returnIf(false, !size);
	//if (size > 128)size = 128;
	auto wsize = _io->Write((char*)frm->buff + frm->offset, size);
	returnIfNoteE(false, !wsize, SMF_ERROR_IO_EOS);
	frm->size -= wsize;
	frm->offset += wsize;
	//dbgTestPXL("%s[%d]%u,%d,%d", Name(),frm->index, wsize,frm->size,frm->offset);
	return true;
}
bool IOSink::set(uint32_t key, void* val) {
	switch (key) {
	case Hash("io"): _io = (IO*)val; return true;
	case Hash("url"): return _url.Parse((const char*)val);
	default:break;
	}
	return Sink::set(key, val);
}

bool IOSink::setParam(void* para0) {
	auto para = (smf_iosink_param_t*)para0;
	returnIfErrC(false, !para);
	if (para->media) {
		if (_iport->_media)delete _iport->_media;
		_iport->_media = IMedia::Create((smf_media_def_t*)para->media);
	}
	//returnIfErrC(false, para->size!=sizeof(smf_iosource_param_t));
	if (para->url)_url.Parse(para->url);
	//returnIfErrC(false, para->size!=sizeof(smf_iosource_param_t));
	if (para->io)_io = (IO*)para->io;
	//if (para->ioReadCacheLen)_ioCacheLen = para->ioReadCacheLen;
	//if (para->ioReadAlign)_ioCacheAlign = para->ioReadAlign;
	//_packed = para->packed;
	//_progress = para->progress;
	//_meta = para->meta;
	
	return true;
}