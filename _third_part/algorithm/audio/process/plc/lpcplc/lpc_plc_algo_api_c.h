#pragma once
#include<new>
#include "plc_base.h"
#include "plc_algo_api_c.h"
#include "plc_base_port_c.h"
#include "plc_memory_manger_c.h"
#include "sbcplc.h"

#define SBC_PLC_API_RET_SUCCESS PLC_API_RET_SUCCESS
#define SBC_PLC_API_RET_FAIL PLC_API_RET_FAIL


class lpc_plc_algo_api_com_c : public plc_algo_api_c {
public:
	lpc_plc_algo_api_com_c() {}
	virtual ~lpc_plc_algo_api_com_c() {}
public:
	plc_api_ret_t create(void** pHd, plc_api_param_t* plc_api_param, plc_base_port_c* plc_base_port) override;
	plc_api_ret_t set(void* hd, plc_api_set_e choose, void* val) override;
	plc_api_ret_t get(void* hd, plc_api_get_e choose, void* val) override;
	plc_api_ret_t run(void* hd, uint8_t* in, int32_t inLen, int32_t* inUsed, uint8_t* out, int32_t* pOutLen, uint16_t is_lost) override;
	plc_api_ret_t destory(void* hd) override;
protected:
	i32 init(plc_api_param_t* plc_api_param);
	i32 deinit();
	virtual lpc_plc_algo_api_com_c* create_hd(plc_base_port_c* plc_base_port) { return 0; };
	virtual i32 run(uint8_t* in, uint8_t* out, b1 is_lost){return SBC_PLC_API_RET_FAIL;};
protected:
	plc_base_port_c* _base_porting = 0;
	plc_memory_manger_c _mm;
	u32 _byte_per_frame;
	u16 _channels;
	u16 _channel_select;
};

class lpc_plc_algo_api_i16_c : public lpc_plc_algo_api_com_c {
	friend lpc_plc_algo_api_com_c;
public:
	lpc_plc_algo_api_i16_c() {}
	virtual ~lpc_plc_algo_api_i16_c() {}
protected:
	virtual lpc_plc_algo_api_com_c* create_hd(plc_base_port_c* plc_base_port) override;
	virtual i32 run(uint8_t* in, uint8_t* out, b1 is_lost) override;
};
