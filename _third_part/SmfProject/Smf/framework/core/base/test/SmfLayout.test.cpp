#include "SmfLayout.h"
#include "SmfFCC.h"
#include "SmfPorting.h"
using namespace smf;

EXTERNC bool smf_test_layout(const char* serial,uint32_t* params) {
	auto obj = Layout::Create(serial, 0, params);
	returnIfErrC(false, !obj);
	bool loop = true;
	if (!obj->Register([](smf_message_t* msg) {
		if (msg->id == fcc64("error")) {
			*((bool*)msg->priv) = false;
			return true;
		}
		if (msg->id == fcc32("eos")) {
			*((bool*)msg->priv) = false;
			return true;
		}
		return false;
		},&loop)) {
		dbgErrPL();
		return false;
	}
	returnIfErrC(false, !obj->Set("sts", 2));
	while (loop)
		sleep_for(100);
	returnIfErrC(false, !obj->Set("sts", 0));
	delete obj;
	return true;
}
