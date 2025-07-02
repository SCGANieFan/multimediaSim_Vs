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
#include <stdlib.h>
#include "lpc_plc_api.h"
#include "lpc_plc.h"

#ifndef NULL
#define NULL 0
#endif

void lpc_plc_create(void *hd)
{
    struct WB_PLC_State* st = (struct WB_PLC_State*)hd;
    Reset_WB_PLC(st);
}

void lpc_plc_save(void *hd, int16_t *buf)
{
    struct WB_PLC_State *st = (struct WB_PLC_State*)hd;
    WB_PLC(st, buf, buf);
}

void lpc_plc_generate(void *hd, int16_t *buf, int16_t *encbuf)
{
    struct WB_PLC_State* st = (struct WB_PLC_State*)hd;
    WB_PLC_erasure(st, buf, encbuf, NULL);
}