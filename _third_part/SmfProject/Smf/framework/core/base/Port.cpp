#include "Port.h"
#include "Source.h"
#include "Sink.h"
#include "SmfHash.h"
#include "smf_debug.h"
#include "smf_error.h"
#include "string.hh"
#include "chip.h"
using namespace smf;
Port::~Port() {
	if (_deleteFrame && _frame)
		Free(_frame);
	if (_deleteHook && _hook)
		delete _hook;
	if (_deleteMedia && _media)
		delete _media;
	Port::DisLink();
}
Port::Port(Element* parent, bool isInput) {
	_parent = parent;
	_isInport = isInput;
	_max = 1024 * 4;
	if (_isInport)parent->_iport = this;
	else parent->_oport = this;
}
uint64_t Port::CodecType()const {
	return _isAudio ? fcc64("audio") : (_isVideo ? fcc64("video") : (_isOther ? fcc64("other") : 0));
}
void Port::dbgErrProcess(const char* file, unsigned line, unsigned error)const {
	ErrorProcess(_parent->_error, file, line, error);
}
bool Port::Link(Port* port) {
	returnIfErrC(false, !port);
	if (_linker) {
		if (_linker == port) {
			_parent->onEvent(Hash("linked"), port->_parent);
			return true;
		}
		else {
			return false;
		}
	}
	_linker = port;
	return port->Link(this);
}
bool Port::DisLink(Port* port) {
	returnIf(true, !port);
	if (_linker == port) {
		dislinkWait();
		_linker = 0;
		port->DisLink(this);
		_parent->onEvent(Hash("dislinked"), port->_parent);
	}
	return true;
}
void Port::DisLink() {
	if (_linker) {
		dislinkWait();
		auto port = _linker;
		_linker = 0;
		port->DisLink(this);
		_parent->onEvent(Hash("dislinked"), port->_parent);
	}
}
bool Port::Replace(Port*dst) {
	returnIf(false, !dst);
	returnIfErrC(false, !_isMultiLink && dst->_isMultiLink);
	if(_isInport && _media && !dst->_media) {
		dst->_media = _media;
		dst->_deleteMedia = _deleteMedia;
		if(_deleteMedia) {
			_deleteMedia = false;
		}
	}
	dst->_isAudio = _isAudio;
	dst->_isVideo = _isVideo;
	dst->_isOther = _isOther;
	if (_linker) {
		auto linker = _linker;
		_linker = 0;
		linker->DisLink(this);
		linker->Link(dst);
	}
	return true;
}
bool Port::Push(smf_message_t&msg) {
	return _linker ? _linker->Parent()->receiveMessage(msg) : false;
}
void Port::Set(IMedia* media, bool deleteMedia) {
	if (_deleteMedia && _media)delete _media;
	_media = media; 
	_deleteMedia = deleteMedia;
	_mediaUpdate = true;
}
bool Port::Set(const char* codec) {
	if (_media && _media->Codec() == fcc64(codec)) {
		return true;
	}
	auto media = IMedia::Create(codec);
	returnIfErrC(false, !media);
	Set(media, true);
	return true;
}
bool Port::SetMediaScript(const char* script) {
	returnIfWarnC(false, !script);
	auto media = IMedia::CreateScript(script);
	returnIfWarnC(false, !media);
	Set(media, true);
	return true;
}
bool Port::SetAndClone(IMedia*media) {
	returnIfWarnC(false, !media);
	Set(media->Clone(), true);
	return true;
}
void Port::SetReallocFrame(uint32_t max) {
	_max = max;
	_reallocFrame = true;
}
Frame* Port::InitFrame(Frame*frm) {
	if (!CpuCheck()) Root().Error().err |= SMF_ERROR_MASK_CPU;
	//
	if (_reallocFrame) {
		_reallocFrame = false;
		if (frm == _frame) {
			frm = 0;
		}
		if (_frame) {
			if (_deleteFrame) {
				delete _frame;
			}
			_frame = 0;
		}
	}
	//
	if (frm && frm->buff) {
		if (!frm->size) {
			frm->offset = 0;
		}
		if(!frm->Media()){
			frm->Port(this);
			frm->Media(Media());
		}
		if (!frm->sinfo) {
			frm->sinfo = _parent->SharedInfo();
		}
		if (_mediaUpdate) {
			frm->Media(Media());
			_mediaUpdate = false;
		}
		return frm;
	}
	//
	if(!_frame){
		if (_noAllocFrame)return frm;
		returnIfErrC(0, !_noAllocFrameBuff && !_max);
		auto max = _noAllocFrameBuff ? 0 : _max;
		auto fsize = ((sizeof(Frame) + 7) & ~7);
		auto size = fsize + max;
		auto buff = Alloc(size);
		returnIfErrC(0, !buff);
		memset(buff, 0, size);
		_deleteFrame = true;
		_frame = (Frame*)buff;
		_frame->buff = _noAllocFrameBuff ? (char*)0 : ((char*)buff + fsize);
		_frame->max = max;
		_frame->Port(this);
		_frame->Media(Media());
		_frame->sinfo = _parent->SharedInfo();
	}
	//
	if (frm == _frame) {

	}
	else if (frm) {
		*frm = *_frame;
	}
	else {
		frm = _frame;
	}
	//
	if (!frm->size) {
		frm->offset = 0;
	}
	if (_mediaUpdate) {
		frm->Media(Media());
		_mediaUpdate = false;
	}
	return frm;
}

