#include "FreeRTOS.h"                   // ARM.FreeRTOS::RTOS:Core
#include "task.h"                       // ARM.FreeRTOS::RTOS:Core
#include "SmfPortingThread.h"
#include "smf_debug.h"

EXTERNC int32_t osKernelLock(void);
EXTERNC int32_t osKernelUnlock(void);
EXTERNC int32_t osKernelRestoreLock(void);
namespace smf {
#if 0
	typedef struct
	{
		TaskHandle_t xHandle;			/* The handle of the task to which the rest of the information in the structure relates. */
		const char* pcTaskName;			/* A pointer to the task's name.  This value will be invalid if the task was deleted since the structure was populated! */ /*lint !e971 Unqualified char types are allowed for strings and single characters only. */
		uint32_t ulCurrCounter;
		uint32_t ulRunTimeCounter;		/* The total run time allocated to the task so far, as defined by the run time stats clock.  See http://www.freertos.org/rtos-run-time-stats.html.  Only valid when configGENERATE_RUN_TIME_STATS is defined as 1 in FreeRTOSConfig.h. */
		uint32_t ulStackFreeMin;		/* The minimum amount of stack space that has remained for the task since the task was created.  The closer this value is to zero the closer the task has come to overflowing its stack. */
		uint32_t ulStackTotal;
	} SmfTaskInfo_t;

	void vTaskGetInfoPriv(TaskHandle_t xTask, SmfTaskInfo_t* pxTaskInfo)
	{
		TaskStatus_t status;
		vTaskGetInfo(xTask, &status, pdTRUE, eInvalid);
		pxTaskInfo->xHandle = status.xHandle;
		if(!status.xHandle) {
			// dbgWarnPL();
			return;
		}
		pxTaskInfo->pcTaskName = status.pcTaskName;
		pxTaskInfo->ulRunTimeCounter = status.ulRunTimeCounter;
		#if ( configGENERATE_RUN_TIME_STATS == 1 )
		pxTaskInfo->ulCurrCounter = portGET_RUN_TIME_COUNTER_VALUE();
		#else
		pxTaskInfo->ulCurrCounter = 0;
		#endif
		pxTaskInfo->ulStackFreeMin = status.usStackHighWaterMark << 2;
		StaticTask_t * xHandle = (StaticTask_t *)status.xHandle;
		pxTaskInfo->ulStackTotal = (uint8_t*)xHandle->pxDummy8 - (uint8_t*)xHandle->pxDummy6;
	}
#endif
#if 0
	using Item_t = ThreadItem_t;
	void* SmfGetThreadID(const char* name) {
		return xTaskGetHandle(name);
	}
	bool SmfThreadUpdateInfo(Item_t* item) {
		TaskInfo_t info;
		vTaskGetInfoPriv((TaskHandle_t)item->_id, &info);
		// returnIfWarnC(false, !info.xHandle);
		if(!info.xHandle) return false;
		auto rt = (info.ulRunTimeCounter >= item->_runtime) ? (info.ulRunTimeCounter - item->_runtime) : ((0xfffffffful) - item->_runtime + info.ulRunTimeCounter);
		auto dd = (info.ulCurrCounter >= item->_counter) ? (info.ulCurrCounter - item->_counter) : ((0xfffffffful) - item->_counter + info.ulCurrCounter);
		//int usage = dd ? ((rt * 100) / dd ) : 0;
		item->_usage = dd ? ((rt * 10000ull) / dd) : 0;
		item->_runtime = info.ulRunTimeCounter;
		item->_counter = info.ulCurrCounter;
		item->_stackFreeMin = info.ulStackFreeMin;
		item->_stackSize = info.ulStackTotal;
		return true;
	}
#endif
#if 1
	using Item_t = ThreadItem_t;
	void* SmfGetThreadID(const char* name) {
		return xTaskGetHandle(name);
	}
	bool SmfThreadUpdateInfo(Item_t* item) {
		TaskStatus_t status;
		auto xTask = (TaskHandle_t)item->_id;
		vTaskGetInfo(xTask, &status, pdTRUE, eInvalid);

		//auto pcTaskName = status.pcTaskName;
		auto ulRunTimeCounter = status.ulRunTimeCounter;
		auto ulCurrCounter = ulCpuGetCurCount();

		auto ulStackFreeMin = status.usStackHighWaterMark << 2;
		auto xHandle = (StaticTask_t*)status.xHandle;
		//auto ulStackTotal = 0;// (uint8_t*)xHandle->pxDummy8 - (uint8_t*)xHandle->pxDummy6;
		auto stackFreeMin = status.usStackHighWaterMark;

		// returnIfWarnC(false, !info.xHandle);
		if (!xHandle) return false;
		auto rt = (ulRunTimeCounter >= item->_runtime) ? (ulRunTimeCounter - item->_runtime) : ((0xfffffffful) - item->_runtime + ulRunTimeCounter);
		auto dd = (ulCurrCounter >= item->_counter) ? (ulCurrCounter - item->_counter) : ((0xfffffffful) - item->_counter + ulCurrCounter);
		//int usage = dd ? ((rt * 100) / dd ) : 0;
		item->_usage = dd ? ((rt * 10000ull) / dd) : 0;
		item->_runtime = ulRunTimeCounter;
		item->_counter = ulCurrCounter;
		item->_stackFreeMin = stackFreeMin;
		//item->_stackSize = ulStackTotal;
		return true;
	}
	
	bool os_lock(void) {
		auto rst = osKernelLock();
		switch (rst) {
		case 1://locked
			return true;
		case 0://not locked
			return true;
		default://error coded
			dbgErrPDL(rst);
			return false;
		}
	}
	void os_unlock(void) {
		auto rst = osKernelRestoreLock();
		switch (rst) {
		case 1://locked
			break;
		case 0://not locked
			break;
		default://error coded
			dbgErrPDL(rst);
			break;
		}
	}
#endif
}
