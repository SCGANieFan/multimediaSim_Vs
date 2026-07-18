#include "smf_shm_fifo.h"
#include "smf_shm_fifo_def.h"
#include "smf_debug.h"
//
#ifndef ALIGN64
#define ALIGN64(x) (((x) + 63) & ~63)
#define ALIGN_64(x) (((x)) & ~63)
#endif
//
extern void *memcpy(void *destin, const void *source, size_t n);
extern void *memset(void *s, int ch, size_t n);
extern void cache_flush(void* data, uint32_t size);
extern void cache_invalid(void* data, uint32_t size);
extern void sleep_for(uint32_t ms);
extern void smf_media_update_vtable(smf_shm_fifo_media32_t* media);
//
static uint32_t buff_alloc(smf_shm_fifo_t* shm, uint32_t size, uint64_t*pi);
static void buff_refund(smf_shm_fifo_t* shm, uint32_t size);
static void buff_free(smf_shm_fifo_t* shm, uint64_t pi, uint32_t size);
//
static smf_shm_fifo_pack_t* fifo_write_begin(smf_shm_fifo_t* shm);
static void fifo_write_end(smf_shm_fifo_t* shm);
static smf_shm_fifo_pack_t* fifo_read_begin(smf_shm_fifo_t* shm);
static void fifo_read_end(smf_shm_fifo_t* shm);
static void fifo_read_sync(smf_shm_fifo_t* shm, int offset);
//
static bool media_write(smf_shm_fifo_t* shm, smf_shm_fifo_media_t* media);
static bool media_read(smf_shm_fifo_t* shm, smf_shm_fifo_media_t* media);
static bool media_update(smf_shm_fifo_t* shm);
static bool media_wait(smf_shm_fifo_t* shm, uint32_t timeout);
//
static inline smf_shm_fifo_media32_t* get_media(smf_shm_fifo_t* shm);
static inline char* get_media_extra(smf_shm_fifo_t* shm);
static inline smf_shm_fifo_pack_t* get_fifo(smf_shm_fifo_t* shm);
// static inline char* get_data(smf_shm_fifo_t* shm);
//
uint32_t smf_shm_fifo_get_size(int bmax, int fmax) {
	return sizeof(smf_shm_fifo_t) + ALIGN64(sizeof(smf_shm_fifo_pack_t) * fmax) + ALIGN64(bmax);
}

