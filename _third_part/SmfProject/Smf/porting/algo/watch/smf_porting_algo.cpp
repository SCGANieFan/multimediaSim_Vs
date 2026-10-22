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
#include "SmfPortingDevice.h"
#include "smf_msg.h"
#include "smf_debug.h"
#include "smf_audio_process.h"
#include "hw_codec_iir_process.h"

namespace smf {
    static int smf_filter_start(MediaAlgo_e dev, MediaAlgoCfg_t* cfg) {
        returnIfErrC(1, dev >= MEDIA_ALGO_MAX);
        returnIfErrC(2, !cfg);
        enum AUD_SAMPRATE_T sample_rate = (enum AUD_SAMPRATE_T)cfg->sample_rate;
        returnIfErrC(3, !sample_rate);
        uint32_t chmap = cfg->chmap;
        if (!chmap) chmap = AUD_CHANNEL_MAP_CH0 | AUD_CHANNEL_MAP_CH1;
        return hw_codec_iir_open(sample_rate, HW_CODEC_IIR_DAC, chmap);
    }

    static int smf_filter_stop(MediaAlgo_e dev) {
        return hw_codec_iir_close(HW_CODEC_IIR_DAC);
    }

    static int smf_filter_set_parameter(MediaAlgo_e dev, MediaAlgoCfg_t* cfg) {
        returnIfErrC(1, dev >= MEDIA_ALGO_MAX);
        returnIfErrC(2, !cfg);
        enum AUD_SAMPRATE_T sample_rate = (enum AUD_SAMPRATE_T)cfg->sample_rate;
        returnIfErrC(3, !sample_rate);
        returnIfErrC(4, !cfg->pri);
        AUDIO_PROCESS_CFG_T *audio_cfg = (AUDIO_PROCESS_CFG_T *)cfg->pri;
        HW_CODEC_IIR_CFG_T* iir_cfg  = hw_codec_iir_get_cfg(sample_rate,(IIR_CFG_T*)&audio_cfg->algo_cfg.iir_eq);
        return hw_codec_iir_set_cfg((HW_CODEC_IIR_CFG_T*)iir_cfg, sample_rate, HW_CODEC_IIR_DAC);
    }

    static int smf_filter_get_parameter(MediaAlgo_e dev, MediaAlgoCfg_t* cfg) {
        return 0;
    }

    int smf_device_filter(MediaAlgo_e dev, int status, MediaAlgoCfg_t* cfg) {
        dbgTestPXL("%d/%d", dev, status);
        switch (status) {
        case 0: return smf_filter_stop(dev);
        case 1: return smf_filter_start(dev, cfg);
        case 2: return smf_filter_set_parameter(dev, cfg);
        case 3: return smf_filter_get_parameter(dev, cfg);
        default: dbgErrPXL("not opt"); return 100;
        }
        return 0;
    }
}