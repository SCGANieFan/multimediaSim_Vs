#include "smf_api_shm.h"
#include "smf_debug.h"
#include "IPool.h"
#include "ShmFifo.h"
using namespace smf;

EXTERNC void* smf_shm_open(const char* shm_name, int buff_size, int fifo_max) {
	return ShmFifo::Open(shm_name, buff_size, fifo_max);
}

EXTERNC void* smf_shm_open_slave(const char* shm_name, uint32_t timeout) {
	return ShmFifo::Open(shm_name, timeout);
}

EXTERNC bool smf_shm_close(void* shm_) {
	returnIfErrC(false, !shm_);
	auto shm = (ShmFifo*)shm_;
	return ShmFifo::Close(shm);
}

EXTERNC bool smf_shm_write_init(void* shm_, smf_media_t* media, bool nocopy, bool isring) {
	returnIfErrC(false, !shm_);
	auto shm = (ShmFifo*)shm_;
	returnIfErrC(false, !shm->InitWrite(true, nocopy, isring));
	if(media){
		return shm->Write((IMedia*)media);
	}else{
		return true;
	}
}

EXTERNC bool smf_shm_read_init(void* shm_, bool nocopy, bool partread) {
	returnIfErrC(false, !shm_);
	auto shm = (ShmFifo*)shm_;
	returnIfErrC(false, !shm->InitRead(true, nocopy, partread));
	return true;
}
EXTERNC smf_media_t* smf_shm_get_media(void* shm_, uint32_t timeout) {
	returnIfErrC(0, !shm_);
	auto shm = (ShmFifo*)shm_;
	return (smf_media_t*)shm->Read(timeout);
}
EXTERNC uint32_t smf_shm_get_fifo_max(void* shm_) {
	returnIfErrC(0, !shm_);
	auto shm = (ShmFifo*)shm_;
	return shm->GetFifoMax();
}
EXTERNC uint32_t smf_shm_get_fifo_used(void* shm_, bool isread) {
	returnIfErrC(0, !shm_);
	auto shm = (ShmFifo*)shm_;
	return shm->GetFifoUsed(isread);
}
EXTERNC uint32_t smf_shm_get_fifo_left(void* shm_, bool isread) {
	returnIfErrC(0, !shm_);
	auto shm = (ShmFifo*)shm_;
	return shm->GetFifoLeft(isread);
}

EXTERNC bool smf_shm_write(void* shm_, smf_frame_t* frm) {
	returnIfErrC(false, !shm_);
	returnIfErrC(false, !frm);
	auto shm = (ShmFifo*)shm_;
	return shm->Write(*(Frame*)frm);
}
EXTERNC bool smf_shm_read(void* shm_, smf_frame_t* frm) {
	returnIfErrC(false, !shm_);
	returnIfErrC(false, !frm);
	auto shm = (ShmFifo*)shm_;
	return shm->Read(*(Frame*)frm);
}
EXTERNC bool smf_shm_set_volume(void* shm_, uint16_t volume) {
	returnIfErrC(false, !shm_);
	auto shm = (ShmFifo*)shm_;
	return shm->SetVolume(volume);
}

EXTERNC bool smf_shm_set_mute(void* shm_, bool mute) {
	returnIfErrC(false, !shm_);
	auto shm = (ShmFifo*)shm_;
	return shm->SetVolume(mute);
}
