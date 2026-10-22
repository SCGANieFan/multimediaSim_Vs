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
#include "hal_btpcm.h"
#include "codec_int.h"
#include "stdbool.h"
#include "string.h"
#include "hal_dma.h"
#include "hal_tdm.h"
#include "hal_sleep.h"
#include "SmfIntCodec.h"
#include "SmfExitCall.h"
#include "smf_api_ics.h"

namespace smf {
	//

#if !defined(CODEC_CLK_SEL_AON) && defined(CODEC_CLK_SEL_BTH)
static uint32_t codec_device_count = 0;
#endif

#define SPEECH_CODEC_CAPTURE_CHANNEL_NUM 1
#define DIGITAL_REG(a)                        (*(volatile uint32_t *)(a))
#define SMF_INTCODEC_MAX 3
	static uint32_t _micMaps[] = {
		0,
		AUD_CHANNEL_MAP_CH0,
		AUD_CHANNEL_MAP_CH0 | AUD_CHANNEL_MAP_CH1,
	};

	static SmfIntCodec _intcodecObjs[SMF_INTCODEC_MAX] = {
		{1, codec_int_open, codec_int_close, codec_int_stream_open, codec_int_stream_close, codec_int_stream_start, codec_int_stream_stop, codec_int_stream_setup}
#ifdef SMF_USING_CODEC2
		,{2, codec2_int_open, codec2_int_close, codec2_int_stream_open, codec2_int_stream_close, codec2_int_stream_start, codec2_int_stream_stop, codec2_int_stream_setup}
#endif
#ifdef SMF_USING_CODEC3
		,{3, codec3_int_open, codec3_int_close, codec3_int_stream_open, codec3_int_stream_close, codec3_int_stream_start, codec3_int_stream_stop, codec3_int_stream_setup}
#endif
	};
	static int smf_device_start_intcodec(MediaDevice_e dev, MediaDeviceCfg_t* cfg) {
#if !defined(CODEC_CLK_SEL_AON) && defined(CODEC_CLK_SEL_BTH)
		returnIfErrC(false, !smf_ics_sync_ready(SMF_CPU_CP_3, 1000, 5));
		if(!codec_device_count){
			hal_cmu_codec_bth_clock_enable();
			hal_cmu_codec_bth_clock_reset_clr();
			dbgTestPXL("Enable bth clock");
		}
		codec_device_count++;
		ExitCallx1 excb([](void* priv){
			codec_device_count--;
			dbgErrPXL("codec open fail,device count:%d", codec_device_count);
			if(!codec_device_count){
				hal_cmu_codec_bth_clock_reset_set();
				hal_cmu_codec_bth_clock_disable();   
			}
		}, NULL);
#else
#ifdef SMF_USING_CODEC2
	if(dev == MEDIA_DEV_INTCODEC_RX2) {
		//bth use codec, need block
		dbgDevPXL("lock subsys");
		hal_subsys_wake_lock(HAL_SUBSYS_WAKE_LOCK_USER_VAD);
	}
#endif
#endif
		enum AUD_STREAM_T stream = (enum AUD_STREAM_T)((dev & 1) ? AUD_STREAM_PLAYBACK : AUD_STREAM_CAPTURE);
		struct HAL_CODEC_CONFIG_T codec_cfg;
		memset(&codec_cfg, 0, sizeof(codec_cfg));
		codec_cfg.bits = (enum AUD_BITS_T)cfg->sample_bits;
		codec_cfg.sample_rate = (enum AUD_SAMPRATE_T)cfg->sample_rate;
		codec_cfg.channel_num = (enum AUD_CHANNEL_NUM_T)cfg->channel_num;
		if (stream == AUD_STREAM_CAPTURE) {//mic select channels map.
			//dbgDevPXL("chmap:%x",cfg->chmap);
			uint32_t chmap = cfg->chmap;
			if (!chmap)chmap = _micMaps[codec_cfg.channel_num];
			codec_cfg.channel_map = (enum AUD_CHANNEL_MAP_T)chmap;
#if defined(CHIP_BEST3601) || defined(CHIP_BEST3601P)
			if(codec_cfg.channel_map&AUD_CHANNEL_MAP_ECMIC_CH0){
				hal_codec_set_echo_rate(HAL_CODEC_ECHO_RATE_384K);
				hal_codec_set_echo_ch(HAL_CODEC_ECHO_CHAN_DAC_DATA_OUT_L);
				hal_codec_set_echo_path(HAL_CODEC_ECHO_PATH_EQ);
			}
#endif
		}
		codec_cfg.use_dma = 1;// CODEC_TRUE;
		#ifdef KERNEL_NUTTX
			codec_cfg.vol = 100;
		#else
			codec_cfg.vol = cfg->vol;//TGT_VOLUME_LEVEL_MAX;
			if(!codec_cfg.vol || (codec_cfg.vol > TGT_VOLUME_LEVEL_MAX)) {
				dbgNotePXL("vol is null,use max value,%d", TGT_VOLUME_LEVEL_MAX);
				codec_cfg.vol = TGT_VOLUME_LEVEL_MAX;
			}
			//dbgDevPDL(codec_cfg.vol);
		#endif
		codec_cfg.io_path = (dev & 1) ? AUD_OUTPUT_PATH_SPEAKER : AUD_INPUT_PATH_MAINMIC;// AUD_IO_PATH_NULL;
		dbgDevPXL("[%u]%s,%u*%u*%u,map:%08x,vol:%u", dev, (codec_cfg.io_path== AUD_OUTPUT_PATH_SPEAKER?"spk":"mic")
			, codec_cfg.sample_rate, codec_cfg.channel_num, codec_cfg.bits
			, codec_cfg.channel_map, codec_cfg.vol
		);
		// int ret = -1;
		int index = (int)dev >> 1;
		returnIfWarnC(-1, !(index >= 0 && index < SMF_INTCODEC_MAX));
		SmfIntCodec& obj = _intcodecObjs[index];
		returnIfWarnC(-1, !obj);
		returnIfErrC(-1, obj.Start(stream, codec_cfg));
#if !defined(CODEC_CLK_SEL_AON) && defined(CODEC_CLK_SEL_BTH)
		dbgTestPXL("current codec count:%d", codec_device_count);
		excb.Reset();
#endif
		return 0;
	}
	static int smf_device_stop_intcodec(MediaDevice_e dev) {
		enum AUD_STREAM_T stream = (enum AUD_STREAM_T)((dev & 1) ? AUD_STREAM_PLAYBACK : AUD_STREAM_CAPTURE);
		int index = (int)dev >> 1;
		returnIfWarnC(-1, !(index >= 0 && index < SMF_INTCODEC_MAX));
		SmfIntCodec& obj = _intcodecObjs[index];
		returnIfWarnC(-1, !obj);
		returnIfErrC(-1, obj.Stop(stream));
#if !defined(CODEC_CLK_SEL_AON) && defined(CODEC_CLK_SEL_BTH)
		codec_device_count-- ;
		if(!codec_device_count){
			hal_cmu_codec_bth_clock_reset_set();
			hal_cmu_codec_bth_clock_disable();   
		}
		dbgTestPXL("current codec count:%d", codec_device_count);
#else
#ifdef SMF_USING_CODEC2
	if(dev == MEDIA_DEV_INTCODEC_RX2) {
		dbgWarnPXL("unlock subsys");
		hal_subsys_wake_unlock(HAL_SUBSYS_WAKE_LOCK_USER_VAD);
	}
#endif
#endif
		return 0;
	}
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
		if(cfg->chmap) i2s_cfg.channel_map = (enum AUD_CHANNEL_MAP_T)cfg->chmap;
		else i2s_cfg.channel_map = (enum AUD_CHANNEL_MAP_T)(AUD_CHANNEL_MAP_CH0 | AUD_CHANNEL_MAP_CH1);
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
	static int smf_device_start_btpcm(MediaDevice_e dev, MediaDeviceCfg_t* cfg) {
		
		enum AUD_STREAM_T stream = (enum AUD_STREAM_T)((dev & 1) ? AUD_STREAM_PLAYBACK : AUD_STREAM_CAPTURE);
		enum HAL_BTPCM_ID_T id = HAL_BTPCM_ID_0;
		enum AUD_SAMPRATE_T rate = (enum AUD_SAMPRATE_T)cfg->sample_rate;
		//MediaDeviceCodec_e codec = cfg->codec;
		//switch (codec){
		//case MEDIA_DEV_CODEC_MSBC:rate = AUD_SAMPRATE_16000; break;
		//case MEDIA_DEV_CODEC_CVSD:rate = AUD_SAMPRATE_8000; break;
		//default:dbgErrPDL(codec);return 100;
		//}
#ifdef CHIP_BEST3601P
		hal_cmu_btpcm_sel_cfg(true);
#endif
#ifdef CHIP_BEST1600	
		uint8_t* btpcm_sel = (uint8_t*)0xD03300A8;
		(*btpcm_sel) |= 1;
#endif
		dbgDevPXL("[%d]%s", dev, (stream == AUD_STREAM_PLAYBACK) ? "tx" : "rx");
		int ret = hal_btpcm_open(id, stream);
		returnIfErrCS(ret, ret, "%d", ret);

		struct HAL_BTPCM_CONFIG_T btpcm_cfg;
		memset(&btpcm_cfg, 0, sizeof(btpcm_cfg));
		btpcm_cfg.use_dma = 1;
		btpcm_cfg.bits = AUD_BITS_16;
		btpcm_cfg.channel_num = SPEECH_CODEC_CAPTURE_CHANNEL_NUM;
		btpcm_cfg.sample_rate = rate;
		ret = hal_btpcm_setup_stream(id, stream, &btpcm_cfg);
		returnIfErrCS(ret, ret, "%d", ret);
		ret = hal_btpcm_start_stream(id, stream);
		returnIfErrCS(ret, ret, "%d", ret);
		return ret;
	}
	static int smf_device_stop_btpcm(MediaDevice_e dev) {
		enum AUD_STREAM_T stream = (enum AUD_STREAM_T)((dev & 1) ? AUD_STREAM_PLAYBACK : AUD_STREAM_CAPTURE);
		enum HAL_BTPCM_ID_T id = HAL_BTPCM_ID_0;
		int ret = hal_btpcm_stop_stream(id, stream);
		returnIfErrCS(ret, ret, "%d", ret);
		ret = hal_btpcm_close(id, stream);
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
		{&smf_device_start_intcodec,&smf_device_stop_intcodec},
		{&smf_device_start_intcodec,&smf_device_stop_intcodec},
		{&smf_device_start_intcodec,&smf_device_stop_intcodec},
		{&smf_device_start_btpcm,&smf_device_stop_btpcm},
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

	void smf_device_clean(uint8_t cpuid) {
		switch (cpuid) {
		case SMF_CPU_DSP:
		#ifdef CHIP_BEST1700
			//hal_dsp_dma_close();
		#endif
			break;
		}
	}
}
