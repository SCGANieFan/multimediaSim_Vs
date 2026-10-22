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
#include "string.h"
#include "SmfPortingDevice.h"
#include "smf_debug.h"

namespace smf {
    static int _echo_bitmap = 0;
    static int _capture_dev = -1;
    static int _playback_dev = -1;
    static MediaDeviceCfg_t* _capture_cfg;
    static MediaDeviceCfg_t* _playback_cfg;
    int smf_echo_capture_device_set(MediaDevice_e dev, MediaDeviceCfg_t* cfg)
    {
		returnIfErrC(-1, dev>=MEDIA_DEV_MAX);
		returnIfErrC(-2, !cfg);
        returnIfErrC(-3, dev & 1);
        _capture_dev = dev;
        _capture_cfg = cfg;
        return 0;
    }
    int smf_echo_playback_device_set(MediaDevice_e dev, MediaDeviceCfg_t* cfg)
    {
		returnIfErrC(-1, dev>=MEDIA_DEV_MAX);
		returnIfErrC(-2, !cfg);
        returnIfErrC(-3, !(dev & 1));
        _playback_dev = dev;
        _playback_cfg = cfg;
        return 0;
    }
    static int smf_echo_start()
    {
        returnIfErrC(-1, _capture_dev < 0 || _playback_dev < 0);
        returnIfErrC(-1, !_capture_cfg || !_playback_cfg);
        smf_device_start((MediaDevice_e)_capture_dev, _capture_cfg);
        smf_device_start((MediaDevice_e)_playback_dev, _playback_cfg);
        return 0;
    }
    static int smf_echo_stop()
    {
        returnIfErrC(-1, _capture_dev < 0 || _playback_dev < 0);
        smf_device_stop((MediaDevice_e)_capture_dev);
        _capture_dev = -1;
        smf_device_stop((MediaDevice_e)_playback_dev);
        _playback_dev = -1;
        return 0;
    }
    int smf_echo_ref_open(MediaDevice_e dev)
    {
        returnIfErrC(-1, !(dev & 1));
        returnIfErrC(-2, dev == _playback_dev);
        int ref_dev_id = dev >> 1;
        int playback_dev_id = _playback_dev >> 1;
        returnIfErrC(0, _echo_bitmap & (1 << ref_dev_id));

        if (!_echo_bitmap){
            if (smf_echo_start() < 0){
                dbgInfoPXL("Echo start failed!!!");
                return -3;
            }
            _echo_bitmap |= 1 << playback_dev_id;
        }
        _echo_bitmap |= 1 << ref_dev_id;
        return 0;
    }
    int smf_echo_ref_close(MediaDevice_e dev)
    {
        returnIfErrC(-1, !(dev & 1));
        returnIfErrC(-2, dev == _playback_dev);
        int ref_dev_id = dev >> 1;
        int playback_dev_id = _playback_dev >> 1;
        returnIfErrC(0, _echo_bitmap == 0);

        _echo_bitmap &= ~(1 << ref_dev_id);
        if (_echo_bitmap == (1 << playback_dev_id)){
            if (smf_echo_stop() < 0){
                dbgInfoPXL("Echo stop failed!!!");
                return -3;
            }
            _echo_bitmap &= ~(1 << playback_dev_id);
        }
        return 0;
    }
}