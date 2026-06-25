#include "DumySink.h"
#include "smf_debug.h"
using namespace smf;
///
EXTERNC void smf_dumy_sink_register() {
	DumySink::Register<DumySink>("sink-dumy");
}
/// 
bool DumySink::open(void*) {
	dbgTestPL();
	return true;
}
bool DumySink::close() {
	dbgTestPL();
	return true;
}
bool DumySink::receiveFrame(Frame*frm) {
	//auto media = frm->Media();
	//if(_flags.Check(IS_Log))
	//	dbgTestPXL("%s[%d,%d]%p,%d,%d,%d,0x%08x,%p,%s",Name(),frm->index,frm->index- _index,frm->buff, frm->offset, frm->size, frm->max, frm->flags, media, media?(const char*)&media->_codec:"");
	//dbgTestDump((char*)frm->buff+frm->offset, 8);
	frm->size = 0;
	_index = frm->index;
	if (_interval) {
		if (!_tp)_tp = get_local_ms();
		_tp += _interval;
		sleep_until(_tp);
	}
	return true;
}

bool DumySink::set(uint32_t key, void* val) {
	switch (key) {
	case Hash("sync"):_interval = (uint32_t)val; return true;
	}
	return Sink::set(key, val);
}
