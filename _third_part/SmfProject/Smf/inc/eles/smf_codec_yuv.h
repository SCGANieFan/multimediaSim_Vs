#pragma once
#ifndef __SMF_CODEC_YUV_H__
#define __SMF_CODEC_YUV_H__

#include "smf_api.h"
#include "smf_media_def.h"
enum yuv_format_e {
	YUV_FORMAT_begin = 0,

	vyuy,
	uyvy,
	yuyv,
	yvyu,
	yuy2 = yuyv,

	i420 = 10,//y4-u1-v1
	yv12,//y4-v1-u1
	i422,//y4-u2-v2
	yv16,//y4-v2-u2
	i444,//y4-u4-v4
	yv24,//y4-v4-u4

	nv12 = 20,//y4-uv1
	nv21,//y4-vu1
	nv61,//y4-uv2
	nv16,//y4-vu2
	nv24,//y4-uv4
	nv42,//y4-vu4

	i420p16 = 30,	//y4-u1-v1
	i420p16s,	//y4-u1-v1
	i422p16,	//y4-u2-v2
	i422p16s,	//y4-u2-v2
	i444p16,	//y4-u4-v4
	i444p16s,	//y4-u4-v4

	YUV_FORMAT_end,
};

typedef struct {
	uint8_t rev;
}smf_media_yuv_def_t;

typedef struct {
	smf_media_def_t head;
	smf_media_video_def_t video;
	smf_media_video_raw_def_t raw;
	smf_media_yuv_def_t yuv;
}smf_media_yuv_t;

typedef struct smf_yuv_enc_open_param_t {
	smf_media_yuv_t media;
}smf_yuv_enc_open_param_t;

typedef struct smf_yuv_dec_open_param_t {
	smf_media_yuv_t* media;
}smf_yuv_dec_open_param_t;

/**
 * register yuv encoder
 */
EXTERNC void smf_yuv_encoder_register(void);

/**
 * register yuv decoder
 */
EXTERNC void smf_yuv_decoder_register(void);
#endif
