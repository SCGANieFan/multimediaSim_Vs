#pragma once
#ifndef __SMF_FRAME_H__
#define __SMF_FRAME_H__
#include <stdbool.h>
#include <stdint.h>
#include <string.h>
#include "smf_media_def.h"

typedef struct smf_shared_info_t {
	struct {
		uint64_t timestamp;
		union {
			uint32_t av;
			struct {
				uint32_t volume : 24;
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
				uint16_t rev16 : 16;
				uint8_t rev8 : 8;
				uint8_t rev4 : 4;
				uint8_t rev2 : 2;
				bool is_audio : 1;
				bool is_video : 1;
			};
		};
	}forward;
	struct {
		uint32_t index;
		uint32_t timestamp;
		union {
			uint32_t av;
			struct {
				uint32_t volume : 24;
				bool mute : 1;
			}audio;
			struct {
				uint32_t video;
			}video;
		};
		union {
			uint32_t flags;
			struct {
				uint16_t rev16 : 16;
				uint8_t rev8 : 8;
				uint8_t rev4 : 4;
				uint8_t rev2 : 2;
				bool rev0 : 1;
				bool rev1 : 1;
			};
		};
	}backward;
}smf_shared_info_t;

/**
 * @brief the frame of data.
 * @details the frame buffer description
	     |<----------------max------------------------>|
	buff:|********|**************************|*********|
	              |<---------size----------->|
	-----------offset----------------------------------
*/
typedef struct smf_frame_t {
	void* buff;
	uint32_t max;
	int32_t offset;
	int32_t size;
	uint16_t flags;//smf_frame_flags_e
	uint16_t ext;//customer
	smf_media_def_t* media;
	uint32_t index;
	uint32_t timestamp;//pts,play time stamp
	struct smf_frame_t* frame;
	void* port;
	void* priv;
	void* pool;
	smf_shared_info_t* sinfo;
	uint32_t dts;//decode/device/bt time stamp
	uint32_t gts;//globel/local time stamp
}smf_frame_t;

typedef bool (*CbFrame)(smf_frame_t**frm,void*priv);
enum smf_frame_flags_e {
	SMF_FRAME_PACKET_MASK = 0xff,
	SMF_FRAME_IS_FAIL = 1u << 0,//0x01 error packet
	SMF_FRAME_IS_EOS = 1u << 1,//0x02 end of scan
	SMF_FRAME_IS_FIRST = 1u << 2,//0x04 first of scan
	SMF_FRAME_IS_KEY = 1u << 3,
	SMF_FRAME_IS_EXTRA = 1u << 4,//0x10
	SMF_FRAME_IS_MEDIA = 1u << 5,//0x20	
	SMF_FRAME_INCOMPLETE = 1u << 6,//0x40
	SMF_FRAME_IS_MUTE = 1u << 7,//0x80
	//
	SMF_FRAME_CACHED = 1u << 8,
	SMF_FRAME_SHARED = 1u << 9,
	SMF_FRAME_PACKED = 1u << 10,
	SMF_FRAME_AUTO_FREE = 1u << 11,
	SMF_FRAME_PROCESS_MULTIFRAMES = 1u << 12,
	//
	SMF_FRAME_IS_PLC = 1u << 13,
	SMF_FRAME_IS_RESERVE = 1u << 14,
	SMF_FRAME_CUST0 = 1u << 15,
	//
	SMF_FRAMR_RESULT = SMF_FRAME_IS_EOS | SMF_FRAME_IS_FAIL,
};

enum smf_frame_flags_ext_e {
	SMF_PACKET_FLAGS_FAIL = SMF_FRAME_IS_FAIL,// 1u << 0,//error packet
	SMF_PACKET_FLAGS_EOS = SMF_FRAME_IS_EOS,//1u << 1,//end of scan
	SMF_PACKET_FLAGS_FIRST = SMF_FRAME_IS_FIRST,//1u << 2,//first of scan
	SMF_PACKET_FLAGS_EXTRA = SMF_FRAME_IS_EXTRA,//1u << 4,//extra data package
	SMF_PACKET_FLAGS_MEDIA = SMF_FRAME_IS_MEDIA,//1u << 5,//shm_media_t
	SMF_PACKET_INCOMPLETE = SMF_FRAME_INCOMPLETE,//1u << 6,
	//SMF_PACKET_INFOMATION = SMF_FRAME_INFOMATION,//1u << 7,
};
//
typedef struct {
	uint8_t seqNO : 4;
	uint8_t crc : 4;
	uint8_t flags;
	uint16_t payload_size;
}smf_packet_t;
#endif
