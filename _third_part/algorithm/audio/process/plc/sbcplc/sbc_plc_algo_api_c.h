#pragma once
#include<new>
#include "plc_base.h"
#include "plc_algo_api_c.h"
#include "plc_base_port_c.h"
#include "plc_memory_manger_c.h"
#include "sbcplc.h"

#define SBC_PLC_API_RET_SUCCESS PLC_API_RET_SUCCESS
#define SBC_PLC_API_RET_FAIL PLC_API_RET_FAIL


class sbc_plc_algo_api_com_c : public plc_algo_api_c {
public:
	sbc_plc_algo_api_com_c() {}
	virtual ~sbc_plc_algo_api_com_c() {}
public:
	plc_api_ret_t create(plc_api_param_t* plc_api_param, plc_base_port_c* plc_base_port) override;
	plc_api_ret_t set(plc_api_set_e choose, void* val) override;
	plc_api_ret_t get(plc_api_get_e choose, void* val) override;
	plc_api_ret_t run(uint8_t* in, int32_t inLen, int32_t* inUsed, uint8_t* out, int32_t* pOutLen, uint16_t is_lost) override;
	plc_api_ret_t destory() override;
protected:
	i32 init(plc_api_param_t* plc_api_param);
	i32 deinit();
	virtual sbc_plc_algo_api_com_c* create_hd(plc_base_port_c* plc_base_port) { return 0; };
	virtual i32 run(uint8_t* in, uint8_t* out, b1 is_lost){return SBC_PLC_API_RET_FAIL;};
protected:
	plc_base_port_c* _base_porting = 0;
	plc_memory_manger_c _mm;
	u32 _byte_per_frame;
	u16 _channels;
	u16 _channel_select;

	struct PLC_State* _hd[2] = { 0 };
	void* _history[2] = { 0 };
	void* _rcos = 0;
	uint32_t _SearchSize = 0;
	uint32_t _MatchSize = 0;
	uint32_t _Olal = 0;
	uint32_t _Recover = 0;
	uint32_t _sampleNum = 0;
	enum sbc_plc_codec_type_e _codecType;
	void* _rcosBuff = 0;
	uint32_t _packetNum = 0;
	uint32_t _smoothNum = 0;

};

class sbc_plc_algo_api_i16_c : public sbc_plc_algo_api_com_c {
	friend sbc_plc_algo_api_com_c;
public:
	sbc_plc_algo_api_i16_c() {}
	virtual ~sbc_plc_algo_api_i16_c() {}
protected:
	virtual sbc_plc_algo_api_com_c* create_hd(plc_base_port_c* plc_base_port) override;
	virtual i32 run(uint8_t* in, uint8_t* out, b1 is_lost) override;
};
#if 0

class sbc_plc_algo_api_i24_c : public sbc_plc_algo_api_com_c {
public:
	sbc_plc_algo_api_i24_c() {}
	virtual ~sbc_plc_algo_api_i24_c() {}
protected:
	virtual sbc_plc_algo_api_com_c* create_hd() override;
	virtual i32 run(uint8_t* in, uint8_t* out, b1 is_lost) override;
};

class sbc_plc_algo_api_i32_c : public sbc_plc_algo_api_com_c {
public:
	sbc_plc_algo_api_i32_c() {}
	virtual ~sbc_plc_algo_api_i32_c() {}
protected:
	virtual sbc_plc_algo_api_com_c* create_hd() override;
	virtual i32 run(uint8_t* in, uint8_t* out, b1 is_lost) override;
};
#endif