#include "smf_debug.h"
#include "BypassMuxer.h"
#include "smf_muxer.h"
///////////////////////////////////////////////////////
using namespace smf;
using namespace smf::audio;
//
EXTERNC void smf_bypass_muxer_register() {
	BypassMuxer::Register<BypassMuxer>("mux-bypass");
}
EXTERNC void smf_pcm_muxer_register() {
	BypassMuxer::Register<BypassMuxer>("mux-pcm");
}
BypassMuxer::~BypassMuxer() {
	Status(EStatus::null);
}
BypassMuxer::BypassMuxer() {
	_iport->_min = 8;
	_iport->_max = 4096;
}
bool BypassMuxer::open(void*para0) {
	return IOSink::open(para0);
}
bool BypassMuxer::close() {
	return IOSink::close();
}
bool BypassMuxer::receiveFrame(Frame* frm) {
	returnIfErrC(false, !_io);
	if (frm->size == 0) {
		return true;
	}
	auto data = (char*)frm->Data();
	auto size = frm->size;
	auto wsize = _io->Write(data, size);
	returnIfErrC(false, wsize < 0);
	frm->size -= wsize;
	// frm->offset += wsize;
	// dbgTestPXL("%d", frm->size);
	return true;
}
bool BypassMuxer::setParam(void* para0) {
	auto para = (smf_muxer_param_t*)para0;
	returnIfErrC(false, !para);
	//returnIfErrC(false, para->size!=sizeof(smf_iosource_param_t));
	// auto media = new MediaPcm(*(MediaPcm*)&para->media);
	// _iport->_media = media;
	return IOSink::setParam(para0);
}
