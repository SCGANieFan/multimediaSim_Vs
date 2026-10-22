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
#include <stdint.h>
#include <stdbool.h>
#include "smf_msg.h"
#include "smf_debug.h"
// #include "SmfPortingShm.h"
#include "SmfPortingIpc.h"
#include "SmfPortingDevice.h"
#include "smf_api_ics.h"

using namespace smf;

EXTERNC bool smf_msvc_clean(uint8_t cpuid);
EXTERNC	int smf_cpu_setup(uint8_t cpuid, bool is_open) {
	dbgTestPDL(is_open);
	if (is_open) {
		returnIfErrC(-2, !smf_ipc_open(cpuid));
		smf_ics_poweron(cpuid);
	}
	else {
		smf_ics_poweroff(cpuid);
		smf_msvc_clean(cpuid);
		smf_ipc_close(cpuid);
		smf_device_clean(cpuid);
	}return 0;
}