bool Port::GetX(uint32_t key, void* val) const {
	if(_isInport) {
		switch(key) {
		case Hash("iMedia"): *(IMedia**)val = Media(); return true;
		case Hash("iMax"): val = (void*)(uintptr_t)_max; return true;
		case Hash("iMin"): val = (void*)(uintptr_t)_min; return true;
		case Hash("ihook"): val = (void*)_hook; return true;
		case Hash("iMeduaV"): {
			auto media = Media();
			if (media) {
				memcpy(val, (void*)media, media->_size);
			}
			return true;
		}
		}
	}
	else {
		switch(key) {
		case Hash("MediaInfo"):
		case Hash("media"):
		case Hash("omedia"): *(IMedia**)val = Media(); return true;
		case Hash("oMax"):val = (void*)(uintptr_t)_max; return true;
		case Hash("oMin"):val = (void*)(uintptr_t)_min; return true;
		case Hash("ohook"):val = (void*)_hook; return true;
		case Hash("oMeduaV"): {
			auto media = Media();
			if (media) {
				memcpy(val, (void*)media, media->_size);
			}
			return true;
		}
		}
	}
	return false;
}
bool Port::SetX(uint32_t key, void* val) {
	switch (key) {
	case Hash("codec"): Set((char*)val); return true;
	}
	if(_isInport) {
		switch (key) {
		case Hash("imediaClone"): SetAndClone((IMedia*)val); return true;
		case Hash("imedia"): Set((IMedia*)val); return true;
		case Hash("iMax"): _max = (int)val; return true;
		case Hash("iMin"): _min = (int)val; return true;
		case Hash("ihook"):Set((IHook*)val, false); return true;
		case Hash("ihookD"):Set((IHook*)val, true); return true;
		case Hash("Audio"):
		case Hash("iAudio"):_isAudio = !!val; return true;
		case Hash("Video"):
		case Hash("iVideo"):_isVideo = !!val; return true;
		case Hash("Other"):
		case Hash("iOther"):_isOther = !!val; return true;
		case Hash("iLog"):_enLog = !!val; return true;
		case Hash("iLogTimestamp"):_enLogTimeStamp = !!val; return true;
		case Hash("iLogMedia"):_enLogMedia = !!val; return true;
		case Hash("disableSendEOSMsg"):_disableSendEOSMsg = (bool)val; return true;
		}
	}
	else {
		switch (key) {
		case Hash("omediaFrom"): if (_media)_media->FromMedia((smf_media_def_t*)val, false); return _media;
		case Hash("omediaClone"): return SetAndClone((IMedia*)val);
		case Hash("omediaScript"): return SetMediaScript((const char*)val);
		case Hash("omedia"):Set((IMedia*)val); return true;
		case Hash("oMax"): _max = (int)val; return true;
		case Hash("oMin"): _min = (int)val; return true;
		case Hash("ohook"):Set((IHook*)val, false); return true;
		case Hash("ohookD"):Set((IHook*)val, true); return true;
		case Hash("Audio"):
		case Hash("oAudio"):_isAudio = !!val; return true;
		case Hash("Video"):
		case Hash("oVideo"):_isVideo = !!val; return true;
		case Hash("Other"):
		case Hash("oOther"):_isOther = !!val; return true;
		case Hash("oLog"):_enLog = !!val; return true;
		case Hash("oLogTimestamp"):_enLogTimeStamp = !!val; return true;
		case Hash("oLogMedia"):_enLogMedia = !!val; return true;
		case Hash("noAllocFrame"):_noAllocFrame = (bool)val; return true;
		case Hash("noAllocFrameBuff"):_noAllocFrameBuff = (bool)val; return true;
		case Hash("noPushWait"):_isPushNoWait = (bool)val; return true;
		case Hash("pushSleep"):_isPushSleep = (bool)val; return true;
		case Hash("enc"):
		case Hash("encode"): 
			if (((smf_media_audio_encode_t*)val)->codec == fcc64("pcm"))return true;
			returnIfErrC(false, !_media || !_media->IsAudio());
			return _media->SetX("fromEncode", val);
		}
	}
	return false;
}

