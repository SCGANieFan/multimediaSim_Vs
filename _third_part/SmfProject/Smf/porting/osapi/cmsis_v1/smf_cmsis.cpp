/*
 * hifi4.c
 *
 *  Created on: 2021/10/29
 *      Author: wangjg
 */
#include "SmfPorting.h"
#include "smf_debug.h"
#include "cmsis_os.h"

namespace smf {
	void* get_thread_id(void) {
		return (void*)osThreadGetId();
	}

	void sleep_for(uint32_t ms) {
		osDelay(ms);
	}
	void sleep_until(uint64_t ms) {
		osDelayUntil(ms);
	}
	void yield() {
		osThreadYield();
	}
#if 0
	int signal_wait(uint32_t mask, uint32_t timeout) {
		osEvent evt = osSignalWait(mask, (timeout == 0xffffffff) ? osWaitForever : timeout);
		switch ((uint32_t)evt.status) {
		case osEventSignal:	return 0;
		case osEventTimeout:dbgErrPXL("timeout:%u", timeout); return 1;//timeout
		default:dbgErrPXL("%d,%08x", evt.status, mask); return -1;
		}
	}

	int signal_set(uint32_t threadid, uint32_t mask) {
		return osSignalSet((osThreadId)threadid, (int32_t)mask);
	}
#endif
}