bool smf_shm_fifo_valid(smf_shm_fifo_t* shm, bool cacheable) {
	returnIfErrC(false, !shm);
	//dbgTestPXL("%p,%d,%08x", shm, cacheable, shm->_magic);
	if(cacheable)cache_invalid(shm->_cblk, eSmfShmBlockSize);
	return shm->_magic == eSmfShmMagic;
}
bool smf_shm_fifo_write_valid(smf_shm_fifo_t* shm) {
	returnIfErrC(false, !shm);
	returnIf(false, !smf_shm_fifo_valid(shm, shm->_w.cacheable));
	//dbgTestPXL("%p,%d,%p,%08x", shm, shm->_w.cacheable, &shm->_w.magic, shm->_w.magic);
	if (shm->_w.cacheable) cache_invalid(shm->_wblk, eSmfShmBlockSize);
	return shm->_w.magic == eSmfShmMagic;
}
bool smf_shm_fifo_read_valid(smf_shm_fifo_t* shm) {
	returnIfErrC(false, !shm);
	returnIf(false, !smf_shm_fifo_valid(shm, shm->_r.cacheable));
	//dbgTestPXL("%p,%d,%p,%08x", shm, shm->_r.cacheable, &shm->_r.magic, shm->_r.magic);
	if (shm->_r.cacheable) cache_invalid(shm->_rblk, eSmfShmBlockSize);
	return shm->_r.magic == eSmfShmMagic;
}
bool smf_shm_fifo_media_valid(smf_shm_fifo_t* shm) {
	returnIfErrC(false, !shm);
	returnIf(false, !smf_shm_fifo_valid(shm, shm->_r.cacheable));
	//dbgTestPXL("%p,%d,%p,%08x", shm, shm->_r.cacheable, &shm->_up.mmagic, shm->_up.mmagic);
	if (shm->_r.cacheable) cache_invalid(shm->_wblk, eSmfShmBlockSize);
	return shm->_up.mmagic == eSmfShmMagic;
}
bool smf_shm_fifo_initialize(smf_shm_fifo_t* shm, int size, int data_size, int fifo_max, bool cacheable, bool nomedia) {
	return smf_shm_fifo_initialize_all(shm, size, data_size, fifo_max, cacheable, nomedia, false, 0);
}
bool smf_shm_fifo_initialize_all(smf_shm_fifo_t* shm, int size, int data_size, int fifo_max, bool cacheable, bool nomedia, bool reset, uint8_t align) {
	//dbgTestPXL("%p,%d,%d,%d,%u,%u", shm, size, data_size, fifo_max, cacheable, nomedia);
	returnIfErrC(false, !shm);
	returnIfErrC(false, (char*)&shm->_mvtable + 4 != shm->_mblk);
	//
	if (cacheable) cache_invalid(shm, eSmfShmBlockSize);
	if (reset) memset(shm, 0, sizeof(smf_shm_fifo_t));
	if (shm->_magic == eSmfShmMagic) return true;
	memset(shm->_cblk, 0, eSmfShmBlockSize);
	//
	uint32_t total = smf_shm_fifo_get_size(data_size, fifo_max);
	returnIfErrC(false, size < total);
	uint32_t fifo_size = ALIGN64(sizeof(smf_shm_fifo_pack_t) * fifo_max);
	shm->_total = total;
	shm->_fifo_size = fifo_size;
	shm->_data_size = data_size;
	shm->_update_size = total - eSmfShmBlockSize - eSmfShmBlockSize;
	shm->_fifo_max = fifo_max;
	shm->_fifo = sizeof(smf_shm_fifo_t);
	shm->_data = shm->_fifo + fifo_size;
	if (align < 8)align = 8;
	shm->_align = align;
	if (!nomedia) {
		shm->_media = (char*)&shm->_mvtable - (char*)shm;
		shm->_media_max = sizeof(shm->_mblk) + 4;
	}
	//
	shm->_magic = eSmfShmMagic;
	if (cacheable) cache_flush(shm->_cblk, eSmfShmBlockSize);
	return true;
}

bool smf_shm_fifo_init_write(smf_shm_fifo_t* shm, bool cacheable, bool nocopy, bool isring) {
	dbgTestPXL("%p,%d,%d,%d", shm, cacheable, nocopy, isring);
	returnIfErrC(false, !shm);
	returnIfErrC(false, (uint32_t)shm & 63);
	if (cacheable) cache_invalid(shm, shm->_total);
	memset(shm->_wblk, 0, eSmfShmBlockSize);
	shm->_w.addr = (uint64_t)((uintptr_t)shm);
	shm->_w.nocopy = nocopy;
	shm->_w.cacheable = cacheable;
	shm->_w.isring = isring;
	shm->_w.magic = eSmfShmMagic;
	shm->_sf.audio.volume = 32768;
	if (cacheable) cache_flush(shm->_wblk, eSmfShmBlockSize);
	//dbgTestPXL("%p,%d,%d,%d", shm, shm->_w.cacheable, shm->_w.nocopy, shm->_w.isring);
	//dbgTestPXL("%p,%p", &shm->_w.magic, shm->_w.magic);
	return true;
}

bool smf_shm_fifo_init_read(smf_shm_fifo_t* shm, bool cacheable, bool nocopy, bool partread) {
	dbgTestPXL("%p,%d,%d,%d",shm, cacheable, nocopy, partread);
	returnIfErrC(false, !shm);
	returnIfErrC(false, (uint32_t)shm & 63);
	if (cacheable) cache_invalid(shm, shm->_total);
	memset(shm->_rblk, 0, eSmfShmBlockSize);
	shm->_r.addr = (uint64_t)((uintptr_t)shm);
	shm->_r.nocopy = nocopy;
	shm->_r.cacheable = cacheable;
	shm->_r.partread = partread;
	shm->_r.magic = eSmfShmMagic;
	if (cacheable) cache_flush(shm->_rblk, eSmfShmBlockSize);
	//dbgTestPXL("%p,%d,%d,%d", shm, shm->_r.cacheable, shm->_r.nocopy, shm->_r.partread);
	//dbgTestPXL("%p,%p", &shm->_r.magic, shm->_r.magic);
	return true;
}

bool smf_shm_fifo_deinit(struct smf_shm_fifo_t* shm) {
	returnIfErrC(false, !shm);
	uint32_t size = shm->_total;
	cache_invalid(shm, size);
	return true;
}

