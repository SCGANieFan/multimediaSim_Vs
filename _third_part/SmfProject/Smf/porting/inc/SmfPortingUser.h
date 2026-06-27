
#pragma once
#include <stdint.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef enum {
	MEDIA_DEV_INTCODEC_RX = 0,
	MEDIA_DEV_INTCODEC_TX,
	MEDIA_DEV_INTCODEC_RX2,
	MEDIA_DEV_INTCODEC_TX2,
	MEDIA_DEV_INTCODEC_RX3,
	MEDIA_DEV_INTCODEC_TX3,
	MEDIA_DEV_BTPCM_RX,
	MEDIA_DEV_BTPCM_TX,
	MEDIA_DEV_I2S0_RX,
	MEDIA_DEV_I2S0_TX,
	MEDIA_DEV_I2S1_RX,
	MEDIA_DEV_I2S1_TX,
	MEDIA_DEV_TDM_I2S0_RX,
	MEDIA_DEV_TDM_I2S0_TX,
	MEDIA_DEV_TDM_I2S1_RX,
	MEDIA_DEV_TDM_I2S1_TX,
	MEDIA_DEV_MAX,
}smf_media_device_e;

typedef enum {
	MEDIA_DEV_STATE_STOP = 0,
	MEDIA_DEV_STATE_START,
	MEDIA_DEV_STATE_MAX,
}smf_media_device_state_e;

void smf_device_notify(smf_media_device_e dev, smf_media_device_state_e state);

void smf_device_stop_success_notify(smf_media_device_e dev);

#ifdef __cplusplus
}
#endif