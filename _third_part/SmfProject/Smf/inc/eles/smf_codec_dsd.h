#pragma once
#ifndef __SMF_CODEC_DSD_H__
#define __SMF_CODEC_DSD_H__

#include "smf_api.h"
#include "smf_media_def.h"
typedef struct smf_media_dsd_def_t {
	uint32_t rev;
}smf_media_dsd_def_t;

typedef struct {
	
	smf_media_def_t head;
	smf_media_audio_def_t audio;
	smf_media_dsd_def_t dsd;
}smf_media_dsd_t;

typedef struct smf_dsd_enc_open_param_t {
	smf_media_dsd_t media;
}smf_dsd_enc_open_param_t;

typedef struct smf_dsd_dec_open_param_t {
	smf_media_dsd_t media;
}smf_dsd_dec_open_param_t;

/**
 * register dsd encoder
 */
EXTERNC void smf_dsd_encoder_register(void);

/**
 * register dsd decoder
 */
EXTERNC void smf_dsd_decoder_register(void);
#endif
