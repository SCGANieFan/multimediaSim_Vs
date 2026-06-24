#pragma once
#ifndef __SMF_PROCESS_PLC_H__
#define __SMF_PROCESS_PLC_H__

#include "smf_api.h"

enum smf_plc_channel_e {
	SMF_PLC_CHANNEL_SELECT_LCHNL = 0,
	SMF_PLC_CHANNEL_SELECT_RCHNL = 1,
	SMF_PLC_CHANNEL_SELECT_LRCHNL = 2,
};

enum PLC_CODEC_TYPE {
	PLC_CODEC_TYPE_SBC = 0,
	PLC_CODEC_TYPE_AAC = 1,
	PLC_CODEC_TYPE_LC3 = 2,
	PLC_CODEC_TYPE_CVSD = 3,
	PLC_CODEC_TYPE_AMR_NB = 4,
	PLC_CODEC_TYPE_AMR_WB = 5,
};

enum smf_plc_mode_e {
	SMF_PLC_MODE_MUSIC_PLC = 0,
	SMF_PLC_MODE_SBC_PLC,
	SMF_PLC_MODE_OTHER_PLC,//not valid
	SMF_PLC_MODE_PLC_MAX,
};

enum smf_plc_application_e {
	SMF_PLC_APPLICATION_VOICE = 0,
	SMF_PLC_APPLICATION_MUSIC,
	SMF_PLC_APPLICATION_192K,
	SMF_PLC_APPLICATION_LOW_LATENCY,
	SMF_PLC_APPLICATION_LFE,
	SMF_PLC_APPLICATION_AUTO,
	SMF_PLC_APPLICATION_MAX,
};

enum smf_plc_sample_align_mode_e {
	SMF_PLC_SAMPLE_ALIGN_MODE_HIGH_BYTE= 0,
	SMF_PLC_SAMPLE_ALIGN_MODE_LOW_BYTE,
};

typedef struct {
	int16_t validBits;
	int16_t storedBits;
	smf_plc_sample_align_mode_e alignMode;
}smf_plc_sample_store_format_s;

typedef struct smf_plc_open_param_s {
	int rate;
	int channelNum;
	int sampleBits;
	int frameSize;
	int targetchannelNum;
	int targetchannel;
	smf_plc_mode_e mode;
	smf_plc_application_e application;
	union {
		struct {
			enum PLC_CODEC_TYPE type;
			int search_size;
			int match_size;
			int olal;
			int recover;
		}sbcPlcParam;
		struct {
		}musicPlcParam;
	};
	void* other;
}smf_plc_open_param_t;

/**
 * register plc processer
 */
EXTERNC void smf_plc_register(void);
EXTERNC void smf_sbcplc_register(void);

#endif
