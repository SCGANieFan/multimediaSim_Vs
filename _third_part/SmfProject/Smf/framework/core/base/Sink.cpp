#include "Sink.h"
#include "smf_debug.h"
using namespace smf;
Sink::Sink() {
	_flags.Set(IS_Sink);
}
//Sink::~Sink() {
//}

bool Sink::Schedule() {
	CleanError();
	Frame* frm = 0;
	if (!_iport->Pull(frm)) {
		if (!processError(frm,0)) {
			return false;
		}
	}
	if (frm && (frm->size || frm->flags)) {
		auto rst = true;
		{
			MeasureCheck mc(this);
			rst = receiveFrame(frm);
		}
		if (!rst && !processError(frm,0)) {
			return false;
		}
		_iport->PostFrame(frm, this);
	}
	return true;
}

bool Sink::run(void* frm) {
	return Pull((Frame*&)frm);
}
