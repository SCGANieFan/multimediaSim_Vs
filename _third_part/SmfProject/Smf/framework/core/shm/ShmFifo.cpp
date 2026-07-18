#include "ShmFifo.h"
#include "IPool.h"
#include "smf_shm_fifo.h"
#include "smf_shm_fifo_def.h"
#include "smf_debug.h"
#include "SmfPorting.h"
using namespace smf;

bool ShmFifo::Valid(bool cacheable)const {
	return smf_shm_fifo_valid((smf_shm_fifo_t*)this, cacheable);
}
bool ShmFifo::WriteValid()const {
	return smf_shm_fifo_write_valid((smf_shm_fifo_t*)this);
}
bool ShmFifo::ReadValid()const {
	return smf_shm_fifo_read_valid((smf_shm_fifo_t*)this);
}
bool ShmFifo::MediaValid()const {
	return smf_shm_fifo_media_valid((smf_shm_fifo_t*)this);
}
uint32_t ShmFifo::Size()const {
	auto shm = *(smf_shm_fifo_t*)this;
	return shm._total;
}
uint32_t ShmFifo::GetSize(int bmax, int fmax) {
	return smf_shm_fifo_get_size(bmax, fmax);
}
bool ShmFifo::Initialize(int total, int data_size, int fifo_max, bool cacheable, bool nomedia, bool reset, uint8_t align) {
	return smf_shm_fifo_initialize_all((smf_shm_fifo_t*)this, total, data_size, fifo_max, cacheable, nomedia, reset, align);
}
bool ShmFifo::DeInitialize() {
	return smf_shm_fifo_deinit((smf_shm_fifo_t*)this);
}

bool ShmFifo::InitWrite(bool cacheable, bool nocopy, bool isring) {
	return smf_shm_fifo_init_write((smf_shm_fifo_t*)this, cacheable, nocopy, isring);
}
bool ShmFifo::InitRead(bool cacheable, bool nocopy, bool partread) {
	return smf_shm_fifo_init_read((smf_shm_fifo_t*)this, cacheable, nocopy, partread);
}

bool ShmFifo::Set(set_e key, void* value) {
	return smf_shm_fifo_set((smf_shm_fifo_t*)this, (enum smf_shm_fifo_set_e)key, value);
}
bool ShmFifo::SetWrite(set_e key, void* value) {
	return smf_shm_fifo_set_write((smf_shm_fifo_t*)this, (enum smf_shm_fifo_set_e)key, value);
}
bool ShmFifo::SetRead(set_e key, void* value) {
	return smf_shm_fifo_set_read((smf_shm_fifo_t*)this, (enum smf_shm_fifo_set_e)key, value);
}
//bool ShmFifo::Sync(uint32_t& idx, int offset) {
//	SmfFifoX<pack_t> fifo(_fifo, _fifo_max, _w.fidx, idx);
//	fifo.ReadSync(offset);
//	idx = fifo.ReadIndex();
//	return true;
//}
bool ShmFifo::Sync(int offset) {
	return smf_shm_fifo_sync((smf_shm_fifo_t*)this, offset);
}

bool ShmFifo::SendPrepare(mem_t& mem, uint32_t size) {
	return smf_shm_fifo_send_prepare((smf_shm_fifo_t*)this, (smf_shm_fifo_mem_t*)&mem, size);
}
bool ShmFifo::Send(mem_t& mem, Frame& frm) {
	smf_shm_fifo_frame_t pack{
	(uint32_t)frm.size,//.size = (uint32_t)frm.size,
	(uint32_t)frm.size,//.max = (uint32_t)frm.size,
	frm.flags,//.flags = frm.flags,
	frm.ext,//.ext = frm.ext,
	frm.index,//.index = frm.index,
	frm.timestamp,//.timestamp = frm.timestamp,
	frm.Data(),//.data = frm.Data(),
	frm.sinfo ? &frm.sinfo->forward : 0,//.forward = frm.sinfo ? &frm.sinfo->forward : 0,
	frm.sinfo ? &frm.sinfo->backward : 0,//.backward = frm.sinfo ? &frm.sinfo->backward : 0
	frm.dts,
	frm.gts
	};
	returnIfErrC(false, !smf_shm_fifo_send((smf_shm_fifo_t*)this, (smf_shm_fifo_mem_t*)&mem, &pack));
	frm.size = 0;
	return true;
}

