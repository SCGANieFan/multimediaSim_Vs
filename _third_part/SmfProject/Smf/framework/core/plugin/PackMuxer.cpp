#include "PackMuxer.h"
#include "string.h"
#include "smf_debug.h"
#include "smf_media.h"
#include "smf_pack.h"
///////////////////////////////////////////////////////
using namespace smf;
using namespace smf::audio;
//
EXTERNC void smf_pack_muxer_register() {
	PackMuxer::Register<PackMuxer>("mux-pack");
}

PackMuxer::~PackMuxer() {
	Status(EStatus::null);
}
PackMuxer::PackMuxer() {
	////_openParamSize = sizeof(OpenParam);
	_iport->_min = 8;
	_iport->_max = 4096;
}

bool PackMuxer::open(void*para0) {
	returnIfErrC(false, !IOSink::open(para0));
	auto media = iMedia();
	returnIfErrC(false, !media);
	returnIfErrC(false, !media->_size);
	smf_packet_t pack;
	pack.seqNO = 0;
	pack.flags = SMF_PACKET_FLAGS_MEDIA;
	pack.payload_size = media->_size;
	smf_pack_crc_fill(&pack);
	auto size = _io->Write(&pack, sizeof(smf_packet_t));
	returnIfErrC(false, !size);
	returnIfErrC(false, size < sizeof(smf_packet_t));
	size = _io->Write(media, media->_size);
	returnIfErrC(false, !size);
	returnIfErrC(false, size < sizeof(smf_media_t));
	return true;
}

bool PackMuxer::close() {
	//dbgTestPL();
	return true;
}

bool PackMuxer::receiveFrame(Frame* frm) {
	// dbgTestPL();	
	returnIfErrC(false, !frm);
	auto data = (char*)frm->buff + frm->offset;
	if (smf_pack_crc_check(data)) {
		auto size = _io->Write(data, frm->size);
		returnIfErrC(false, !size);
		returnIfErrC(false, (int)size < frm->size);
	}
	else {
		smf_packet_t pack;
		pack.seqNO = frm->index;
		pack.flags = frm->flags & SMF_FRAME_PACKET_MASK;
		pack.payload_size = frm->size;
		smf_pack_crc_fill(&pack);
		auto size = _io->Write(&pack, sizeof(smf_packet_t));
		returnIfErrC(false, !size);
		returnIfErrC(false, size < sizeof(smf_packet_t));
		size = _io->Write(data, frm->size);
		returnIfErrC(false, !size);
		returnIfErrC(false, (int)size < frm->size);
	}
	return true;
}