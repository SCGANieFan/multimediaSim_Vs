#pragma once
#include <stdint.h>
#include <stdbool.h>

#define eSmfShmBlockSize 64
#define eSmfShmMagic 0x55aa5a5a

typedef struct{
	uint32_t _vtable;
	uint32_t _size;
	uint64_t _codec;
	uint32_t _mflags;
	uint32_t _extraSize;
	uint32_t _extraData;
}smf_shm_fifo_media32_t;

typedef struct {
	uint64_t timestamp;
	union {
		uint32_t av;
		struct {
			uint16_t volume;
			bool mute : 1;
			bool prompt : 1;
			bool fade : 1;
		}audio;
		struct {
			uint32_t flags;
		}video;
	};
	union {
		uint32_t flags;
		struct {
			bool rev0 : 1;
		};
	};
}smf_shm_fifo_sinfo_forward_t;

typedef struct {
	uint32_t index;
	uint32_t timestamp;
	union {
		uint32_t av;
		struct {
			uint16_t volume;
			bool mute : 1;
		}audio;
		struct {
			uint32_t video;
		}video;
	};
	union {
		uint32_t flags;
		struct {
			bool done : 1;
		};
	};
}smf_shm_fifo_sinfo_backward_t;

typedef struct smf_shm_fifo_pack_t {
	uint32_t size;
	uint16_t flags;
	uint16_t ext;
	uint32_t index;
	uint32_t timestamp;//pts
	uint32_t data;
	uint32_t offset;
	uint64_t ri;
	uint32_t dts;
	uint32_t gts;
}smf_shm_fifo_pack_t;

typedef struct smf_shm_fifo_block_t {
	uint32_t magic;
	uint32_t fidx;
	uint64_t bidx;
	//
	uint16_t midx;//update media;
	union {
		uint16_t flags;
		struct {
			bool nocopy : 1;
			bool cacheable : 1;
			bool isring : 1;
			bool partread: 1;
			bool no_forward : 1;
			bool no_backward : 1;
		};
	};
	uint32_t rev;
	uint64_t addr;
}smf_shm_fifo_block_t;

typedef struct smf_shm_fifo_update_t {
	uint32_t mmagic;
}smf_shm_fifo_update_t;

typedef struct smf_shm_fifo_t {
	union {///common block, align eSmfShmBlockSize
		char _cblk[eSmfShmBlockSize];
		struct {
			uint32_t _magic;
			uint32_t _total;
			uint32_t _fifo_max;
			uint32_t _update_size;
			uint32_t _data_size;
			uint32_t _fifo_size;
			uint32_t _data;// _offset;//char* offset
			uint32_t _fifo;// _offset;//smf_shm_fifo_pack_t* offset
			uint32_t _media;// _offset;//smf_media_def_t* offset
			uint32_t _mextraData;// _offset;//void* offset
			uint8_t _media_max;
			uint8_t _mextraData_max;
			uint8_t _align;
			uint8_t _other;
		};
	};
	union {///read block, align eSmfShmBlockSize
		char _rblk[eSmfShmBlockSize];
		struct {
			smf_shm_fifo_block_t _r;
			smf_shm_fifo_sinfo_backward_t _sb;
			//
			char _rrevs[eSmfShmBlockSize - 4 - sizeof(smf_shm_fifo_block_t) - sizeof(smf_shm_fifo_sinfo_backward_t)];
			uint32_t _mvtable;//media vtable
		};
	};
	union {///media block, align eSmfShmBlockSize
		char _mblk[eSmfShmBlockSize * 3];
	};
	union {///write block, align eSmfShmBlockSize
		char _wblk[eSmfShmBlockSize];
		struct {
			smf_shm_fifo_block_t _w;
			smf_shm_fifo_sinfo_forward_t _sf;
			smf_shm_fifo_update_t _up;
			//
			char _wrevs[eSmfShmBlockSize - sizeof(smf_shm_fifo_block_t) - sizeof(smf_shm_fifo_update_t) - sizeof(smf_shm_fifo_sinfo_forward_t)];
		};
	};
}smf_shm_fifo_t;

