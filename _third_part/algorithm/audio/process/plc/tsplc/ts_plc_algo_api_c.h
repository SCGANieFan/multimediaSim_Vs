#pragma once
#include<new>
#include "plc_base.h"
#include "plc_algo_api_c.h"
#include "plc_base_port_c.h"
#include "plc_memory_manger_c.h"
#include "preemptive_expand.h"
#include "audio_multi_vector.h"

#define TS_PLC_API_RET_SUCCESS PLC_API_RET_SUCCESS
#define TS_PLC_API_RET_FAIL PLC_API_RET_FAIL

using namespace webrtc;


class data_c
{
public:
	data_c() {};
	~data_c() {};
public:
	//get
	INLINE u8* get_data() {return _buff + _off;};
	INLINE i32 get_size() {return _size;};
	INLINE u8* get_left_data() {return get_data() + _size;};
	INLINE i32 get_left_size() {return _max - _off - _size;};
	INLINE u8* get_buf() {return _buff;};
	INLINE b1 init(u8 *buf, i32 max) {
		_buff = buf;
		_off = 0;
		_size = 0;
		_max = max;
		return true;
	};
	INLINE b1 deinit() {
		_buff = 0;
		_off = 0;
		_size = 0;
		_max = 0;
		return true;
	};
	INLINE b1 append(u8* buf, i32 size) {
		PLC_MEM_CPY(get_left_data(), buf, size);_size += size;return true;
	};
	INLINE b1 append(i32 size) {_size += size;return true;};
	INLINE b1 append_fully(u8* buf, i32 size, i32* usedSize) {
		i32 appendSize = get_left_size();
		appendSize = appendSize > size ? size : appendSize;
		append(buf, appendSize);
		if (usedSize)
			*usedSize = appendSize;
		return true;
	};
	INLINE void used(i32 usedSize)	{
		_off += usedSize;
		_size -= usedSize;
	}
	INLINE void clear(i32 size) {
		if (_off) {
			i32 clear_size = size;
			clear_size = clear_size > _off ? _off : clear_size;
			ALGO_ASSERT(_off > 0);
			ALGO_ASSERT(_size > 0);
			PLC_MEM_MOVE(_buff, _buff + clear_size, _size);
			_off -= clear_size;
		}
	}
	INLINE void clear_used(){
		clear(_off);
	}
protected:
	u8* _buff = 0;
	i32 _off = 0;
	i32 _size = 0;
	i32 _max = 0;
};

class ts_plc_algo_api_c : public plc_algo_api_c {
public:
	ts_plc_algo_api_c() {}
	virtual ~ts_plc_algo_api_c() {}
public:
	plc_api_ret_t create(void** pHd, plc_api_param_t* plc_api_param, plc_base_port_c* plc_base_port) override;
	plc_api_ret_t set(void* hd, plc_api_set_e choose, void* val) override;
	plc_api_ret_t get(void* hd, plc_api_get_e choose, void* val) override;
	plc_api_ret_t run(void* hd, uint8_t* in, int32_t inLen, int32_t* inUsed, uint8_t* out, int32_t* pOutLen, uint16_t is_lost) override;
	plc_api_ret_t destory(void* hd) override;
protected:
	i32 init(plc_api_param_t* plc_api_param);
	i32 deinit();
	virtual ts_plc_algo_api_c* create_hd(plc_base_port_c* plc_base_port) { return 0; };
	virtual i32 run(uint8_t* in, uint8_t* out, i32* out_len, b1 is_lost){return TS_PLC_API_RET_FAIL;};
protected:
	plc_base_port_c* _base_porting = 0;
	plc_memory_manger_c _mm;
	u32 _byte_per_frame;
	u32 _rate;
	u16 _width;
	u16 _channels;
	u16 _channel_select;
	u16 _frame_sample;
	PreemptiveExpand* _pe = 0;
	AudioMultiVector* _algo_buf = 0;
	data_c _tmp_buff;
	i32 _hist_byte = 0;
	i16 _lost_cnt = 0;
};

class ts_plc_algo_api_i16_c : public ts_plc_algo_api_c {
	friend ts_plc_algo_api_c;
public:
	ts_plc_algo_api_i16_c() {}
	virtual ~ts_plc_algo_api_i16_c() {}
protected:
	virtual ts_plc_algo_api_c* create_hd(plc_base_port_c* plc_base_port) override;
	virtual i32 run(uint8_t* in, uint8_t* out, i32* out_len, b1 is_lost) override;
};
