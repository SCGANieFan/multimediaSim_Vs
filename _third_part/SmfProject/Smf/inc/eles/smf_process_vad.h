#pragma once
#ifndef __SMF_PROCESS_VAD_H__
#define __SMF_PROCESS_VAD_H__

#include "smf_api.h"
// #include "smf_media_api_def.h"
typedef struct smf_vad_open_param_s {
	int rate;
	int channels;
	int frame_ms;
	int vad_mode;
	int sample_bits;
	void* other;
}smf_vad_open_param_t;

/**
 * register vad processer
 */
EXTERNC void smf_vad_register(void);

#endif