bool smf_shm_fifo_set(struct smf_shm_fifo_t* shm, enum smf_shm_fifo_set_e key, void* value) {
	returnIfErrC(false, !shm);
	switch (key) {
	default:dbgErrPXL("%u:%p", key, value); return false;
	}
}

bool smf_shm_fifo_set_write(struct smf_shm_fifo_t* shm, enum smf_shm_fifo_set_e key, void* value) {
	returnIfErrC(false, !shm);
	smf_shm_fifo_block_t* blk = &shm->_w;
	switch (key) {
	case SMF_SHM_FIFO_SET_no_forward: blk->no_forward = (uint32_t)value;return true;
	case SMF_SHM_FIFO_SET_no_backward: blk->no_backward = (uint32_t)value;return true;
	default:dbgErrPXL("%u:%p", key, value); return false;
	}
}

bool smf_shm_fifo_set_read(struct smf_shm_fifo_t* shm, enum smf_shm_fifo_set_e key, void* value) {
	returnIfErrC(false, !shm);
	smf_shm_fifo_block_t* blk = &shm->_r;
	switch (key) {
	case SMF_SHM_FIFO_SET_no_forward: blk->no_forward = (uint32_t)value; return true;
	case SMF_SHM_FIFO_SET_no_backward: blk->no_backward = (uint32_t)value; return true;
	default:dbgErrPXL("%u:%p", key, value); return false;
	}
}

uint32_t smf_shm_fifo_get_max(struct smf_shm_fifo_t* shm) {
	return shm->_fifo_max;
}

uint32_t smf_shm_fifo_get_buff_max(struct smf_shm_fifo_t* shm) {
	return shm->_data_size;
}

uint32_t smf_shm_fifo_get_used(struct smf_shm_fifo_t* shm, bool isread) {
	if ((isread && shm->_r.cacheable) || (!isread && shm->_w.cacheable))cache_invalid(shm, sizeof(smf_shm_fifo_t));
	uint32_t mi = shm->_fifo_max;
	uint32_t wi = shm->_w.fidx;
	uint32_t ri = shm->_r.fidx;
	int used = wi - ri;
	if (used > mi)used = mi;
	return used;
}

uint32_t smf_shm_fifo_get_left(struct smf_shm_fifo_t* shm, bool isread) {
	if ((isread && shm->_r.cacheable) || (!isread && shm->_w.cacheable))cache_invalid(shm, sizeof(smf_shm_fifo_t));
	uint32_t mi = shm->_fifo_max;
	uint32_t wi = shm->_w.fidx;
	uint32_t ri = shm->_r.fidx;
	int used = wi - ri;
	if (used > mi)used = mi;
	return mi - used;
}

uint32_t smf_shm_fifo_get_buff_used(struct smf_shm_fifo_t* shm, bool isread) {
	if ((isread && shm->_r.cacheable) || (!isread && shm->_w.cacheable))cache_invalid(shm, sizeof(smf_shm_fifo_t));
	uint32_t mi = shm->_data_size;
	uint64_t wi = shm->_w.bidx;
	uint64_t ri = shm->_r.bidx;
	int used = wi - ri;
	if (used > mi)used = mi;
	return used;
}

uint32_t smf_shm_fifo_get_buff_left(struct smf_shm_fifo_t* shm, bool isread) {
	if ((isread && shm->_r.cacheable) || (!isread && shm->_w.cacheable))cache_invalid(shm, sizeof(smf_shm_fifo_t));
	uint32_t mi = shm->_data_size;
	uint64_t wi = shm->_w.bidx;
	uint64_t ri = shm->_r.bidx;
	int used = wi - ri;
	if (used > mi)used = mi;
	return mi - used;
}


bool smf_shm_fifo_sync(smf_shm_fifo_t* shm, int offset) {
	returnIfErrC(false, !shm);
	if (shm->_r.cacheable)cache_invalid(shm, shm->_total);
	fifo_read_sync(shm, offset);
	if (shm->_r.cacheable)cache_flush(shm->_rblk, eSmfShmBlockSize);
	return true;
}

