#include "PackDemuxer.h"
#include "smf_debug.h"
#include "smf_pack.h"
#include "unique_ptr.h"
///////////////////////////////////////////////////////
using namespace smf;
using namespace smf::audio;
//
EXTERNC void smf_pack_demuxer_register() {
	PackDemuxer::Register<PackDemuxer>("dem-pack");
}
//
PackDemuxer::~PackDemuxer() {
	Status(EStatus::null);
}
PackDemuxer::PackDemuxer(){
	////_openParamSize = sizeof(OpenParam);
	_oport->_min = 16;
	_oport->_max = 1024 * 4;
}
bool PackDemuxer::open(void* param) {	//dbgTestPL();	
	returnIfErrC(false, !IOSource::open(param));
	//
	smf_packet_t pack;
	auto size = _io->Read(&pack, sizeof(smf_packet_t));
	returnIfErrE(false, !size, SMF_ERROR_IO_EOS);
	returnIfErrE(false, size < sizeof(smf_packet_t), SMF_ERROR_IO_EOS);
	returnIfErrC(false, !smf_pack_crc_check(&pack));
	if (pack.flags & SMF_PACKET_FLAGS_MEDIA) {
		PoolPtr buff(Pool(), pack.payload_size);
		returnIfErrC(false, !buff);
		//returnIfErrCS(false, pack.payload_size != sizeof(smf_media_t),"%d!=%d", pack.payload_size ,sizeof(smf_media_t));
		auto size = _io->Read(buff, pack.payload_size);
		returnIfErrE(false, size < pack.payload_size, SMF_ERROR_IO_EOS);
		auto media = IMedia::Create((smf_media_def_t*)buff);
		returnIfErrC(false, !media);
		_oport->_media = media;
	}
	else {
		returnIfErrC(false, !_io->Seek(0-(int)sizeof(smf_packet_t), smf::Position::current));
	}
	return true;
}

bool PackDemuxer::close() {//dbgTestPL();
	return true;
}

//bool PackDemuxer::seek(int pos0) {	
//	return true;
//}

bool PackDemuxer::generateFrame(Frame*& frm) {
	auto data = (uint8_t*)frm->buff+frm->offset+frm->size;
	auto size = frm->max - frm->size - frm->offset;
	smf_packet_t pack;
	auto rsize = _io->Read(&pack, sizeof(smf_packet_t));
	returnIfErrE(false, !rsize, SMF_ERROR_IO_EOS);
	returnIfErrES(false, rsize< sizeof(smf_packet_t), SMF_ERROR_IO_EOS, "%d<%d", rsize , sizeof(smf_packet_t));
	returnIfErrES(false, size < pack.payload_size, SMF_ERROR_OUTPUT_BUFFER_NOT_ENOUGH, "%d<%d", size , pack.payload_size);
	rsize = _io->Read(data, pack.payload_size);
	returnIfErrE(false, !rsize, SMF_ERROR_IO_EOS);
	returnIfErrES(false, rsize < pack.payload_size, SMF_ERROR_IO_EOS, "%d<%d", rsize, pack.payload_size);
	frm->size += rsize;
	return true;
}