IMedia* Port::Media() const {
	if (_media) {
		return _media;
	}
	if (_isInport) {
		return Linker() ? Linker()->Media() : 0;
	}
	else {
		return Parent() ? Parent()->iMedia() : 0;
	}
}

bool Port::Set(IHook* hook, bool deleteHook) { 
	if (_deleteHook && _hook) {
		delete _hook;
	}
	_hook = hook; 
	_deleteHook = deleteHook; 
	return true;
}

bool Port::Push(Frame* frm,Port* port) {
	_countFrame++;
	returnIf(true, !_parent->IsRunning());
	returnIf(true, !frm);
	returnIf(true, !frm->size && !frm->flags);
	Print(frm);
	if (_hook) {
		_hook->Hook(frm);
		if (!port)return true;
	}
	returnIfErrC(false, !port);
	RecordProcess rec(port);
	auto ele = port->Parent();
	if (ele->FlagsExt().Check(EF_Disable)) {
		frm->size = 0;
		return true;
	}
	returnIf(true, !ele->IsRunning());
	if(!frm->Port()) frm->Port(this);
	if (port->_hook) {
		port->_hook->Hook(frm);
	}
	//
	do {
		port->Print(frm);
		ele->CleanError();
		if (Root().Error().err & SMF_ERROR_MASK_CHIP_CPU) {//[chip fail]skip receiveFrame
			if (frm) frm->size = 0;
		}
		else{
			auto rst = false;
			if (ele->Flags().Check(IS_Source)){
				rst = ele->receiveFrame(frm);
			}
			else {
				MeasureCheck mc(ele);
				rst = ele->receiveFrame(frm);
			}
			if (!rst && !ele->processError(frm, 0)) {
				//dbgErrPL();
				return false;
			}
		}
		if (!(frm->flags & SMF_FRAME_IS_RESERVE) && (!frm->size || (frm->size < port->_min))) {
			break;
		}
		else {
			//dbgTestPXL("[%u]%u,%08x,%s/%s", frm->index, frm->size, frm->flags, _parent->Parent()->Name(), _parent->Name());
		}
		if (_isPushSleep) {
			sleep_for(1);
		}
		else {
			yield();
		}
		_countLoop++;
	} while (_parent->IsRunning() && ele->IsRunning() && !_isPushNoWait);
	//
	return PostFrame(frm, ele);
}
bool Port::Pull(Frame*& frm, Port* port) {
	_countFrame++;
	returnIf(true, !_parent->IsRunning());

	returnIf(true, !port);
	RecordProcess rec(port);
	auto ele = port->Parent();
	if (_parent->FlagsExt().Check(EF_Disable) || ele->FlagsExt().Check(EF_Disable)) {
		return true;
	}
	returnIf(true, !ele->IsRunning());
	//
	if(_max && _media){
		frm = InitFrame(frm);
		if(_parent && !_parent->processError(frm, 0)) return false;
	}else{
		frm = port->InitFrame(frm);
		if(ele && !ele->processError(0, frm)) return false;
	}
	if (frm) {
		frm->port = this;
		if (!frm->size) {
			frm->offset = 0;
		}
		else if (frm->size >= _min) {
			return true;
		}
	}
	//
	ele->CleanError();
	if (Root().Error().err & SMF_ERROR_MASK_CHIP_CPU) {//[chip fail]skip generateFrame
		if (frm) frm->size = frm->max;
	}
	else {
		auto rst = true;
		if (ele->Flags().Check(IS_Sink)) {
			rst = ele->generateFrame(frm);
		}
		else {
			MeasureCheck mc(ele);
			rst = ele->generateFrame(frm);
		}
		if (!rst && !ele->processError(0, frm)) {
			//dbgErrPL();
			return false;
		}
	}
	//
	if (frm) {
		port->Print(frm);
		Print(frm);
		if (frm->size) {
			if(_hook) {
				_hook->Hook(frm);
				if (!port)return true;
			}
			if (port->_hook) {
				port->_hook->Hook(frm);
			}
		}
	}
	//dbgTestPXL("%s[%d]%u,%u/%u,%08x,%p,%p", _parent->Name(), frm->index, frm->offset, frm->size, frm->max, frm->flags, frm->media, frm->Media());
	return true;
}
bool Port::PostFrame(Frame* frm, Element* ele) {
	auto name = ele ? ele->Name() : "";
	if (frm->flags & SMF_FRAME_IS_EOS) {
		if (frm->size <= 0) {
			_parent->Running(false);
			if (ele && ele->_iport && !ele->_oport) {
				//dbgTestPXL("[%d][eos][msg]%s", frm->index, ele->Name());
				if (!ele->_iport->_disableSendEOSMsg) {
					dbgTestPXL("[%u]eos", frm->index);
					ele->SendMessage("eos", smf_direction_backward);
				}
			}
		}
		else if (_linker && (frm->size < _linker->_min)) {
			dbgWarnPXL("%s,%u,%d+%d,%u,min:%u", name, frm->index, frm->offset, frm->size, frm->max, _linker->_min);
			_parent->Running(false);
		}
	}
	if (frm->size <= 0) {
		if (frm->size < 0) {
			dbgErrPXL("%s,%u,%d+%d,%u", name, frm->index, frm->offset, frm->size, frm->max);
		}
		if (frm->flags & SMF_FRAME_IS_RESERVE) {
			frm->flags &= ~SMF_FRAME_IS_RESERVE;
		}
		else {
			frm->flags = 0;
		}
		frm->size = 0;
		frm->offset = 0;
	}
	if (frm->offset + frm->size > frm->max) {
		dbgErrPXL("%s,%u,%d+%d>%u", name, frm->index, frm->offset, frm->size, frm->max);
		frm->offset = 0;
		frm->size = 0;
	}
	if (!ChipCheck()) Root().Error().err |= SMF_ERROR_MASK_CHIP;
	_countPost++;
	return true;
}
void Port::dislinkWait() {
	if (_dislinkWait) {
		_dislinkWaiting = true;
		auto c = 0;
		while (_dislinkWait && _processing) {
			sleep_for(5);
			if (++c > 400) {
				c = 0;
				dbgErrPXL("%s", _parent->Name());
			}
		}
		_dislinkWaiting = false;
	}
}
//ports
Ports::~Ports() {
	Ports::DisLink();
}
Ports::Ports(Element* parent, bool isInput):Port(parent, isInput) {
	_isMultiLink = true;
	memset(_linkers, 0, sizeof(_linkers));
}
int Ports::CountGet()const {
	auto cnt = 0;
	for (int i = 0; i < Count(); i++) {
		if (_linkers[i] ) {
			cnt++;
		}
	}
	return cnt;
}
int Ports::Index(Port* port)const {
	returnIfErrC(-2, !port);
	if (_linkers[port->_index] == port) {
		return port->_index;
	}
	for (int i = 0; i < Count(); i++) {
		if (_linkers[i] == port) {
			port->_index = i;
			return i;
		}
	}
	return -1;
}
bool Ports::Link(Port* port) {
	returnIfErrC(false, !port);
	if (_linker == port) {
		return true;
	}
	if (_linkers[port->_index] == port) {
		return true;
	}
	for (int i = 0; i < Count(); i++) {
		if (_linkers[i] == port) {
			_linker = port;
			_linker->_index = i;
			return true;
		}
	}
	//
	for (int i = 0; i < Max(); i++) {
		if (!_linkers[i]) {
			if (_count <= i) {
				_count = i + 1;
			}
			_linkers[i] = port;
			_linker = port;
			_linker->_index = i;
			return port->Link(this);
		}
	}
	return false;
}
bool Ports::DisLink(Port* port) {
	returnIf(true, !port);
	returnIf(true, !_linker);
	dislinkWait();
	if (_linker == port) {//clear linker
		_linker = 0;
	}
	
	if (_linkers[port->_index] == port) {
		_linkers[port->_index] = 0;
	}
	else {
		bool find = false;
		for (int i = 0; i < Count(); i++) {//clear linkers
			if (_linkers[i] == port) {
				_linkers[i] = 0;
				find = true;
				break;
			}
		}
		if (!find) {
			return true;
		}
	}

	if (!_linker) {//reset linker
		for (int i = 0; i < Count(); i++) {
			if (_linkers[i]) {
				_linker = _linkers[i];
				_linker->_index = i;
				break;
			}
		}
	}

	port->DisLink(this);
	_parent->onEvent(Hash("dislinked"), port->_parent);

	return true;
}
void Ports::DisLink() {
	if (_linker) {
		dislinkWait();
		_linker = 0;
		for (int i = 0; i < Count(); i++) {
			if (_linkers[i]) {
				auto port = _linkers[i];
				_linkers[i] = 0;
				port->DisLink(this);
			}
		}
	}
}
bool Ports::Replace(Port* dst) {
	returnIf(false, !dst);
	for (int i = 0; i < Count(); i++) {
		auto link = _linkers[i];
		if (link) {
			_linkers[i] = 0;
			link->DisLink(this);
			link->Link(dst);
			if(!dst->_isMultiLink) {
				_linker = 0;
				break;
			}
		}
	}
	return Port::Replace(dst);
}