bool ShmFifo::Write(Frame& frm) {
	auto shm = *(smf_shm_fifo_t*)this;
	smf_shm_fifo_frame_t pack{
		(uint32_t)frm.size,//.size = (uint32_t)frm.size,
		(uint32_t)frm.size,//.max = (uint32_t)frm.size,
		frm.flags,//.flags = frm.flags,
		frm.ext,//.ext = frm.ext,
		frm.index,//.index = frm.index,
		frm.timestamp,//.timestamp = frm.timestamp,
		frm.Data(),//.data = frm.Data(),
		frm.sinfo ? &frm.sinfo->forward : 0,//.forward = frm.sinfo ? &frm.sinfo->forward : 0,
		frm.sinfo ? &frm.sinfo->backward : 0,//.backward = frm.sinfo ? &frm.sinfo->backward : 0
		frm.dts,
		frm.gts
	};
	if (frm.media) {
		if (shm._up.mmagic != eSmfShmMagic) {dbgTestPL();
			smf_shm_fifo_write_media((smf_shm_fifo_t*)this, (smf_shm_fifo_media_t*)frm.media);
		}
		else if (frm.Media()->MediaChanged()) {dbgTestPL();
			smf_shm_fifo_write_media((smf_shm_fifo_t*)this, (smf_shm_fifo_media_t*)frm.media);
			frm.Media()->MediaChange(false);
		}
	}
	returnIf(false, !smf_shm_fifo_write((smf_shm_fifo_t*)this, &pack));
	frm.size = 0;
	return true;
}

bool ShmFifo::Read(Frame& frm) {
	smf_shm_fifo_frame_t pack{
		0,//size = 0,
		0,//.max = 0,
		0,//.flags = 0,
		0,//.ext = 0,
		0,//.index = 0,
		0,//.timestamp = 0,
		0,//.data = 0,
		frm.sinfo ? &frm.sinfo->forward : 0,//.forward = frm.sinfo ? &frm.sinfo->forward : 0,
		frm.sinfo ? &frm.sinfo->backward : 0,//.backward = frm.sinfo ? &frm.sinfo->backward : 0
		0,//dts
		0//gts
	};
	auto shm = *(smf_shm_fifo_t*)this;
	//returnIf(false, !smf_shm_fifo_read((smf_shm_fifo_t*)this, &pack));
	if (shm._r.nocopy) {
		returnIf(false, !smf_shm_fifo_read((smf_shm_fifo_t*)this, &pack));
		returnIfErrC(false, frm.size);
		frm.buff = pack.data;
		frm.size = pack.size;
		frm.offset = 0;
		frm.max = pack.size;
	}
	else {
		pack.data = frm.LeftData();
		pack.max = frm.Left();
		returnIf(false, !smf_shm_fifo_read((smf_shm_fifo_t*)this, &pack));
		frm.size += pack.size;

	}
	frm.index = pack.index;
	frm.timestamp = pack.timestamp;
	frm.flags = pack.flags;
	//frm.priv = pack.priv;
	frm.ext = pack.ext;
	frm.media = (smf_media_def_t*)((char*)(smf_shm_fifo_t*)this + shm._media);
	frm.dts = pack.dts;
	frm.gts = pack.gts;
	return true;
}

bool ShmFifo::Write(IMedia* media) {
	return smf_shm_fifo_write_media((smf_shm_fifo_t*)this, (smf_shm_fifo_media_t*)media);
}

