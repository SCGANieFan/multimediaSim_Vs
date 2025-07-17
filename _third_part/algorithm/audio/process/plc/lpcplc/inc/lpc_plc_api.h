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
#ifndef LPC_PLC_API_H
#define LPC_PLC_API_H

#include <stdint.h>
#include <stdio.h>
#include "lpc_plc.h"

//struct LpcPlcState_;
//
//typedef struct LpcPlcState_ LpcPlcState;

typedef struct LpcPlcState_
{
    struct WB_PLC_State *plc[8];
    int32_t channels;
    int32_t samples;
} LpcPlcState;

#ifdef __cplusplus
extern "C" {
#endif

void lpc_plc_create(void *hd);

//void lpc_plc_destroy(LpcPlcState *st);

void lpc_plc_save(void *st, int16_t *buf);

void lpc_plc_generate(void *st, int16_t *buf, int16_t *encbuf);

#ifdef __cplusplus
}
#endif

#endif