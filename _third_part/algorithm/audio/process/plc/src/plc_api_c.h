#pragma once

#include "plc_api.h"
#include "plc_algo_api_c.h"
#include "plc_api_base_port_c.h"
class plc_api_c {
public:
	plc_api_c() {}
	~plc_api_c() {}
public:
	static plc_api_ret_t create(void** pHd, plc_api_param_t* param);
	static plc_api_ret_t run(void* hd, uint8_t* in, int32_t inLen, int32_t* inUsed, uint8_t* out, int32_t* pOutLen, uint16_t isLost);
	static plc_api_ret_t set(void* hd, plc_api_set_e choose, void* val);
	static plc_api_ret_t get(void* hd, plc_api_get_e choose, void* val);
	static plc_api_ret_t destory(void* hd);
private:
	plc_api_ret_t init(plc_api_param_t* param);
public:
	constexpr static const char* _version = "2.0.0.3";
	plc_api_base_port_c _api_base_port;
	plc_algo_api_c* _plc_algo_api = 0;
};

