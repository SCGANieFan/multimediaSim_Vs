/***************************************************************************
 *
 * Copyright 2015-2021 BES.
 * All rights reserved. All unpublished rights reserved.
 *
 * No part of this work may be used or reproduced in any form or by any
 * means, or stored in a database or retrieval system, without prior written
 * permission of BES.
 *
 * Use of this work is governed by a license granted by BES.
 * This work contains confidential and proprietary information of
 * BES. which is protected by copyright, trade secret,
 * trademark and other intellectual property rights.
 *
 ****************************************************************************/
#pragma once
#ifndef __SMF_DEVICE_H__
#define __SMF_DEVICE_H__
#include <stdint.h>
#include <stdbool.h>

namespace smf {
	typedef enum MediaDevice_e : uint8_t {
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
	}MediaDevice_e;

	typedef enum MediaDeviceState_e : uint8_t {
		MEDIA_DEV_STATE_STOP = 0,
		MEDIA_DEV_STATE_START,
		MEDIA_DEV_STATE_MAX,
	}MediaDeviceState_e;

	typedef enum MediaAlgo_e : uint8_t {
		MEDIA_ALGO_FILTER = 0,
		MEDIA_ALGO_MAX,
	}MediaAlgo_e;

	typedef struct {
		uint32_t sample_rate;
		uint8_t sample_bits;
		uint8_t channel_num;
		uint8_t vol;
		bool master;
		uint32_t chmap;
		uint16_t fs_cycles;
		uint8_t slot_cycles;
		uint8_t align;
		uint8_t device_cpuid;
	}MediaDeviceCfg_t;

	typedef struct {
		uint32_t sample_rate;
		uint8_t sample_bits;
		uint8_t channel_num;
		uint32_t chmap;
		void* pri;
	}MediaAlgoCfg_t;

	int smf_device_start(MediaDevice_e dev, MediaDeviceCfg_t* cfg);
	int smf_device_stop(MediaDevice_e dev);

	int smf_device_set(MediaDevice_e dev, MediaDeviceCfg_t cfg);
	// int smf_device_start_(MediaDevice_e dev);

	void smf_device_mic_channels_map_set(uint32_t map);
	int smf_device_filter(MediaAlgo_e dev, int status, MediaAlgoCfg_t* cfg);
	void smf_device_clean(uint8_t cpuid);

	uint8_t smf_dma_get_intcodec_halid(MediaDevice_e dev);
	uint8_t smf_dma_cmu_config(bool on);
}
#endif /* __SMF_DEVICE_H__ */