IMedia* ShmFifo::Read(uint32_t timeout) {
	return (IMedia*)smf_shm_fifo_get_media32((smf_shm_fifo_t*)this, timeout);
}

bool ShmFifo::SetVolume(uint16_t volume) {
	return smf_shm_fifo_set_volume((smf_shm_fifo_t*)this, volume);
}

bool ShmFifo::SetMute(bool mute) {
	return smf_shm_fifo_set_mute((smf_shm_fifo_t*)this, mute);
}

bool ShmFifo::SetSInfo(sinfo_e keys, uint32_t vals) {
	return smf_shm_fifo_sinfo_set((smf_shm_fifo_t*)this, (smf_shm_fifo_sinfo_e)keys, vals);
}

bool ShmFifo::GetSInfo(sinfo_e keys, uint32_t&vals) {
	return smf_shm_fifo_sinfo_get((smf_shm_fifo_t*)this, (smf_shm_fifo_sinfo_e)keys, &vals);
}

ShmFifo* ShmFifo::Open(const char* shm_name, int buff_size, int fifo_max, int shared_index, bool reset, uint8_t align) {
	auto total = ShmFifo::GetSize(buff_size, fifo_max);
	auto shm = (ShmFifo*)Pools().Shared(shared_index).Alloc(shm_name, total, 64);
	returnIfErrC(0, !shm);
	if (!shm->Initialize(total, buff_size, fifo_max, true, false, reset, align)) {
		dbgErrPL();
		Close(shm);
		return 0;
	}
	return shm;
}
ShmFifo* ShmFifo::Open(const char* shm_name, uint32_t timeout, int shared_index) {
	uint32_t total = 0;
	auto shm = (ShmFifo*)Pools().Shared(shared_index).Alloc(shm_name, total);
	returnIfErrC(0, !shm);
	if (timeout) {
		uint32_t ms = 0;
		while (ms < timeout) {
			if (shm->Valid(true))
				break;
			ms += 10;
			sleep_for(10);
		}
	}
	return shm;
}
bool ShmFifo::Close(ShmFifo* shm, int shared_index) {
	shm->DeInitialize();
	return Pools().Shared(shared_index).Free(shm);
}

char* ShmFifo::Print(char* ptr, char* end)const {
	auto& shm = *(smf_shm_fifo_t*)this;
	ptr = snprintf(ptr, end, "%08x(%u,%u)%u(%u,%u)%u(%u,%u)"
		, &shm, shm._fifo_max, shm._data_size
		, shm._w.fidx - shm._r.fidx, shm._r.fidx, shm._w.fidx
		, (uint32_t)(shm._w.bidx - shm._r.bidx), (uint32_t)shm._r.bidx, (uint32_t)shm._w.bidx
	);
	return ptr;
}

uint32_t ShmFifo::GetFifoMax()const { return smf_shm_fifo_get_max((smf_shm_fifo_t*)this); }
uint32_t ShmFifo::GetBuffMax()const { return smf_shm_fifo_get_buff_max((smf_shm_fifo_t*)this); }
uint32_t ShmFifo::GetFifoUsed(bool isread)const { return smf_shm_fifo_get_used((smf_shm_fifo_t*)this,isread); }
uint32_t ShmFifo::GetFifoLeft(bool isread)const { return smf_shm_fifo_get_left((smf_shm_fifo_t*)this, isread); }
uint32_t ShmFifo::GetBuffUsed(bool isread)const { return smf_shm_fifo_get_buff_used((smf_shm_fifo_t*)this, isread); }
uint32_t ShmFifo::GetBuffLeft(bool isread)const { return smf_shm_fifo_get_buff_left((smf_shm_fifo_t*)this, isread); }

bool ShmFifo::Empty(bool isread)const { return GetFifoUsed(isread) == 0; }
bool ShmFifo::Full(bool isread)const { return GetFifoLeft(isread) == 0; }
