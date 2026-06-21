#include "Service.h"
#include "SmfHash.h"
#include "SmfFCC.h"
#include "smf_debug.h"
using namespace smf;

Service::Service() {
	_flags.Set(IS_Service);
}
Service::~Service() {
	if (_sts != EStatus::null) {
		dbgWarnPXL("[%s]no close!", Name());
		Status(EStatus::null);
	}
}
bool Service::set(uint32_t key, void* val) {
	switch (key) {
	case Hash("sts"):
	case SMF_PARAM_Status:return Status((EStatus)(int)val);
	case Hash("stsup"):return Status((EStatus)(int)val,EStatusChange::up);
	case Hash("stsdown"):return Status((EStatus)(int)val,EStatusChange::down);
	case SMF_PARAM_OpenParam:return setParam(val);
	}
	return Object::set(key, val);
}
bool Service::get(uint32_t key, void* val) const {
	switch (key) {
	case Hash("sts"):
	case SMF_PARAM_Status:*(EStatus*)val = Status();return true;
	}
	return Object::get(key, val);
}
#if 0
bool Service::processMessage(smf_message_t&msg) {
	switch (msg.id) {
	case fcc64("eos"):
		dbgInfoPXL("[eos]%s/%s", Name(), ((Object*)msg.creater)->Name());
		_error.err = SMF_ERROR_IO_EOS;
		//if(!_parent)Status(EStatus::null);
		return false;
	case fcc64("error"):
		dbgInfoPXL("[error]%s/%s,%d,%08x,%08x", Name(), ((Object*)msg.creater)->Name(), _error.err, _error.err32[0], _error.err32[1]);
		//if(!_parent)Status(EStatus::null);
		return false;
	default:
		break;
	}
	return Object::processMessage(msg);
}
#endif
bool Service::statusChange(EStatus target) {
	returnIfErrC(false,_sts!=_sts0);
	auto sts = _sts;
	_sts0 = sts;
	if (sts < target) {
		while (sts < target) {
			sts = (EStatus)((int)sts + 1);
			_sts0 = sts;
			returnIfErrC(false, !statusChangeStep(sts));
			_sts = sts;
		}
	}
	else if (sts > target) {
		while (sts > target) {
			sts = (EStatus)((int)sts - 1);
			_sts0 = sts;
			returnIfErrC(false, !statusChangeStep(sts));
			_sts = sts;
		}
	}
	else {

	}
	return true;
}
bool Service::statusChangeStep(EStatus sts) {
	if (_sts < sts) {
		switch (_sts) {
		case EStatus::null:
			if (!open(_openParam)) {
				dbgErrPXL("[%s]open()", Name());
				close();
				return false;
			}
			break;
		case EStatus::ready:
			if (!start()) {
				dbgErrPXL("[%s]start()", Name());
				stop();
				return false;
			}
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
bool Service::statusChangeSteps(EStatus sts, Object** list, int N) {
	if (_sts < sts) {
		for (int i = 0; i < N; i++) {
			auto obj = list[i];
			if (obj && obj->Flags().Check(IS_Service)) {
				returnIfErrC(false, !((Service*)obj)->Status(sts, EStatusChange::up));
			}
		}
	}
	else {
		for (int i = N - 1; i >= 0; i--) {
			auto obj = list[i];
			if (obj && obj->Flags().Check(IS_Service)) {
				((Service*)obj)->Status(sts, EStatusChange::down);
			}
		}
	}
	return true;
}
bool Service::Status(EStatus sts, EStatusChange chg) {
	switch (chg) {
	case EStatusChange::null:break;
	case EStatusChange::up:	if ((int)sts <= (int)_sts)return true; break;
	case EStatusChange::down: if ((int)sts >= (int)_sts)return true; break;
	}
	auto rst = statusChange(sts);
	_sts0 = _sts;
	return rst;
}
EStatus Service::Status()const { return _sts; }
EStatus Service::StatusNext()const { return _sts0; }
bool Service::open(void* para) { return true; }
bool Service::close() { return true; }
bool Service::start() { return true; }
bool Service::stop() { return true; }
bool Service::pause() { return true; }
bool Service::resume() { return true; }
bool Service::IsNone()const { return _sts == EStatus::null; }
bool Service::IsOpen()const { return _sts >= EStatus::ready; }
bool Service::IsPlaying()const { return _sts == EStatus::play; }
bool Service::IsPaused()const { return _sts == EStatus::pause; }


char* Service::print(char* ptr, char* end) const {
	ptr = Object::print(ptr, end);
	if (_sts != _sts0) {
		ptr += snprintf(ptr, end - ptr, "{%u%u}", _sts, _sts0);
	}
	return ptr;
}
