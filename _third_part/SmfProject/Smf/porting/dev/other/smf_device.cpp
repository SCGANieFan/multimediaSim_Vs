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
#include "smf_debug.h"
#include "stdbool.h"
#include "string.h"

namespace smf {
	int smf_device_start(MediaDevice_e dev, MediaDeviceCfg_t* cfg) {
		dbgWarnPDL(dev);
		return 0;
	}

	int smf_device_stop(MediaDevice_e dev) {
		dbgWarnPDL(dev);
		return 0;
	}

	void smf_device_clean(uint8_t cpuid) {
	}
}