bool smf_shm_fifo_write(smf_shm_fifo_t* shm, smf_shm_fifo_frame_t* frm) {
	returnIfErrC(false, !shm);
	// dbgTestPXL("%u,%u,%u", (uint32_t) shm->_w.fidx,  (uint32_t)shm->_r.fidx, (uint32_t)shm->_fifo_max);
	if (shm->_w.cacheable) {
		cache_invalid(shm->_rblk, eSmfShmBlockSize);
	}
	smf_shm_fifo_pack_t* pack = fifo_write_begin(shm);
	returnIf(false, !pack);
	pack->size = frm->size;
	pack->flags = frm->flags;
	pack->index = frm->index;
	pack->ext = frm->ext;
	pack->timestamp = frm->timestamp;
	pack->dts = frm->dts;
	pack->gts = frm->gts;
	if (shm->_w.nocopy) {
		pack->ri = 0;
		pack->data = (uint32_t)frm->data;
		pack->offset = 0;
		if (shm->_w.cacheable) {
			cache_flush((void*)ALIGN_64((uint32_t)pack->data), ALIGN64(pack->size));
		}
	}
	else {
		pack->data = 0;
		pack->offset = buff_alloc(shm, frm->size, &pack->ri);
		returnIf(false, !pack->offset);
		memcpy((char*)shm + pack->offset, (char*)frm->data, frm->size);
	}
	if (frm->forward && !shm->_w.no_forward) {
		shm->_sf = *(smf_shm_fifo_sinfo_forward_t*)frm->forward;
	}
	if (frm->backward && !shm->_w.no_backward) {
		*(smf_shm_fifo_sinfo_backward_t*)frm->backward = shm->_sb;
	}
	if (shm->_w.cacheable) {//flush fifo & data
		cache_flush(get_fifo(shm), shm->_update_size - eSmfShmBlockSize*2 - eSmfShmBlockSize - eSmfShmBlockSize);
	}
	//
	fifo_write_end(shm);
	frm->size = 0;
	// dbgTestPXL("[%d]%p,%d,(%d)%d/%d,%d/%d", pack.index, pack.buff, pack.size, shm->_widx-_ridx, shm->_ridx, shm->_widx, shm->_woffset, shm->_roffset);
	if (shm->_w.cacheable) {//flush media & wblk
		//cache_flush(shm->_mblk, shm->_update_size);
		cache_flush(shm->_mblk, eSmfShmBlockSize*2 + eSmfShmBlockSize + eSmfShmBlockSize);
	}
	return true;
}

bool smf_shm_fifo_send_prepare(smf_shm_fifo_t* shm, smf_shm_fifo_mem_t* mem, uint32_t size) {
	returnIfErrC(false, !shm);
	returnIfErrC(false, !mem);
	returnIfErrC(false, !size);
	returnIfErrC(false, shm->_w.nocopy);
	// dbgTestPXL("%u,%u,%u", (uint32_t) shm->_w.fidx,  (uint32_t)shm->_r.fidx, (uint32_t)shm->_fifo_max);
	if (shm->_w.cacheable) {
		cache_invalid(shm->_rblk, eSmfShmBlockSize);
	}
	smf_shm_fifo_pack_t* pack = fifo_write_begin(shm);
	returnIf(false, !pack);
	uint64_t ri = 0;
	uint32_t offset = buff_alloc(shm, size, &ri);
	returnIf(false, !offset);
	mem->priv[0] = ri;
	mem->priv[1] = offset;
	mem->buff = (char*)shm + offset;
	mem->size = size;
	return true;
}

