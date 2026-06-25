#include "SmfPackage.h"
#include "smf_pack.h"
#include "smf_debug.h"

using namespace smf;
bool SmfPackage::Pack(void* pack0, uint32_t size, uint32_t flags) {
	auto pack = (smf_packet_t*)pack0;
	pack->seqNO = _index++ & 0xf;
	pack->payload_size = (uint16_t)size;
	pack->flags = flags & SMF_FRAME_PACKET_MASK;
	smf_pack_crc_fill(pack);
	return true;
}
bool SmfPackage::Pack(Frame* frm) {
	returnIfErrC(false, frm->offset < Size());
	smf_packet_t pack;
	pack.seqNO = _index++ & 0xf;
	pack.payload_size = (uint16_t)(frm->size);	
	pack.flags = frm->flags & SMF_FRAME_PACKET_MASK;
	smf_pack_crc_fill(&pack);
	frm->offset -= Size();
	frm->size += Size();
	frm->flags |= SMF_FRAME_PACKED;
	memcpy((char*)frm->buff + frm->offset, &pack, sizeof(smf_packet_t));
	return true;
}
bool SmfPackage::Unpack(Frame* frm) {
	auto data = (char*)frm->buff + frm->offset;
	smf_packet_t pack;
	memcpy(&pack, data, Size());
	returnIfErrC(false, !smf_pack_crc_check(&pack));
	returnIfErrC(false, pack.payload_size+Size()!=frm->size);
	if (pack.seqNO != (_index++ & 0xf)) {
		dbgErrPXL("pack seqNO fail:%x!=%x", pack.seqNO, _index);
		_index = ((_index & ~0xf) | pack.seqNO) + 1;
	}
	frm->flags &= ~SMF_FRAME_PACKET_MASK;
	frm->flags |= pack.flags;
	frm->offset += Size();
	frm->size -= Size();
	return true;
}
int SmfPackage::Unpack(Frame* frm, void* data, int max) {
	smf_packet_t pack;
	auto hsize = Size();
	memcpy(&pack, data, hsize);
	returnIfErrC(false, !smf_pack_crc_check(&pack));
	returnIfErrC(false, pack.payload_size + hsize > max);
	if (pack.seqNO != (_index++ & 0xf)) {
		dbgErrPXL("pack seqNO fail:%x!=%x", pack.seqNO, _index);
		_index = ((_index & ~0xf) | pack.seqNO) + 1;
	}
	memset((void*)frm, 0, sizeof(Frame));
	frm->buff = (char*)data + hsize;
	frm->flags = pack.flags;
	frm->size = pack.payload_size;
	frm->max = pack.payload_size;
	return pack.payload_size+hsize;
}