bool Ports::Push(smf_message_t& msg) {
	for (int i = 0; i < Count(); i++) {
		if (_linkers[i] && _linkers[i]->_parent->receiveMessage(msg)) {
			if (!msg.is_notify) {
				return true;
			}
		}
	}
	return false;
}

bool Ports::PushAll(Frame* frm) {
	for (int i = 0; i < Count(); i++) {
		auto port = _linkers[i];
		if (port) {
			Frame xfrm = *frm;
			Push(&xfrm, port);
			yield();
		}
	}
	frm->size = 0;
	return true;
}

char* Port::Print(char* ptr, char* end, Frame* frm) const {
	if (_enLog || _enLogTimeStamp || (_enLogMedia && frm && frm->media)) {
		ptr = snprintf(ptr,end,"%c:%s/%s"
			, _isInport ? 'i' : 'o'
			, _parent->Parent()?_parent->Parent()->Name():""
			, _parent->Name()
		);
		if(frm)
			ptr = frm->Print(ptr,end,_enLogTimeStamp,_enLogMedia);
	}
	return ptr;
}

void Port::Print(Frame* frm) const {
	if (_enLog || _enLogTimeStamp || (_enLogMedia && frm && frm->media)) {
		string buff{ 1024 };
		auto buf = buff.c_str();
		auto end = buf + 1024;
		auto ptr = Print(buf, end, frm);
		dbgTestOutputL(buf, ptr - buf);
		//dbgOutput(dbg_chn_test, buf, ptr - buf);
	}
}

char* Port::Print(char* ptr, char* end) const {
	if (_countFrame || _countPost) {
		ptr = snprintf(ptr, end, ",%c(%u,%u)"
			, _isInport ? 'i' : 'o'
			, _countFrame, _countPost
		);
		_countFrame = 0;
		_countPost = 0;
	}
	if (_countLoop) {
		ptr = snprintf(ptr, end, "L%u"
			, _countLoop
		);
		_countLoop = 0;
	}
	if (_dislinkWaiting && ptr!=end) {
		*ptr++ = 'W';
	}
	if (_processing && ptr != end) {
		*ptr++ = 'P';
	}
	return ptr;
}
