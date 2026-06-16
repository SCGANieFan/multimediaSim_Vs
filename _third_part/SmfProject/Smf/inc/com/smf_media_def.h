#pragma once
#ifndef __SMF_MEDIA_DEF_H__
#define __SMF_MEDIA_DEF_H__
#include <stdbool.h>
#include <stdint.h>
#include <string.h>
//
typedef struct smf_media_def_t {
	uint32_t* vtable;
	uint32_t _size;
	uint64_t _codec;
	uint32_t _mflags;
	uint32_t _extraSize;
	uint8_t* _extraData;
	uint32_t _bitrate;
}smf_media_def_t;
//
typedef struct smf_media32_def_t {
	uint32_t _size;
	uint32_t _codec;//not used uint64_t
	uint32_t _codec_high;//not used uint64_t
	uint32_t _mflags;
	uint32_t _extraSize;
	uint8_t* _extraData;
	uint32_t _bitrate;
}smf_media32_def_t;
//
typedef struct smf_media_audio_def_t {
	uint32_t _rate; 
	uint8_t _channels; 
	uint8_t _rev; 
	uint16_t _frameSamples;
}smf_media_audio_def_t;
//
typedef struct smf_media_video_def_t {
	uint16_t _width;
	uint16_t _height;
	uint16_t _frameRate;
	uint16_t _frameRateDenum;
	uint16_t _roiX, _roiY, _roiW, _roiH;//Region of Interest
}smf_media_video_def_t;
typedef struct {
	uint8_t _format;//format_e
	uint8_t _pixelBits;//8,16,24,32
}smf_media_video_raw_def_t;
//
typedef enum {
	SMF_MEDIA_IS_Audio = 1u << 0,   //IS_Audio
	SMF_MEDIA_IS_Video = 1u << 1,   //IS_Video
	SMF_MEDIA_IS_Stream = 1u << 2,  //IS_Stream
	SMF_MEDIA_IS_Other = 1u << 6,   //IS_Other
	SMF_MEDIA_IS_Error = 1u << 20,	//IS_Error
	SMF_MEDIA_IS_Changed = 1u << 18,//IS_Changed
}smf_media_mflags_e;
//
typedef struct {
	uint64_t codec;//smf_stream_codec_e
	uint32_t bitrate;
	uint32_t params[13];
}smf_media_audio_encode_t;
//
typedef struct smf_pcm_pack_t {
	void* data;
	uint32_t size;
	uint32_t sample_rate;
	uint8_t channels;
	uint8_t sample_bits;
	uint8_t sample_width;
	uint8_t flags;
}smf_pcm_pack_t;
//
typedef struct smf_pcm_format_t {
	uint32_t sample_rate;
	uint8_t channels;
	uint8_t sample_bits;
	uint8_t sample_width;
	union {
		uint8_t channel_select : 4;//[0]:disable,1~8=>0~7chns
		bool isfloat : 1;
		bool lrmix : 1;
	};
}smf_pcm_format_t;
#endif

