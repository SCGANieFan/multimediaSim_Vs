#pragma once
#include <stdint.h>

#ifndef EXTERNC
#ifdef __cplusplus
#define EXTERNC extern "C" 
#else
#define EXTERNC
#endif
#endif


#define PLC_API_RET_SUCCESS			(0)
#define PLC_API_RET_INPUT_ERROR		(-1)
#define PLC_API_RET_FAIL			(-2)
#define PLC_API_RET_NOT_SUPPORT		(-3)


typedef struct plc_api_param_t {
	void* (*malloc_cb)(uint32_t size) = 0;                  //dynamic memory alloc function pointer, it can not be null
	void* (*realloc_cb)(void* rmem, uint32_t newsize) = 0;  //dynamic memory realloc function pointer, it can not be null
	void(*free_cb)(void* buf) = 0;                          //dynamic memory free function pointer, it can not be null
	void(*print_cb)(const char* fmt, ...) = 0;              //log print function pointer, if donot want print, it can be null
};


typedef int32_t plc_api_ret_t;

EXTERNC uint32_t plc_api_create(plc_api_param_t* param);
EXTERNC plc_api_ret_t plc_api_open(uint32_t id);
EXTERNC plc_api_ret_t plc_api_receive(uint32_t id, uint8_t* in, int32_t in_len);
EXTERNC plc_api_ret_t plc_api_run(uint32_t id, uint8_t* in, int32_t in_len, int32_t* in_used, uint8_t* out, int32_t* p_out_len, uint16_t is_lost);
EXTERNC plc_api_ret_t plc_api_generate(uint32_t id, uint8_t* out, int32_t* p_out_len);
EXTERNC plc_api_ret_t plc_api_set(uint32_t id, const char *choose, void* val);
EXTERNC plc_api_ret_t plc_api_get(uint32_t id, const char* choose, void* val);
EXTERNC plc_api_ret_t plc_api_close (uint32_t id);
EXTERNC plc_api_ret_t plc_api_destory(uint32_t id);

EXTERNC void plc_api_register_music_plc_int16(void);
EXTERNC void plc_api_register_music_plc_int32(void);
EXTERNC void plc_api_register_music_plc_f32(void);