bool smf_shm_fifo_send(smf_shm_fifo_t* shm, smf_shm_fifo_mem_t* mem, smf_shm_fifo_frame_t* frm) {
	returnIfErrC(false, !shm);
	returnIfErrC(false, !mem);
	returnIfErrC(false, !frm);
	returnIfErrC(false, !mem->priv[1]);
	returnIfErrC(false, frm->size > mem->size);
	// dbgTestPXL("%u,%u,%u", (uint32_t) shm->_w.fidx,  (uint32_t)shm->_r.fidx, (uint32_t)shm->_fifo_max);
	if (shm->_w.cacheable) {
		cache_invalid(shm->_rblk, eSmfShmBlockSize);
	}
	smf_shm_fifo_pack_t* pack = fifo_write_begin(shm);
	returnIf(false, !pack);
	pack->size = frm->size;
	pack->flags = frm->flags;
	pack->index = frm->index;
	pack->ext = frm->ext;
	pack->timestamp = frm->timestamp;
	pack->dts = frm->dts;
	pack->gts = frm->gts;
	pack->data = 0;
	pack->offset = mem->priv[1];
	pack->ri = mem->priv[0];
	if (frm->size < mem->size)buff_refund(shm, mem->size - frm->size);

	if (frm->forward)shm->_sf = *(smf_shm_fifo_sinfo_forward_t*)frm->forward;
	if (frm->backward)*(smf_shm_fifo_sinfo_backward_t*)frm->backward = shm->_sb;
	if (shm->_w.cacheable) {//flush fifo & data
		cache_flush(get_fifo(shm), shm->_update_size - eSmfShmBlockSize * 2 - eSmfShmBlockSize - eSmfShmBlockSize);
	}
	//
	fifo_write_end(shm);
	frm->size = 0;
	// dbgTestPXL("[%d]%p,%d,(%d)%d/%d,%d/%d", pack.index, pack.buff, pack.size, shm->_widx-_ridx, shm->_ridx, shm->_widx, shm->_woffset, shm->_roffset);
	if (shm->_w.cacheable) {//flush media & wblk
		//cache_flush(shm->_mblk, shm->_update_size);
		cache_flush(shm->_mblk, eSmfShmBlockSize * 2 + eSmfShmBlockSize + eSmfShmBlockSize);
	}
	return true;
}

bool smf_shm_fifo_read(smf_shm_fifo_t* shm, smf_shm_fifo_frame_t* frm) {
	returnIfErrC(false, !shm);
	// dbgTestPXL("%p,%d,%d,%d", shm, shm->_r.cacheable, shm->_r.nocopy, shm->_r.partread);
	returnIf(false, !smf_shm_fifo_valid(shm, shm->_r.cacheable));
	if (shm->_r.cacheable) {
		cache_invalid(shm->_mblk, shm->_update_size);
	}
	smf_shm_fifo_pack_t* pack = fifo_read_begin(shm);
	returnIf(false, !pack);
	//*frm = *pack;
	frm->size = pack->size;
	frm->flags = pack->flags;
	frm->index = pack->index;
	frm->ext = pack->ext;
	frm->timestamp = pack->timestamp;
	frm->dts = pack->dts;
	frm->gts = pack->gts;
	char* data = pack->data ? (char*)pack->data : ((char*)shm + pack->offset);
	if (shm->_r.cacheable && pack->data) {
		cache_invalid(data, ALIGN64(pack->size));
	}
	if (shm->_r.nocopy) {
		frm->data = data;
	}
	else {
		if (frm->max >= pack->size)
			memcpy(frm->data, data, pack->size);
		else {
			returnIfWarnCS(false, !shm->_r.partread,"[%p]%d<%d,%p",shm,frm->max,pack->size,frm->data);
			memcpy(frm->data, data, frm->max);
			pack->offset += frm->max;
			pack->size -= frm->max;
			frm->size = frm->max;
			return true;
		}
	}
	//
	buff_free(shm, pack->ri, pack->size);
	fifo_read_end(shm);
	//
	if (frm->forward && !shm->_r.no_forward) {
		*(smf_shm_fifo_sinfo_forward_t*)frm->forward = shm->_sf;
	}
	if (frm->backward && !shm->_r.no_backward) {
		shm->_sb = *(smf_shm_fifo_sinfo_backward_t*)frm->backward;
	}
	//
	media_update(shm);
	//
	if (shm->_r.cacheable) {
		cache_flush(shm->_rblk, eSmfShmBlockSize);
	}
	return true;
}

bool smf_shm_fifo_write_media(smf_shm_fifo_t* shm, smf_shm_fifo_media_t* media) {
	// dbgTestPXL("%p,%p", shm, media);
	returnIfErrC(false, !shm);
	bool rst = media_write(shm, media);
	if (shm->_w.cacheable) { cache_flush(shm->_mblk, eSmfShmBlockSize * 4); }
	return rst;
}

smf_shm_fifo_media_t* smf_shm_fifo_get_media32(smf_shm_fifo_t* shm, uint32_t timeout) {
	//dbgTestPXL("%p,%u,%d", shm, timeout, shm->_r.cacheable);
	returnIfErrC(0, !shm);
	returnIfErrC(0, !media_wait(shm, timeout));
	if (shm->_r.cacheable) { cache_invalid(shm->_mblk, eSmfShmBlockSize * 4); }
	returnIfErrC(0, !media_update(shm));
	if (shm->_r.cacheable) { cache_flush(shm->_rblk, eSmfShmBlockSize); }
	return (smf_shm_fifo_media_t*)((char*)shm + shm->_media);
}

