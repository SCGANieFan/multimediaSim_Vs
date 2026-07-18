#pragma once
#include <stdint.h>
#include <stdbool.h>
#ifndef EXTERNC
#ifndef __cplusplus
#define EXTERNC
#else
#define EXTERNC extern "C"
#endif
#endif

typedef struct {
	uint32_t _vtable;
	uint32_t _size;
	uint64_t _codec;
	uint32_t _mflags;
	uint32_t _extraSize;
	void* _extraData;
}smf_shm_fifo_media_t;

typedef struct smf_shm_fifo_frame_t {
	uint32_t size;
	uint32_t max;
	uint16_t flags;
	uint16_t ext;
	uint32_t index;
	uint32_t timestamp;
	void* data;
	void* forward;
	void* backward;
	uint32_t dts;
	uint32_t gts;
}smf_shm_fifo_frame_t;

typedef struct smf_shm_fifo_mem_t {
	void* buff;
	uint32_t size;
	uint64_t priv[2];
}smf_shm_fifo_mem_t;

struct smf_shm_fifo_t;
EXTERNC bool smf_shm_fifo_initialize_all(struct smf_shm_fifo_t* shm, int size, int data_size, int fifo_max, bool cacheable, bool nomedia, bool reset, uint8_t align);

EXTERNC uint32_t smf_shm_fifo_get_size(int data_size, int fifo_max);

EXTERNC bool smf_shm_fifo_valid(struct smf_shm_fifo_t* shm, bool cacheable);
EXTERNC bool smf_shm_fifo_write_valid(struct smf_shm_fifo_t* shm);
EXTERNC bool smf_shm_fifo_read_valid(struct smf_shm_fifo_t* shm);
EXTERNC bool smf_shm_fifo_media_valid(struct smf_shm_fifo_t* shm);

EXTERNC bool smf_shm_fifo_initialize(struct smf_shm_fifo_t* shm, int size, int data_size, int fifo_max, bool cacheable, bool nomedia);
EXTERNC bool smf_shm_fifo_init_write(struct smf_shm_fifo_t* shm, bool cacheable, bool nocopy, bool isring);
EXTERNC bool smf_shm_fifo_init_read(struct smf_shm_fifo_t* shm, bool cacheable, bool nocopy, bool partread);
EXTERNC bool smf_shm_fifo_deinit(struct smf_shm_fifo_t* shm);

EXTERNC bool smf_shm_fifo_write(struct smf_shm_fifo_t* shm, smf_shm_fifo_frame_t* frm);
EXTERNC bool smf_shm_fifo_read(struct smf_shm_fifo_t* shm, smf_shm_fifo_frame_t* frm);
EXTERNC bool smf_shm_fifo_sync(struct smf_shm_fifo_t* shm, int offset);

EXTERNC bool smf_shm_fifo_send_prepare(struct smf_shm_fifo_t* shm, smf_shm_fifo_mem_t* mem, uint32_t size);
EXTERNC bool smf_shm_fifo_send(struct smf_shm_fifo_t* shm, smf_shm_fifo_mem_t* mem, smf_shm_fifo_frame_t* frm);

EXTERNC bool smf_shm_fifo_write_media(struct smf_shm_fifo_t* shm, smf_shm_fifo_media_t* media);
EXTERNC bool smf_shm_fifo_read_media(struct smf_shm_fifo_t* shm, smf_shm_fifo_media_t* media, uint32_t timeout);
EXTERNC smf_shm_fifo_media_t* smf_shm_fifo_get_media32(struct smf_shm_fifo_t* shm, uint32_t timeout);

EXTERNC uint32_t smf_shm_fifo_get_max(struct smf_shm_fifo_t* shm);
EXTERNC uint32_t smf_shm_fifo_get_buff_max(struct smf_shm_fifo_t* shm);
EXTERNC uint32_t smf_shm_fifo_get_used(struct smf_shm_fifo_t* shm, bool isread);
EXTERNC uint32_t smf_shm_fifo_get_left(struct smf_shm_fifo_t* shm, bool isread);
EXTERNC uint32_t smf_shm_fifo_get_buff_used(struct smf_shm_fifo_t* shm, bool isread);
EXTERNC uint32_t smf_shm_fifo_get_buff_left(struct smf_shm_fifo_t* shm, bool isread);


enum smf_shm_fifo_sinfo_e{
	SMF_SHM_FIFO_SINFO_forward_flags32 = 0x00,
	SMF_SHM_FIFO_SINFO_volume,
	SMF_SHM_FIFO_SINFO_mute,
	SMF_SHM_FIFO_SINFO_prompt,
	SMF_SHM_FIFO_SINFO_fade,
	SMF_SHM_FIFO_SINFO_forward_flags_set = 0x20,
	SMF_SHM_FIFO_SINFO_forward_flags_clear = 0x40,

	SMF_SHM_FIFO_SINFO_backward_flags32 = 0x60,
	SMF_SHM_FIFO_SINFO_backward_flags_set = 0x80,
	SMF_SHM_FIFO_SINFO_backward_flags_clear = 0xa0,
};

EXTERNC bool smf_shm_fifo_sinfo_set(struct smf_shm_fifo_t* shm, enum smf_shm_fifo_sinfo_e keys, uint32_t val);
EXTERNC bool smf_shm_fifo_sinfo_get(struct smf_shm_fifo_t* shm, enum smf_shm_fifo_sinfo_e keys, uint32_t* val);

EXTERNC bool smf_shm_fifo_set_volume(struct smf_shm_fifo_t* shm, uint16_t volume);
EXTERNC bool smf_shm_fifo_set_mute(struct smf_shm_fifo_t* shm, bool mute);

enum smf_shm_fifo_set_e {
	SMF_SHM_FIFO_SET_begin = 0,
	SMF_SHM_FIFO_SET_no_forward,
	SMF_SHM_FIFO_SET_no_backward,
	SMF_SHM_FIFO_SET_max,
};
EXTERNC bool smf_shm_fifo_set(struct smf_shm_fifo_t* shm, enum smf_shm_fifo_set_e key, void* value);
EXTERNC bool smf_shm_fifo_set_write(struct smf_shm_fifo_t* shm, enum smf_shm_fifo_set_e key, void* value);
EXTERNC bool smf_shm_fifo_set_read(struct smf_shm_fifo_t* shm, enum smf_shm_fifo_set_e key, void* value);
