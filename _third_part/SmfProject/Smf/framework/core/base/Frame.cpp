#include "Frame.h"
#include "Port.h"
#include "SmfPorting.h"
#include "smf_debug.h"
using namespace smf;
Frame::Frame() {
	buff = 0;
	max = 0;
	offset = 0;
	size = 0;
	flags = 0;
	media = 0;
	index = 0;
	timestamp = 0;
	frame = 0;
	port = 0;
	priv = 0;
	pool = 0;
	sinfo = 0;
}
Frame::~Frame() {
	//if (free)free(this);
	Free();
}
void Frame::TimeStampBase(uint64_t tp) {
	if (sinfo) {
		sinfo->forward.timestamp = tp;
	}
}
void Frame::TimeStampReset() {
	if (sinfo) {
		sinfo->forward.timestamp = get_local_ms();
	}
	timestamp = 0;
}
void Frame::TimeStampUpdate() {
	if (sinfo && !sinfo->forward.timestamp)
		TimeStampReset();
	timestamp = (uint32_t)(get_local_ms() - TimeStampBase());
}
void Frame::CleanOffset() {
	if (offset) {
		memmove(buff, Data(), size);
		offset = 0;
	}
}
void Frame::SetIndex(uint32_t idx) {
	index = idx;
	if (media)
		timestamp = Media()->FrameDurationUs() * idx / 1000;
}
uint32_t Frame::NextIndex() {
	SetIndex(index + 1);
	return index;
}
char* Frame::Print(char* ptr, char* end, bool timestamp_en, bool media_en) {
	ptr = snprintf(ptr,end,"[%d,%u]%s,%p,%d,%d,%d,%08x"
		, index, (flags& SMF_FRAME_IS_KEY?1:0), CodecStringX(), buff, offset, size, max, flags
	);

	if (timestamp_en) {
		uint32_t tpbase = TimeStampBase();
		uint32_t tpoffset = TimeStampOffset();
		uint32_t tp = TimeStamp();
		ptr = snprintf(ptr, end, ",%u=%u+%u", tp, tpbase, tpoffset);
	}
	if (media_en && media) {
		ptr = Media()->Print(ptr, end);
	}
	return ptr;
}
//smf_frame_t* Frame::To() {
//	return this;
//}
//IMedia* Frame::Media() const { 
//	if (media)
//		return (IMedia*)media;
//	else if (port)
//		return ((smf::Port*)port)->Media();
//	else 
//		return nullptr;
//}
#if 0
void smfTestFrame() {
	Frame frm;
	auto frm0 = &frm;
	auto frm1 = (smf_frame_t*)&frm;
	auto frm2 = (Frame*)frm1;
	auto frm3 = static_cast<smf_frame_t*>(&frm);
	auto frm4 = static_cast<Frame*>(frm3);
	dbgTestPPL(frm0);
	dbgTestPPL(frm1);
	dbgTestPPL(frm2);
	dbgTestPPL(frm3);
	dbgTestPPL(frm4);
	dbgTestPDL(sizeof(Frame));
	dbgTestPDL(sizeof(smf_frame_t));
}
#endif