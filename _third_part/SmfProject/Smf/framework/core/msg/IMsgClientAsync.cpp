#include "IMsgClientAsync.h"
#include "SmfHash.h"
#include "thread.h"
#include "smf_debug.h"
#include "SmfPorting.h"

using namespace smf;
using namespace smf::msg;
IMsgClientAsync::IMsgClientAsync() {
	_flags.Set(IS_ASync);
}
IMsgClientAsync::~IMsgClientAsync() {
	_loop = false;
	sleep_for(10);
}
bool IMsgClientAsync::init(IMsgService* svc,bool en){
    //dbgTestPXL("%p,%d",svc, en);
	if (en) {
		//_svc = svc;
		_loop = true;
		thread thrCmd;
		thrCmd.Start(Name(), [](void* priv) {((IMsgClientAsync*)priv)->Loop(); }, this, _stackSize);
		thrCmd.detach();
	}
	else {
		_loop = false;
	}
	return true;
}

Result IMsgClientAsync::Process(smf_msg_t* msg) {
	//auto rst = check(msg);
	//returnIfErrC(rst, rst != Result::True);
	return check(msg) != Result::Other ? (Result)receive(msg) : Result::Other;
}

bool IMsgClientAsync::set(uint32_t key, void* val) {
	switch (key) {
	case Hash("stackSize") :_stackSize = (uint32_t)val; return true;
	}
	return IMsgClient::set(key, val);
}

bool IMsgClientAsync::receive(smf_msg_t* msg_) {
	//dbgTestPL();
    auto idx = _fifos.WriteIndex() % _fifos.Max();
    auto msg = (smf_msg_t*)_fifoBuffs[idx];
    memcpy(msg, msg_, msg_->size);
    _fifos.Write(msg);
    _sem.post();
    return true;
}

void IMsgClientAsync::Loop(void) {
	dbgTestPL();
	while (_loop) {
        _sem.wait();
        smf_msg_t* msg = 0;
		if (!_fifos.Read(msg))continue;
        if (!msg)continue;
		process(msg);
 	}
}

