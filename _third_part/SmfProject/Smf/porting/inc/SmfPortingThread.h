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
#ifndef __SMF_PORTING_SHM_H__
#define __SMF_PORTING_SHM_H__
#include <stdint.h>
#include <stdbool.h>
#include <smf_common.h>
#include <smf_msg.h>
namespace smf {
	struct ThreadItem_t {
		uint64_t _name;
		void* _id;
		void* _para;
		void (*_func)(void*);
		uint32_t _stackSize;
		uint32_t _stackFreeMin;
		mutable uint32_t _runtime;
		mutable uint32_t _counter;
		mutable uint32_t _usage;
		void* _sema;
	};

	void* SmfGetThreadID(const char* name);	
	bool SmfThreadUpdateInfo(ThreadItem_t* item);

	ThreadItem_t* SmfThreadHold(const char* item);
	void SmfThreadRelease(ThreadItem_t* item);
}
#endif /* __SMF_PORTING_SHM_H__ */
