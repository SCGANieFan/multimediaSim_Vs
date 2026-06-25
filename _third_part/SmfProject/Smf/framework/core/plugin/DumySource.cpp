#include "DumySource.h"
#include "SmfHash.h"
#include "smf_debug.h"
using namespace smf;

EXTERNC void smf_dumy_source_register(){
	DumySource::Register<DumySource>("src-dumy");
}
bool DumySource::open(void*para) {
	if (!oMedia()) {
		_oport->Set("pcm");
	}
	return Source::open(para);
}
bool DumySource::set(uint32_t key, void* val) {
	switch (key) {
	case Hash("codec"):_oport->Set((const char*)val); return  true;
	case Hash("codec32"):_oport->Set((const char*)&val);  return  true;
	case Hash("ms"):_intervalMs = (uint32_t)val; return  true;
	case Hash("ec"):
	case Hash("eosCnt"):_eosCount = (uint32_t)val; return  true;
	}
    return Source::set(key, val);
}
bool DumySource::generateFrame(Frame*& ofrm){
	if (!_timestamp) {
		_timestamp = get_local_ms();
		ofrm->TimeStampReset();
	}
	auto tpoffset = _intervalMs * _count;
	if (_eosCount && _count > _eosCount) {
		//if (_flags.Check(IS_Live))
		//	sleep_until(nextMs);
		//SendMessage("eos", smf_direction_upward);
		return false;
	}
	ofrm->size = ofrm->max - ofrm->offset;
	memset((char*)ofrm->buff + ofrm->offset, ofrm->index & 0xff, ofrm->size);
	ofrm->timestamp = tpoffset;
	ofrm->index = _count++;
	if (_eosCount && _count == _eosCount) {
		ofrm->flags |= SMF_FRAME_IS_EOS;
		dbgTestPXL("eos");
	}
	//if (_flags.Check(IS_Log))
	//	dbgTestPXL("%s[%d]%p,%d,%d,%d,0x%08x", Name(), ofrm->index, ofrm->buff, ofrm->offset, ofrm->size, ofrm->max, ofrm->flags);
	if (_flags.Check(IS_Live))
		sleep_until(_timestamp + tpoffset);
    return true;
}

