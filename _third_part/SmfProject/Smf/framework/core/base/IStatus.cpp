#include "IStatus.h"
#include "smf_debug.h"
using namespace smf;
#if 0
using namespace smf::x;
bool IStatus::statusChange(EStatus target) {
	auto sts = _sts;
	_sts0 = sts;
	if (sts < target) {
		while (sts < target) {
			sts = (EStatus)((int)sts + 1);
			_sts0 = sts;
			//returnIfErrC(false, !statusChangeBefore(sts, sts1));
			returnIfErrC(false, !statusChangeStep(sts));
			//returnIfErrC(false, !statusChangeAfter(sts, sts1));
			_sts = sts;
		}
	}
	else if (sts > target) {
		while (sts > target) {
			sts = (EStatus)((int)sts - 1);
			_sts0 = sts;
			//returnIfErrC(false, !statusChangeBefore(sts, sts1));
			returnIfErrC(false, !statusChangeStep(sts));
			//returnIfErrC(false, !statusChangeAfter(sts, sts1));
			_sts = sts;
		}
	}
	else {

	}
	return true;
}
bool IStatus::statusChangeStep(EStatus sts) {
	if (_sts < sts) {
		switch (_sts) {
		case EStatus::null:
			returnIfErrC(false, !open(_openParam));
			break;
		case EStatus::ready:
			returnIfErrC(false, !start());
			break;
		case EStatus::play:
			returnIfErrC(false, !pause());
			break;
		case EStatus::pause:
			dbgErrPXL("sts:%d,%d", (int)_sts, (int)sts);
			return false;
		case EStatus::max:
			dbgErrPXL("sts:%d,%d", (int)_sts, (int)sts);
			return false;
		}
	}
	else if (_sts > sts) {		
		switch (_sts) {
		case EStatus::null:
			dbgErrPXL("sts:%d,%d", (int)_sts, (int)sts);
			return false;
		case EStatus::ready:
			returnIfErrC(false, !close());
			break;
		case EStatus::play:
			returnIfErrC(false, !stop());
			break;
		case EStatus::pause:
			returnIfErrC(false, !resume());
			break;
		case EStatus::max:
			dbgErrPXL("sts:%d,%d", (int)_sts, (int)sts);
			return false;
		}
	}
	else {

	}
	return true;
}
bool IStatus::Status(EStatus sts, EStatusChange chg) {
	switch (chg) {
	case EStatusChange::null:break;
	case EStatusChange::up:	if ((int)sts <= (int)_sts)return true; break;
	case EStatusChange::down: if ((int)sts >= (int)_sts)return true; break;
	}
	return statusChange(sts);
}
EStatus IStatus::Status()const {return _sts;}
EStatus IStatus::StatusNext()const {return _sts0;}
bool IStatus::open(void*para) {return true;}
bool IStatus::close() { return true; }
bool IStatus::start() { return true; }
bool IStatus::stop() { return true; }
bool IStatus::pause() { return true; }
bool IStatus::resume() { return true; }
//bool IStatus::statusChangeBefore(EStatus) {return true;}
//bool IStatus::statusChangeAfter(EStatus) {return true;}
//bool IStatus::statusChangeError(EStatus) {return false;}
bool IStatus::IsNone()const {return _sts == EStatus::null;}
bool IStatus::IsOpen()const {return _sts == EStatus::ready;}
bool IStatus::IsPlaying()const {return _sts == EStatus::play;}
bool IStatus::IsPaused()const {return _sts == EStatus::pause;}

bool IStatus::Open(void*para) {
	if (para)_openParam = para;
	return Status(EStatus::ready,EStatusChange::up);
}
bool IStatus::Close() {
	return Status(EStatus::null, EStatusChange::down);
}
#if 0
bool IStatus::Start() {
	if (_sts == EStatus::play)return true;
	returnIfErrC(false, _sts != EStatus::ready);
	return Status(EStatus::play);
}
bool IStatus::Stop() {
	if (_sts == EStatus::ready)return true;
	returnIfErrC(false, _sts != EStatus::play);
	return Status(EStatus::ready);
}
bool IStatus::Pause() {
	if (_sts == EStatus::pause)return true;
	returnIfErrC(false, _sts != EStatus::play);
	return Status(EStatus::pause);
}
bool IStatus::Resume() {
	if (_sts == EStatus::play)return true;
	returnIfErrC(false, _sts != EStatus::pause);
	return Status(EStatus::play);
}
#endif
#endif