bool smf_shm_fifo_read_media(smf_shm_fifo_t* shm, smf_shm_fifo_media_t* media, uint32_t timeout) {
	returnIfErrC(0, !shm);
	returnIfErrC(0, !media_wait(shm, timeout));
	if (shm->_r.cacheable) { cache_invalid(shm->_mblk, eSmfShmBlockSize * 4); }
	media_read(shm, media);
	if (shm->_r.cacheable) { cache_flush(shm->_rblk, eSmfShmBlockSize); }
	return true;
}

bool smf_shm_fifo_set_volume(struct smf_shm_fifo_t* shm, uint16_t volume) {
	return smf_shm_fifo_sinfo_set(shm, SMF_SHM_FIFO_SINFO_volume, volume);
}

bool smf_shm_fifo_set_mute(struct smf_shm_fifo_t* shm, bool mute) {
	return smf_shm_fifo_sinfo_set(shm, SMF_SHM_FIFO_SINFO_mute, mute);
}

bool smf_shm_fifo_sinfo_set(struct smf_shm_fifo_t* shm, enum smf_shm_fifo_sinfo_e keys, uint32_t val){
	returnIfErrC(0, !shm);
	if (keys < SMF_SHM_FIFO_SINFO_backward_flags32) {//forward
		if (shm->_w.cacheable) { cache_invalid(shm->_wblk, eSmfShmBlockSize); }
	}
	else {//backward
		if (shm->_r.cacheable) { cache_invalid(shm->_rblk, eSmfShmBlockSize); }
	}
	//
	switch (keys) {
	case SMF_SHM_FIFO_SINFO_volume:shm->_sf.audio.volume = val; break;
	case SMF_SHM_FIFO_SINFO_mute:shm->_sf.audio.mute = val; break;
	case SMF_SHM_FIFO_SINFO_prompt:shm->_sf.audio.prompt = val; break;
	case SMF_SHM_FIFO_SINFO_fade:shm->_sf.audio.fade = val; break;
	case SMF_SHM_FIFO_SINFO_forward_flags32:shm->_sf.flags = val; break;
	case SMF_SHM_FIFO_SINFO_backward_flags32:shm->_sb.flags = val; break;
	default:
		switch (keys & 0xf0) {
		case SMF_SHM_FIFO_SINFO_forward_flags_set:
		case SMF_SHM_FIFO_SINFO_forward_flags_set + 0x10:
			shm->_sf.flags |= val;
			break;
		case SMF_SHM_FIFO_SINFO_forward_flags_clear:
		case SMF_SHM_FIFO_SINFO_forward_flags_clear + 0x10:
			shm->_sf.flags &= ~val;
			break;
		case SMF_SHM_FIFO_SINFO_backward_flags_set:
		case SMF_SHM_FIFO_SINFO_backward_flags_set + 0x10:
			shm->_sb.flags |= val;
			break;
		case SMF_SHM_FIFO_SINFO_backward_flags_clear:
		case SMF_SHM_FIFO_SINFO_backward_flags_clear + 0x10:
			shm->_sb.flags &= ~val;
			break;
		default:
			dbgErrPPL(keys);
			return false;
		}
	}
	//
	if (keys < SMF_SHM_FIFO_SINFO_backward_flags32) {//forward
		if (shm->_w.cacheable) { cache_flush(shm->_wblk, eSmfShmBlockSize); }
	}
	else {//backward
		if (shm->_r.cacheable) { cache_flush(shm->_rblk, eSmfShmBlockSize); }
	}
	return true;
}

bool smf_shm_fifo_sinfo_get(struct smf_shm_fifo_t* shm, enum smf_shm_fifo_sinfo_e keys, uint32_t* val) {
	returnIfErrC(0, !shm);
	returnIfErrC(0, !keys);
	returnIfErrC(0, !val);
	if (keys < SMF_SHM_FIFO_SINFO_backward_flags32) {//forward
		if (shm->_r.cacheable) { cache_invalid(shm->_wblk, eSmfShmBlockSize); }
	}
	else {//backward
		if (shm->_w.cacheable) { cache_invalid(shm->_rblk, eSmfShmBlockSize); }
	}
	//
	switch (keys) {
	case SMF_SHM_FIFO_SINFO_volume:*val = shm->_sf.audio.volume; break;
	case SMF_SHM_FIFO_SINFO_mute:*val = shm->_sf.audio.mute; break;
	case SMF_SHM_FIFO_SINFO_prompt:*val = shm->_sf.audio.prompt; break;
	case SMF_SHM_FIFO_SINFO_fade:*val = shm->_sf.audio.fade; break;
	case SMF_SHM_FIFO_SINFO_forward_flags32:*val = shm->_sf.flags; break;
	case SMF_SHM_FIFO_SINFO_backward_flags32:*val = shm->_sb.flags; break;
	default:
		dbgErrPPL(keys);
		return false;
	}
	return true;
}

