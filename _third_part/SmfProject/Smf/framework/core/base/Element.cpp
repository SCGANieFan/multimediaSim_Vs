#include "Element.h"
#include "Pipeline.h"
#include "SmfPorting.h"
#include "SmfFCC.h"
#include "SmfHash.h"
#include "smf_debug.h"
#include "thread.h"
#include "chip.h"
using namespace smf;
///
Element::Element()
	:_tempBuff{0,0}
{
	_flags.Set(IS_Element);
	_stackSize = 1024 * 8;
	_loopSleepMs = 1;
	memset(&_kips, 0, sizeof(_kips));
}
Element::~Element() {
	if (Status()!=EStatus::null) {
		dbgWarnPXL("[%s]no close!",Name());
		Status(EStatus::null);
	}
}
bool Element::start() {
	if (_oport && !_oport->_max) { _oport->_max = _oport->_min = (_oport->Linker())->_min; }
	Running(true);
	_flags.Set(IS_EOS, false);
	bool rst = true;
	if (_flags.CheckAny(IS_StartWithThread)) {
		//dbgTestPXL("%s",Name());
		_threadLoop = true;
		thread thr;
		rst = thr.Start(Name(), [](void* priv) {((Element*)priv)->Loop(); }, this, _stackSize, 0, _threadPriority);
		if (rst) {
			thr.detach();
		}
		else {
			dbgErrPXL("thread fail[%s]%u,%u",Name(), _stackSize, _threadPriority);
			_threadLoop = false;
		}
	}
	else {
		_threadLoop = false;
	}
	JobHold();
	return true;
}
bool Element::stop() {//dbgTestPXL(Name());
	Running(false);
	auto c = 0;
	while (_threadLoop) {
		sleep_for(5);
		if (++c > 400) {
			c = 0;
			dbgErrPXL("%s", Name());
		}
	}
	JobRelease();
	return true;
}
bool Element::set(uint32_t key, void* val) {
	switch (key) {
	case Hash("stackSize"): _stackSize = (uint32_t)val; return true;
	case Hash("priority"): _threadPriority = (uint32_t)val; return true;
	case Hash("thread"):_flags.Set(IS_StartWithThread, (bool)val); return true;
	case Hash("LoopSleepMs"):_loopSleepMs = (uint8_t)(uint32_t)val; return true;
	case Hash("tBuff"):
	case Hash("tempBuff"): if (val) { _tempBuff = *(smf_pair_t*)val; } return true;
	//case Hash("packed"): _packed = (bool)val; return true;
	//case Hash("iMedia"): if (_iport)_iport->SetAndClone((IMedia*)val); return true;
	//case Hash("oMedia"): if (_oport)_oport->SetAndClone((IMedia*)val); return true;
	case Hash("linkTo"): return LinkTo((Element*)val);
	case Hash("linkToX"): return LinkTo((const char*)val);
	case Hash("linkFrom"): return LinkFrom((Element*)val);
	case Hash("linkFromX"): return LinkFrom((const char*)val);
	case Hash("dislink"): DislinkInput(); DislinkOutput(); return true;
	case Hash("running"): Running((uint32_t)val); return true; //LinkFrom((Element*)val);
	case Hash("selected"): if(val)((Pipeline*)Parent())->_selected = this; return true; //LinkFrom((Element*)val);
	case Hash("src"):Flags().Set(EFlags::IS_Source, val); return true;
	case Hash("sink"):Flags().Set(EFlags::IS_Sink, val); return true;
	case Hash("processTimeout"):_processTimeout = (uint32_t)val; return true;
	}
	if (_oport && _oport->SetX(key, val))return true;
	if (_oport && _oport->_media && _oport->_media->SetX(key, val))return true;
	if (_iport && _iport->SetX(key, val))return true;
	if (_iport && _iport->_media && _iport->_media->SetX(key, val))return true;
	return Service::set(key, val);
}
bool Element::get(uint32_t key, void* val) const {
	if (_oport && _oport->GetX(key, val))return true;
	if (_oport && _oport->_media && _oport->_media->GetX(key, val))return true;
	if (_iport && _iport->GetX(key, val))return true;
	if (_iport && _iport->_media && _iport->_media->GetX(key, val))return true;
	switch (key) {
	case Hash("getKips"):Kips(*(smf_value_t*)val, false); return true;
	case Hash("getKipsRecursive"):Kips(*(smf_value_t*)val, true); return true;
	}
	return Service::get(key, val);
}
void Element::Loop() {
	//dbgTestPSL(Name());
	while (IsRunning() && StatusNext() >= EStatus::play) {
		if (IsPaused()) {//pause/resume
			sleep_for(10);
			continue;
		}
		if (!Schedule())
			break;
		//_loopSleepMs ?sleep_for(_loopSleepMs):yield();
		LoopSleep();
		if (!CpuCheck()) Root().Error().err |= SMF_ERROR_MASK_CPU;
	}
	//dbgTestPSL(Name());
	_threadLoop = false;
}
void Element::LoopSleep() {
	_loopSleepMs ? sleep_for(_loopSleepMs) : yield();
}
bool Element::processError(Frame* ifrm, Frame* ofrm) {
	switch (_error.errid) {
	case SMF_ERROR_OK:return true;
	case SMF_ERROR_IO_EOS:
		dbgWarnPXL("IO EOS:%s", Name());
		_flags.Set(IS_EOS, true);
		if (ofrm) {
			ofrm->flags |= SMF_FRAME_IS_EOS;
			if (!ifrm)ofrm->index++;
		}
		if (!_oport)
			return SendMessage("eos", smf_direction_upward);
		return true;
	case SMF_ERROR_IO_FAIL: 
		dbgWarnPXL("%s", Name()); 
		_flags.Set(IS_Error, true);
		if (ofrm) {
			ofrm->flags |= SMF_FRAME_IS_FAIL;
			if (!ifrm)ofrm->index++;
		}
		return false;
	case SMF_ERROR_INPUT_BUFFER_NOT_ENOUGH:
		return true;
	case SMF_ERROR_OUTPUT_BUFFER_NOT_ENOUGH:
		if (!ofrm->size && !ofrm->offset)return false;
		return true;
	case SMF_ERROR_SHARE_WRITE_FAIL: {
		return false;
	}
	default:
		dbgErrPXL("%s %d", Name(), _error.errid);
		dbgErrPQL(_error.err64);
		if (ifrm)ifrm->size = 0;
		if (ofrm)ofrm->size = 0;
		SendMessage("error", smf_direction_upward, (uint32_t)Name(), _error.errid, _error.err32[0], _error.err32[1]);
		return false;
	}
}
bool Element::receiveMessage(smf_message_t& msg) {
	if (Service::receiveMessage(msg))
		return true;
	if (_oport && msg.direction == smf_direction_forward)
		return _oport->Push(msg);
	else if (_iport && msg.direction == smf_direction_backward)
		return _iport->Push(msg);
	return false;
}
bool Element::processMessage(smf_message_t& msg){
	//dbgTestPXL("%s,%s",&msg.id, Name());
	switch(msg.id) {
	case fcc64("getKips"): {
		if (_measureProc.count) {
			_measureProc.kips(_kips);
		}
		msg.data[0] += _kips.avg;
		msg.data[1] += _kips.max;
		msg.data[2] += _kips.min;
	}
		return false;
	case fcc64("done"):
	case fcc64("eos"):
		dbgInfoPXL("%s,%s", (char*)&msg.id, Name());
		_flags.Set(IS_EOS, true);
		Running(false);
		if(_flags.Check(IS_Source) && !_flags.Check(IS_Sink)) {
			msg.direction = smf_direction_upward;
		}
		break;
	case fcc64("fail"):
		dbgInfoPXL("%s,%s", (char*)&msg.id, Name());
		_flags.Set(IS_Error, true);
		Running(false);
		break;
	}
	return Service::processMessage(msg);
}

