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
#ifndef __SMF_API_SHM_H__
#define __SMF_API_SHM_H__
#include "smf_frame.h"
#include "smf_media.h"
#ifndef EXTERNC
#ifndef __cplusplus
#define EXTERNC
#else
#define EXTERNC extern "C"
#endif
#endif
//
EXTERNC void* smf_shm_open_slave(const char* shm_name, uint32_t timeout);
EXTERNC void* smf_shm_open(const char* shm_name, int buff_size, int fifo_max);
EXTERNC bool smf_shm_close(void* shm);
//write
EXTERNC bool smf_shm_write_init(void* shm, smf_media_t* media, bool nocopy, bool isring);
EXTERNC bool smf_shm_write(void* shm, smf_frame_t* frm);
EXTERNC bool smf_shm_set_volume(void* shm, uint16_t volume);
EXTERNC bool smf_shm_set_mute(void* shm, bool mute);
//read
EXTERNC bool smf_shm_read_init(void* shm, bool nocopy, bool partread);
EXTERNC smf_media_t* smf_shm_get_media(void* shm, uint32_t timeout);
EXTERNC bool smf_shm_read(void* shm, smf_frame_t* frm);
//fifo
EXTERNC uint32_t smf_shm_get_fifo_max(void* shm);
EXTERNC uint32_t smf_shm_get_fifo_used(void* shm, bool isread);
EXTERNC uint32_t smf_shm_get_fifo_left(void* shm, bool isread);
#endif /* __SMF_PORTING_SHM_H__ */
