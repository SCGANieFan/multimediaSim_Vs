#include "Service.h"
#include "SmfHash.h"
#include "SmfFCC.h"
#include "smf_debug.h"
using namespace smf;

///for path_smf_api
bool Service::Open(void* para) {
	if (para) {
		returnIfErrC(false, !setParam(para));
	}
	//_openParam = para;
	return Status(EStatus::ready, EStatusChange::up);
}
bool Service::Close() {
	return Status(EStatus::null, EStatusChange::down);
}
bool Service::setParam(void*para) {
	_openParam = para;
	return true;
}