char* Element::print(char* ptr, char* end) const {
	ptr = Service::print(ptr, end);
	if (_measureProc.count) {
		_measureProc.kips(_kips);
	}
	if (_kips.avg) {
		ptr += snprintf(ptr, end - ptr, "{%u(%u,%u)%u}"
			, _kips.avg
			, _kips.min
			, _kips.max
			, _kips.cnt
		);
	}
	if (_flags.Check(IS_StartWithThread)) {
		ptr += snprintf(ptr, end - ptr, "[%u%u]"
			, _threadRunning
			, _threadLoop
		);
	}
	if (_iport) {
		ptr = _iport->Print(ptr,end);
	}
	if (_oport) {
		ptr = _oport->Print(ptr, end);
	}
	return ptr;
}
#if 0
uint32_t Element::toUs(uint32_t size) {
	auto media = oMedia();
	if (!media) {
		media = iMedia();
	}
	if (media) {
		//auto bps = media->BytePerSecond();
		//if (bps.den) {
		//	return size * 1000000ull * bps.num / bps.den;
		//}
		return media->toUs(size);
	}
	return 0;
}
uint32_t Element::toMs(uint32_t size) {
	return toUs(size)/1000;
}
#endif
bool Element::Replace(Element* ele) {
	ele->ID(ID());
	if (_iport) {
		returnIfErrC(false, !_iport->Replace(ele->iPort()));
	}
	if (_oport) {
		returnIfErrC(false, !_oport->Replace(ele->oPort()));
	}
	//
	if (Parent()) {
		returnIfErrC(false, !Parent()->Replace(this, ele));
	}
	//
	return true;
}
#if 0
bool Element::PackageFrame(Frame& frm) {
	int packsize = sizeof(smf_packet_t);
	smf_packet_t pack;
	pack.seqNO = _packIndex & 0xf;
	pack.payload_size = (uint16_t)(frm.size);
	pack.flags = frm.flags & SMF_FRAME_PACKED_MASK;
	smf_pack_crc_fill(&pack);
	frm.offset -= packsize;
	frm.size += packsize;
	frm.flags |= SMF_FRAME_PACKED;
	memcpy((char*)frm.buff + frm.offset, &pack, packsize);
	//
	_packIndex++;
	return true;

}
bool Element::PrepareInputPacked(Frame* frm) {
	const int packsize = sizeof(smf_packet_t);
	smf_packet_t pkt;
	int seqNo = _ipackindex++;
	memcpy(&pkt, (char*)frm->buff + frm->offset, packsize);
	//returnIfErrEF(false, !smf_pack_crc_check(&pkt), SMF_ERROR_INVALID_PACKET, dbgErrDump((char*)frm->buff + frm->offset, 16));
	if (!smf_pack_crc_check(&pkt)) {
		dbgTestPXL("seqNo %d size %d frm index %d", seqNo, pkt.payload_size, frm->index);
		dbgErrDump(frm->buff, 32);
		returnIfErrEF(false, !smf_pack_crc_check(&pkt), SMF_ERROR_INVALID_PACKET, dbgErrDump((char*)frm->buff + frm->offset, 16));
	}
	returnIfErrCS(false, pkt.payload_size > frm->max, "[%d]%d>%d", seqNo, pkt.payload_size, frm->max);
	returnIfErrCS(false, pkt.payload_size + packsize > frm->size, "[%d]%d>%d", seqNo, pkt.payload_size, frm->size);
	if ((seqNo & 0xf) != pkt.seqNO) {
		dbgTestPXL("%08x<>%02x",seqNo,pkt.seqNO);
	}
	frm->offset += packsize;
	//frm->buff = (char*)frm->buff + frm->offset ;
	frm->size = pkt.payload_size;
	frm->flags = (frm->flags & ~SMF_FRAME_PACKED_MASK) | pkt.flags;
	return true;
}
#endif

Element::SharedInfo_t* Element::SharedInfo()const {
	if (_iport 
		&& _iport->Linker()
		&& _iport->Linker()->Parent()
		) {
		return _iport->Linker()->Parent()->SharedInfo();
	}
	return 0;
}
void Element::Kips(uint32_t avg, uint32_t min, uint32_t max) {
	_kips.avg = avg;
	_kips.min = min;
	_kips.max = max;
	_kips.cnt = 0;
}
void Element::Kips(smf_value_t& val, bool recursive) const {
	val.avg += _kips.avg;
	val.min += _kips.min;
	val.max += _kips.max;
	val.cnt ++;
	if (recursive && _flags.Check(IS_List)) {
		_children.Foreach([](Object* obj, void* priv) {
			if (obj->Flags().Check(IS_Element)) {
				((Element*)obj)->Kips(*(smf_value_t*)priv, true);
			}
			}, &val);
	}
}
void Element::beforeLink(Port* port, bool link) {
}
