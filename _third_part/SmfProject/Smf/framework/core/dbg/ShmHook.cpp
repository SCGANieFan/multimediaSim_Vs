#include "ShmHook.h"
#include "SmfPorting.h"
#include "smf_debug.h"
using namespace smf;
ShmHook::ShmHook(uint8_t pointer, uint32_t buffmax, uint32_t fifomax, mutex* mtx) {
	init(pointer, buffmax, fifomax, mtx);
}
ShmHook::ShmHook(uint8_t pointer, ShmFifo* shm, mutex* mtx) {
	_pointer = pointer;
	_mtx = mtx;
	_shm = shm;
}
ShmHook::~ShmHook() {
	if (_shm && _shmDelete) {
		ShmFifo::Close(_shm);
	}
}

bool ShmHook::init(uint8_t pointer, uint32_t buffmax, uint32_t fifomax, mutex* mtx) {
	_pointer = pointer;
	_mtx = mtx;
	char name[16];
	sprintf(name, "hook_%s", get_cpu_name());
	auto shm = ShmFifo::Open(name, buffmax, fifomax);
	returnIfErrC(false, !shm);
	_shm = shm;
	_shmDelete = true;
	return true;
}

bool ShmHook::Hook(Frame*& frm_) {
	if (!frm_)return true;
	Frame frm = *frm_;
	frm.ext = _pointer;
	frm.sinfo = 0;
	_datacnt++;
	_datasize += frm.size;
	unique_lock<mutex> lck(_mtx);
	return _shm->Write(frm);
}