static bool media_write(smf_shm_fifo_t* shm, smf_shm_fifo_media_t* media) {
	returnIf(false, !media);
	returnIf(false, !shm);
	returnIf(false, !shm->_media);
	returnIfErrC(false, media->_size < sizeof(smf_shm_fifo_media_t));
	returnIfErrC(false, !media->_codec);
	//dbgTestPXL("%p,%p", shm, media);
	//update media
	smf_shm_fifo_media32_t* dst = get_media(shm);//(smf_shm_fifo_media32_t*)((char*)shm + shm->_media);
	//dst->_vtable = 0;
	dst->_size = media->_size;
	dst->_codec = media->_codec;
	dst->_mflags = media->_mflags;
	dst->_extraSize = 0;// media->_extraSize;
	dst->_extraData = 0;// (uint32_t)media->_extraData;
	memcpy((char*)dst + sizeof(smf_shm_fifo_media32_t)
		, (char*)media + sizeof(smf_shm_fifo_media_t)
		, media->_size - sizeof(smf_shm_fifo_media_t));

	shm->_mextraData = (shm->_media + dst->_size + 3) >> 2 << 2;
	shm->_mextraData_max = shm->_media + sizeof(shm->_mblk) - shm->_mextraData;
	dbgTestPXL("%p/%p,%s(%u,%u)(%u,%u)", shm, media, &media->_codec, shm->_media, dst->_size, shm->_mextraData, shm->_mextraData_max);
	//update extra data
	if (shm->_mextraData && media->_extraData && media->_extraSize) {
		if (media->_extraSize <= shm->_mextraData_max) {
			memcpy(get_media_extra(shm), (void*)media->_extraData, media->_extraSize);
			dst->_extraData = (uint32_t)get_media_extra(shm);//shm->_r.addr + shm->_mextraData;
			dst->_extraSize = media->_extraSize;
		}
		else {
			dbgWarnPXL("extraDataSize:%d>%d", media->_extraSize, shm->_mextraData_max);
		}
	}

	shm->_w.midx++;
	shm->_up.mmagic = eSmfShmMagic;
	// dbgTestPXL("%p,%p,%d,%s,%p,%08x", shm, dst, dst->_size, &dst->_codec, &shm->_up.mmagic, shm->_up.mmagic);
	return true;
}

static bool media_update(smf_shm_fifo_t* shm) {
	returnIf(false, !shm);
	returnIf(false, !shm->_media);
	if (shm->_r.midx != shm->_w.midx) {
		shm->_r.midx++;
		smf_media_update_vtable(get_media(shm));
	}
	return true;
}

static bool media_read(smf_shm_fifo_t* shm, smf_shm_fifo_media_t* media) {
	if (media_update(shm)) {
		smf_shm_fifo_media32_t* src = get_media(shm);//(smf_shm_fifo_media32_t*)((char*)shm + shm->_media);
		//media->_vtable = 0;
		media->_size = src->_size;
		media->_codec = src->_codec;
		media->_mflags = src->_mflags;
		media->_extraSize = src->_extraSize;
		media->_extraData = (void*)src->_extraData;
		memcpy((char*)media + sizeof(smf_shm_fifo_media_t), (char*)src + sizeof(smf_shm_fifo_media32_t), src->_size - sizeof(smf_shm_fifo_media32_t));
		// dbgTestPXL("%p,%p,%d,%s", shm, src, src->_size, &src->_codec);
	}
	return true;
}

