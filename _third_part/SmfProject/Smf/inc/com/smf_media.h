#pragma once
#ifndef __SMF_MEDIA_H__
#define __SMF_MEDIA_H__
#include "smf_media_def.h"
#include "smf_codec_aac.h"
#include "smf_codec_alac.h"
#include "smf_codec_amr.h"
#include "smf_codec_ape.h"
#include "smf_codec_cvsd.h"
#include "smf_codec_codec2.h"
#include "smf_codec_flac.h"
#include "smf_codec_g711.h"
#include "smf_codec_g722.h"
#include "smf_codec_gif.h"
#include "smf_codec_h264.h"
#include "smf_codec_h265.h"
#include "smf_codec_jpeg.h"
#include "smf_codec_lc3.h"
#include "smf_codec_ldac.h"
#include "smf_codec_mp3.h"
#include "smf_codec_opus.h"
#include "smf_codec_pcm.h"
#include "smf_codec_png.h"
#include "smf_codec_rgb.h"
#include "smf_codec_sbc.h"
#include "smf_codec_silk.h"
#include "smf_codec_ssc.h"
#include "smf_codec_vorbis.h"
#include "smf_codec_yuv.h"
//
typedef struct smf_media_t {
	smf_media_def_t head;
	union {
		struct {
			smf_media_audio_def_t audio;
			union {
				smf_media_pcm_def_t pcm;
				smf_media_g711_def_t g711;
				smf_media_g722_def_t g722;
				
				smf_media_flac_def_t flac;
				smf_media_ape_def_t ape;
				smf_media_alac_def_t alac;
				
				smf_media_aac_def_t aac;
				smf_media_mp3_def_t mp3;
				smf_media_opus_def_t opus;
				smf_media_vorbis_def_t vorbis;
				
				smf_media_lc3_def_t lc3;
				smf_media_sbc_def_t sbc;
				//smf_media_msbc_def_t msbc;
				//smf_media_gsbc_def_t gsbc;
				smf_media_cvsd_def_t cvsd;
				smf_media_ldac_def_t ldac;
				smf_media_ssc_def_t ssc;
				
				smf_media_silk_def_t silk;
				smf_media_amr_def_t amr;
				smf_media_codec2_def_t codec2;
			};
		};
		struct {
			smf_media_video_def_t video;
			union {
				smf_media_rgb_def_t rgb;
				smf_media_yuv_def_t yuv;
				smf_media_jpeg_def_t jpeg;
				smf_media_gif_def_t gif;
				smf_media_png_def_t png;
				smf_media_h264_def_t h264;
				smf_media_h264_def_t h265;
			};
		};
	};
}smf_media_t;

enum smf_codec_e {
	SMF_CODEC_NULL,
	SMF_CODEC_PCM = FCC3('p', 'c', 'm'),
	SMF_CODEC_AAC = FCC3('a', 'a', 'c'),
	SMF_CODEC_AMR = FCC3('a', 'm', 'r'),
	SMF_CODEC_AMRnb = FCC5('a', 'm', 'r', 'w', 'b'),
	SMF_CODEC_AMRwb = FCC5('a', 'm', 'r', 'n', 'b'),
	SMF_CODEC_SBC = FCC3('s', 'b', 'c'),
	SMF_CODEC_MSBC = FCC4('m', 's', 'b', 'c'),
	SMF_CODEC_CVSD = FCC4('c', 'v', 's', 'd'),
	SMF_CODEC_LC3 = FCC3('l', 'c', '3'),
	SMF_CODEC_LDAC = FCC4('l', 'd', 'a', 'c'),
	SMF_CODEC_LHDC = FCC4('l', 'h', 'a', 'c'),
	SMF_CODEC_OPUS = FCC4('o', 'p', 'u', 's'),
	SMF_CODEC_SILK = FCC4('s', 'i', 'l', 'k'),
	SMF_CODEC_CODEC2 = FCC6('c', 'o', 'd', 'e', 'c', '2'),
	SMF_CODEC_MP3 = FCC3('m', 'p', '3'),
	SMF_CODEC_VORBIS = FCC6('v', 'o', 'r', 'b', 'i', 's'),

	SMF_CODEC_APE = FCC3('a', 'p', 'e'),
	SMF_CODEC_FLAC = FCC4('f', 'l', 'a', 'c'),
	SMF_CODEC_ALAC = FCC4('a', 'l', 'a', 'c'),

	SMF_CODEC_YUV = FCC3('y', 'u', 'v'),
	SMF_CODEC_RGB = FCC3('r', 'g', 'b'),
	SMF_CODEC_H264 = FCC4('h', '2', '6', '4'),
	SMF_CODEC_H265 = FCC4('h', '2', '6', '5'),
	SMF_CODEC_JPEG = FCC4('j', 'p', 'e', 'g'),
	SMF_CODEC_GIF = FCC3('g', 'i', 'g'),
	SMF_CODEC_PNG = FCC3('p', 'n', 'g'),
};

typedef struct {
	uint64_t codec;//smf_stream_codec_e
	uint32_t bitrate;
	union {
		smf_media_pcm_def_t pcm;
		smf_media_g711_def_t g711;
		smf_media_g722_def_t g722;

		smf_media_flac_def_t flac;
		smf_media_ape_def_t ape;
		smf_media_alac_def_t alac;

		smf_media_aac_def_t aac;
		smf_media_mp3_def_t mp3;
		smf_media_opus_def_t opus;
		smf_media_vorbis_def_t vorbis;

		smf_media_lc3_def_t lc3;
		smf_media_sbc_def_t sbc;
		//smf_media_msbc_def_t msbc;
		//smf_media_gsbc_def_t gsbc;
		smf_media_cvsd_def_t cvsd;
		smf_media_ldac_def_t ldac;
		smf_media_ssc_def_t ssc;

		smf_media_silk_def_t silk;
		smf_media_amr_def_t amr;
		smf_media_codec2_def_t codec2;
	};
}smf_audio_encode_t;
#endif

