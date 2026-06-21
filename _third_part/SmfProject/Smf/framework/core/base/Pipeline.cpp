#include "Pipeline.h"
#include "SmfFCC.h"
#include "SmfHash.h"
#include "smf_debug.h"
using namespace smf;
///
EXTERNC void smf_pipeline_register() {
	Pipeline::Register<Pipeline>("pl");
}
///
Pipeline::Pipeline(){
	_flags.Set(EFlags::IS_Pipeline);
}
Pipeline::~Pipeline() {
	Status(EStatus::null);
	_iport = 0;
	_oport = 0;
}
bool Pipeline::processFrame(Frame* ifrm, Frame*& ofrm) {
	return false;
}
bool Pipeline::generateFrame(Frame*& ofrm){ return false; }
bool Pipeline::receiveFrame(Frame* ifrm){ return false; }
bool Pipeline::statusChangeStep(EStatus sts) {
	//dbgTestPXL("[%s]%d->%d", Name(), _sts, sts);
	//
	if (_sts > sts) {
		Element::statusChangeStep(sts);
	}
	//
	returnIfErrC(false, !statusChangeSteps(sts, (Object**)_list, Count()));
	//
	if (_sts < sts) {
		returnIfErrC(false, !Element::statusChangeStep(sts));
	}
	return true;
}
bool Pipeline::processMessage(smf_message_t& msg) {
	//dbgTestPXL("%s,%s",&msg.id, Name());
	switch (msg.id) {
	case fcc64("eos"): 
		if (_pmsg)*_pmsg = msg.id;
		if (_flags.Check(IS_EOS | IS_Error)) {
			return false;
		}
		_flags.Set(IS_EOS, true);
		if (_flagsExt.Check(PLExtFlags::EF_AutoDestroyAfterEOS) && !_flags.Check(IS_Destroy)) {
			_flags.Set(IS_Destroy, true);
			InvokeDelete(this, 10);
		}
		break;
	case fcc64("done"):
		if (_pmsg)*_pmsg = msg.id;
		if (_flags.Check(IS_EOS | IS_Error)) {
			return false;
		}
		_flags.Set(IS_EOS, true);
		if (_flagsExt.Check(PLExtFlags::EF_AutoDestroyAfterDONE) && !_flags.Check(IS_Destroy)) {
			_flags.Set(IS_Destroy, true);
			InvokeDelete(this, 10);
		}
		break;
	case fcc64("error"):
		if (_pmsg)*_pmsg = msg.id;
		if (_flags.Check(IS_EOS | IS_Error)) {
			return true;
		}
		_flags.Set(IS_Error, true);
		if (_flagsExt.Check(PLExtFlags::EF_AutoDestroyAfterERROR) && !_flags.Check(IS_Destroy)) {
			_flags.Set(IS_Destroy, true);
			InvokeDelete(this, 10);
		}
		break;
	}
	return Base::processMessage(msg);
}
bool Pipeline::set(uint32_t key, void* val) {
	switch (key) {
	case Hash("link"):return linkx((uint8_t)((uint32_t)val>>8), (uint8_t)((uint32_t)val));
	case Hash("autoDestroyAfterEOS"):_flagsExt.Set(PLExtFlags::EF_AutoDestroyAfterEOS, !!val); return true;
	case Hash("autoDestroyAfterDONE"):_flagsExt.Set(PLExtFlags::EF_AutoDestroyAfterDONE, !!val); return true;
	case Hash("autoDestroyAfterERROR"):_flagsExt.Set(PLExtFlags::EF_AutoDestroyAfterERROR, !!val); return true;
	case Hash("pmsg"):_pmsg = (uint32_t*)val; return true;
	case Hash("seekToMs"): 
	case Hash("seekForMs"): 
	case Hash("volumeReduce"): 
	case Hash("muteForce"): 
	case Hash("volume"): 
	case Hash("mute"): return _selected ? ((Object*)_selected)->Set(key, val) : false;
	}
	return Element::set(key,val);
}
bool Pipeline::get(uint32_t key, void* val) const {
	switch (key) {
	case Hash("meta"):
	case Hash("metaV"):
	case Hash("imedia"):
	case Hash("imediaV"):
	case Hash("omedia"):
	case Hash("omediaV"):
	case Hash("progress"):
	case Hash("progressV"): return _selected ? ((Object*)_selected)->Get(key, val) : false;
	}
	return Element::get(key, val);
}

void Pipeline::initSource() const {
	if (_oport)return;
	for (int i = 0; i < Count(); i++) {
		auto ele = _list[i];
		if (ele && ele->_oport && !ele->_oport->Linker()) {
			((Pipeline*)this)->Flags().Set(EFlags::IS_Source);
			((Pipeline*)this)->_oport = ele->_oport;
			return;
		}
	}
	//dbgTestPSL(Name());
	return;
}
void Pipeline::initSink() const{
	if (_iport)return;
	for (int i = 0; i < Count(); i++) {
		auto ele = _list[i];
		if (ele && ele->_iport && !ele->_iport->Linker()) {
			((Pipeline*)this)->Flags().Set(EFlags::IS_Sink);
			((Pipeline*)this)->_iport = ele->_iport;
			return;
		}
	}
	//dbgTestPSL(Name());
	return;
}
bool Pipeline::linkx(uint8_t sink_idx, uint8_t src_idx) {
	//dbgTestPXL("%d->%d", sink_idx, src_idx);
	returnIfErrC(false, src_idx >= Count());
	returnIfErrC(false, sink_idx >= Count());
	auto src = _list[src_idx];
	auto sink = _list[sink_idx];
	returnIfErrC(false, !src);
	returnIfErrC(false, !sink);
	return sink->LinkTo(src);
}
void Pipeline::beforeLink(Port* port, bool link) {
	Element::beforeLink(port, link);
	if (link) {
		initSink();
		initSource();
	}
}

bool Pipeline::Schedule() {
	returnIfWarnC(false, !IsRunning());
	for (int i = 0; i < Count(); i++) {
		auto ele = _list[i];
		if (ele && ele->Flags().Check(IS_Scheduled)) {
			if(ele->Status()==EStatus::play)
				ele->Schedule();
		}
	}
	return true;
}
bool Pipeline::run(void* para) {
	return _selected ? _selected->Run(para) : false;
}
void Pipeline::Running(bool val) {
	if (!val) {
		for (int i = 0; i < Count(); i++) {
			auto ele = _list[i];
			if (ele) {
				ele->Running(val);
			}
		}
	}
	Element::Running(val);
}
 