static bool media_wait(smf_shm_fifo_t* shm, uint32_t timeout) {
	//dbgTestPXL("%p,%p,%d,%p,%p", shm, shm->_media, shm->_r.cacheable, &shm->_up.mmagic, shm->_up.mmagic);
	returnIf(false, !shm);
	returnIf(false, !shm->_media);
	uint32_t tp = 0;
	do {
		if (shm->_r.cacheable) cache_invalid(shm->_wblk, eSmfShmBlockSize);
		if (shm->_up.mmagic == eSmfShmMagic)
			return true;
		if(timeout)
			sleep_for(5);
		tp += 5;
	} while (tp < timeout);
	return false;
}

static uint32_t buff_alloc(smf_shm_fifo_t* shm, uint32_t size, uint64_t* pi) {
	if (shm->_align) {
		size = (size + (shm->_align - 1)) / shm->_align * shm->_align;
	}
	uint32_t data = shm->_data;
	uint32_t max = shm->_data_size;
	uint64_t wi = shm->_w.bidx;
	uint64_t ri = shm->_r.bidx;
	bool isring = shm->_w.isring;
	//
	if (size > max) {
		return 0;
	}
	uint32_t wii = wi % max;
	if (max - wii < size) {//monoblock
		wi += (max - wii);
		wii = 0;
	}
	if (!isring) {
		if (wi + size > ri + max) {
			return 0;
		}
	}
	shm->_w.bidx = wi + size;
	if (pi)*pi = wi;
	return data + wii;
}

static void buff_refund(smf_shm_fifo_t* shm, uint32_t size) {
	if (shm->_align) {
		size = (size + (shm->_align - 1)) / shm->_align * shm->_align;
	}
	shm->_w.bidx -= size;
}

static void buff_free(smf_shm_fifo_t* shm, uint64_t ri, uint32_t size) {
	if (shm->_align) {
		size = (size + (shm->_align - 1)) / shm->_align * shm->_align;
	}
	shm->_r.bidx = (shm->_r.nocopy && ri) ? ri : (ri + size);
}

static smf_shm_fifo_pack_t* fifo_write_begin(smf_shm_fifo_t* shm) {
	smf_shm_fifo_pack_t* fifo = get_fifo(shm);//(smf_shm_fifo_pack_t*)((char*)shm + shm->_fifo);
	uint32_t max = shm->_fifo_max;
	uint32_t wi = shm->_w.fidx;
	uint32_t ri = shm->_r.fidx;
	bool isring = shm->_w.isring;
	return (!isring && wi == ri + max) ? (smf_shm_fifo_pack_t*)0 : (fifo + wi % max);
}

static void fifo_write_end(smf_shm_fifo_t* shm) {
	shm->_w.fidx++;
}

static smf_shm_fifo_pack_t* fifo_read_begin(smf_shm_fifo_t* shm) {
	smf_shm_fifo_pack_t* fifo = get_fifo(shm); //(smf_shm_fifo_pack_t*)((char*)shm + shm->_fifo);
	uint32_t max = shm->_fifo_max;
	uint32_t wi = shm->_w.fidx;
	uint32_t ri = shm->_r.fidx;
	//bool isring = shm->_w.isring;
	return (wi == ri) ? (smf_shm_fifo_pack_t*)0 : (fifo + ri % max);
}

static void fifo_read_end(smf_shm_fifo_t* shm) {
	shm->_r.fidx++;
}

static void fifo_read_sync(smf_shm_fifo_t* shm, int offset) {
	shm->_r.fidx = shm->_w.fidx - offset;
}

static inline smf_shm_fifo_media32_t* get_media(smf_shm_fifo_t* shm) {
	return (smf_shm_fifo_media32_t*)((char*)shm + shm->_media);
}
static inline char* get_media_extra(smf_shm_fifo_t* shm) {
	return (char*)shm + shm->_mextraData;
}
static inline smf_shm_fifo_pack_t* get_fifo(smf_shm_fifo_t* shm) {
	return (smf_shm_fifo_pack_t*)((char*)shm + shm->_fifo);
}
// static inline char* get_data(smf_shm_fifo_t* shm) {
// 	return (char*)shm + shm->_data;
// }
#if 0
static void* memcpy(void* dst_, const void* src_, unsigned size) {
	char* dst = (char*)dst_;
	char* src = (char*)src_;
	int c = size;
	while (c-- > 0) {
		*dst++ = *src++;
	}
	return dst_;
}

static void* memset(void* dst_, int chr, int size) {
	char* dst = (char*)dst_;
	int c = size;
	while (c-- > 0) {
		*dst++ = chr;
	}
	return dst_;
}
#endif