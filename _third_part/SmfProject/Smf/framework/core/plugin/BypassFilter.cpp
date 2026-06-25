#include "BypassFilter.h"
#include "smf_debug.h"
#include "SmfHash.h"
#include "MediaAudio.h"
using namespace smf;
EXTERNC void smf_bypass_filter_register() {
	Object::Register<BypassFilter>("filter-bypass");
}
EXTERNC void smf_dumy_filter_register() {
	Object::Register<BypassFilter>("filter-dumy");
}

BypassFilter::BypassFilter() {
	_kips.avg = 1000;
	_kips.max = 1000;
	_iport->_max = 4096;
	_iport->_min = 0;
	_oport->_max = 4096;
	_oport->_min = 0;

}
BypassFilter::~BypassFilter() {
	Status(EStatus::null);
}

bool BypassFilter::Schedule() {
	if(_pmtx) {
		unique_lock<mutex> lck(*_pmtx);
		return  Filter::Schedule();
	}
	return Filter::Schedule();
}

bool BypassFilter::processFrame(Frame* ifrm, Frame*& ofrm) {
	// if (!ifrm->size) {
	// 	ofrm->size = 0;
	// 	return true;
	// }
	if (!ofrm) {
		ofrm = ifrm;
	}
	else if (!ofrm->buff) {
		*ofrm = *ifrm;
	}
	else {
		auto left = ofrm->max - ofrm->size;
		memmove(ofrm->buff, (char*)ofrm->buff + ofrm->offset, ofrm->size);
		ofrm->offset = 0;
		auto size = ifrm->size < left ? ifrm->size : left;
		memcpy((char*)ofrm->buff + ofrm->offset + ofrm->size, (char*)ifrm->buff + ifrm->offset, size);
		ofrm->size += size;
		ofrm->media = ifrm->media;
		ofrm->timestamp = ifrm->timestamp;
		ifrm->offset += size;
		ifrm->size -= size;
		if(ofrm->media && ifrm->size && iMedia()->Is("pcm")) {
			auto media = (MediaPcm*)ofrm->media;
			if(!_isizePerSecond)_isizePerSecond = media->_channels * media->_sampleBits * media->_rate >> 3;
			ofrm->timestamp += _isizePerSecond?size*1000/_isizePerSecond:0;
		}
	}
	//if (_flags.Check(IS_Log))
	//	dbgTestPXL("%s[%d]%d,0x%08x[%d]%d,0x%08x", Name(), ifrm->index, ifrm->size, ifrm->flags, ofrm->index, ofrm->size, ofrm->flags);

	return true;
}

bool BypassFilter::set(uint32_t key, void* val) {
	switch (key) {
	case Hash("mtx"):
	case Hash("mutex"):_pmtx = (mutex*)val;	return true;
	}
	return Filter::set(key, val);
}

EXTERNC void smf_selector_register() {
	Object::Register<Selector>("filter-selector");
}
bool Selector::open(void*para) {
	returnIfErrC(false, !_iports.Select(_iportIdx));
	returnIfErrC(false, !_oports.Select(_oportIdx));
	return BypassFilter::open(para);
}

bool Selector::set(uint32_t key, void* val) {
	switch (key) {
	case Hash("iport"): _iports.Select(_iportIdx=(int)(val)); return true;
	case Hash("oport"): _oports.Select(_oportIdx=(int)(val)); return true;
	}
	return BypassFilter::set(key, val);
}

EXTERNC void smf_collector_register() {
	Object::Register<Collector>("filter-collector");
}
bool Collector::receiveFrame(Frame* ifrm) {
	return _oport->Push(ifrm);
}
bool Collector::generateFrame(Frame*& ofrm) {
	dbgErrPL();
	return false;
}

EXTERNC void smf_divider_register() {
	Object::Register<Divider>("filter-divider");
}
bool Divider::receiveFrame(Frame* ifrm) {
	return _oports.PushAll(ifrm);
}
bool Divider::generateFrame(Frame*& ofrm) {
	dbgErrPL();
	return false;
}