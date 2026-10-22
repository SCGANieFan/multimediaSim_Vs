/***************************************************************************
 *
 * Copyright 2015-2019 BES.
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
//#include "plat_types.h"
//#include "plat_addr_map.h"
#include "SmfPortingDevice.h"
#include "smf_msg.h"
#include "smf_debug.h"
#include "hal_aud.h"
#include "hal_codec.h"
#include "hal_i2s.h"
#include "stdbool.h"
#include "string.h"
#include "hal_dma.h"
#include "hal_tdm.h"
#include "hal_sleep.h"
#include "SmfExitCall.h"
#include "smf_api_ics.h"

namespace smf {
	//
	static int smf_device_start_i2s(MediaDevice_e dev, MediaDeviceCfg_t* cfg) {
		enum HAL_I2S_ID_T i2s_id = (enum HAL_I2S_ID_T)(((dev - MEDIA_DEV_I2S0_RX) >> 1) + HAL_I2S_ID_0);
		enum AUD_STREAM_T stream = (enum AUD_STREAM_T)((dev & 1) ? AUD_STREAM_PLAYBACK : AUD_STREAM_CAPTURE);
		enum HAL_I2S_MODE_T mode = (enum HAL_I2S_MODE_T)((cfg->master) ? HAL_I2S_MODE_MASTER : HAL_I2S_MODE_SLAVE);
		dbgDevPXL("[%d]i2s%u%s(%u),%u*%u*%u", dev, i2s_id,((stream == AUD_STREAM_PLAYBACK) ? "tx" : "rx"), cfg->master
			, cfg->sample_rate,cfg->channel_num,cfg->sample_bits
			);
		int ret = hal_i2s_open(i2s_id, stream, mode);
		returnIfErrCS(ret, ret, "%d", ret);
		struct HAL_I2S_CONFIG_T i2s_cfg;
		memset(&i2s_cfg, 0, sizeof(i2s_cfg));
		i2s_cfg.use_dma = true;
		i2s_cfg.chan_sep_buf = false;
		i2s_cfg.sync_start = false;
		i2s_cfg.cycles = 0;
		i2s_cfg.bits = cfg->sample_bits;
		i2s_cfg.channel_num = cfg->channel_num;
		//i2s_cfg.channel_map = (enum AUD_CHANNEL_MAP_T)(AUD_CHANNEL_MAP_CH0 | AUD_CHANNEL_MAP_CH1);
		i2s_cfg.sample_rate = cfg->sample_rate;
		ret = hal_i2s_setup_stream(i2s_id, stream, &i2s_cfg);
		returnIfErrCS(ret, ret, "%d", ret);
		ret = hal_i2s_start_stream(i2s_id, stream);
		returnIfErrCS(ret, ret, "%d", ret);
		return ret;
	}
	static int smf_device_stop_i2s(MediaDevice_e dev) {
		enum HAL_I2S_ID_T i2s_id = (enum HAL_I2S_ID_T)(((dev - MEDIA_DEV_I2S0_RX) >> 1) + HAL_I2S_ID_0);
		enum AUD_STREAM_T stream = (enum AUD_STREAM_T)((dev & 1) ? AUD_STREAM_PLAYBACK : AUD_STREAM_CAPTURE);
		int ret = hal_i2s_stop_stream(i2s_id, stream);
		returnIfErrCS(ret, ret, "%d", ret);
		ret = hal_i2s_close(i2s_id, stream);
		returnIfErrCS(ret, ret, "%d", ret);
		return ret;
	}

	static int smf_device_start_tdm(MediaDevice_e dev, MediaDeviceCfg_t* cfg) {
		enum HAL_I2S_ID_T i2s_id = (enum HAL_I2S_ID_T)(((dev - MEDIA_DEV_TDM_I2S0_RX) >> 1) + HAL_I2S_ID_0);
		enum AUD_STREAM_T stream = (enum AUD_STREAM_T)((dev & 1) ? AUD_STREAM_PLAYBACK : AUD_STREAM_CAPTURE);
		enum HAL_I2S_MODE_T mode = (enum HAL_I2S_MODE_T)((cfg->master) ? HAL_I2S_MODE_MASTER : HAL_I2S_MODE_SLAVE);
		enum AUD_SAMPRATE_T rate = (enum AUD_SAMPRATE_T)cfg->sample_rate;
		dbgDevPXL("[%d]i2s%u%s(%u)%d/%d", dev, i2s_id, ((stream == AUD_STREAM_PLAYBACK) ? "tx" : "rx"), cfg->master
			, cfg->slot_cycles, cfg->fs_cycles);
		int ret = hal_tdm_open(i2s_id, stream, mode);
		returnIfErrCS(ret, ret, "%d", ret);
		struct HAL_TDM_CONFIG_T tdm_cfg;
		memset(&tdm_cfg, 0, sizeof(tdm_cfg));
		tdm_cfg.mode = HAL_TDM_MODE_FS_ASSERTED_AT_FIRST;// �ź���frame�ĵ�һ��/���һ��bit������
        tdm_cfg.edge = HAL_TDM_FS_EDGE_POSEDGE;//fs_edge �ӵ�һ��������/�½��ؿ�ʼ
        tdm_cfg.cycles = (enum HAL_TDM_CYCLES_T)(cfg->channel_num * cfg->slot_cycles);// ÿ֡��bit��=ch*slot
        tdm_cfg.fs_cycles = (enum HAL_TDM_FS_CYCLES)cfg->fs_cycles;//
        tdm_cfg.slot_cycles = (enum HAL_TDM_SLOT_CYCLES_T)cfg->slot_cycles;
        tdm_cfg.bits = (enum AUD_BITS_T)cfg->sample_bits;
        tdm_cfg.channel_num = (enum AUD_CHANNEL_NUM_T)cfg->channel_num;

		// enum AUD_DATA_ALIGN_T align = cfg->align;
		// tdm_cfg.data_offset =  = 0;
		// if(align == AUD_DATA_ALIGN_RIGHT_JUSTIFIED && cfg->sample_bits == 24) {
		// 	tdm_cfg.data_offset = 8;
		// }

        tdm_cfg.data_offset = 0;//cfg->slot_cycles - cfg->sample_bits;//16bit,���Ҷ����ֵ��Ϊ0.24bit,�����0,�Ҷ���8
        tdm_cfg.chan_sep_buf = false;
        tdm_cfg.sync_start = false;
        tdm_cfg.ext_mclk_freq = 0;
		ret = hal_tdm_setup_stream(i2s_id, stream, rate, &tdm_cfg);
		returnIfErrCS(ret, ret, "%d", ret);
		ret = hal_tdm_start_stream(i2s_id, stream);
		returnIfErrCS(ret, ret, "%d", ret);
		return ret;
	}
	static int smf_device_stop_tdm(MediaDevice_e dev) {
		enum HAL_I2S_ID_T i2s_id = (enum HAL_I2S_ID_T)(((dev - MEDIA_DEV_TDM_I2S0_RX) >> 1) + HAL_I2S_ID_0);
		enum AUD_STREAM_T stream = (enum AUD_STREAM_T)((dev & 1) ? AUD_STREAM_PLAYBACK : AUD_STREAM_CAPTURE);
		int ret = hal_tdm_stop_stream(i2s_id, stream);
		returnIfErrCS(ret, ret, "%d", ret);
		ret = hal_tdm_close(i2s_id, stream);
		returnIfErrCS(ret, ret, "%d", ret);
		return ret;
	}
	typedef struct {
		int (*start)(MediaDevice_e dev, MediaDeviceCfg_t* cfg);
		int (*stop)(MediaDevice_e dev);
	}dev_cb_t;
	static uint32_t _dev_status = 0;
	static dev_cb_t _devs[] = {
		{NULL, NULL},
		{NULL, NULL},
		{NULL, NULL},
		{NULL, NULL},
		{&smf_device_start_i2s,&smf_device_stop_i2s},
		{&smf_device_start_i2s,&smf_device_stop_i2s},
		{&smf_device_start_tdm,&smf_device_stop_tdm},
		{&smf_device_start_tdm,&smf_device_stop_tdm}
	};
	int smf_device_start(MediaDevice_e dev, MediaDeviceCfg_t* cfg){
		//dbgDevPDL(dev);
		//dbgTestPDL(cfg->sample_rate);
		//dbgTestPDL(cfg->channel_num);
		//dbgTestPDL(cfg->sample_bits);
		//dbgTestPDL(cfg->vol);
		//dbgTestPPL((uint32_t)cfg->attr);
		returnIfErrC(1, dev>=MEDIA_DEV_MAX);
		returnIfErrC(2, !cfg);
		if (_dev_status & (1<<dev)) {
			dbgWarnPL();
			return 0;
		}
		int (*start)(MediaDevice_e dev, MediaDeviceCfg_t * cfg)
			= _devs[dev >> 1].start;
		returnIfErrC(3, !start);
		int ret = start(dev, cfg);
		returnIfErrCS(400+ret, ret, "%d", ret);
			//device status
		_dev_status |= (1 << dev);
		return 0;
	}

	int smf_device_stop(MediaDevice_e dev) {
		dbgDevPDL(dev);
		returnIfErrC(1, dev >= MEDIA_DEV_MAX);
		if (!(_dev_status & (1 << dev))) {
			dbgWarnPL();
			return 0;
		}
		int (*stop)(MediaDevice_e dev)
			= _devs[dev >> 1].stop;
		returnIfErrC(3, !stop);
		int ret = stop(dev);
		returnIfErrCS(400 + ret, ret, "%d", ret);
		//device status
		_dev_status &= ~(1 << dev);
		//dbgTestPDL(dev);
		return 0;
	}
}
