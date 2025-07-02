#pragma once

#include "plc_api.h"
#include "plc_base_port_c.h"

class plc_algo_api_c {
public:
	plc_algo_api_c() {}
	virtual ~plc_algo_api_c() {}
public:
	virtual plc_api_ret_t create(void** pHd, plc_api_param_t* param, plc_base_port_c * plc_base_port) {
		return PLC_API_RET_NOT_SUPPORT;
	}
	virtual plc_api_ret_t run(void* hd, uint8_t* in, int32_t inLen, int32_t* inUsed, uint8_t* out, int32_t* pOutLen, uint16_t is_lost) {
		return PLC_API_RET_NOT_SUPPORT;
	}
	virtual plc_api_ret_t set(void* hd, plc_api_set_e choose, void* val) {
		return PLC_API_RET_NOT_SUPPORT;
	}
	virtual plc_api_ret_t get(void* hd, plc_api_get_e choose, void* val) {
		return PLC_API_RET_NOT_SUPPORT;
	}
	virtual plc_api_ret_t destory(void* hd) {
		return PLC_API_RET_NOT_SUPPORT;
	}
};
