#pragma once
#include<stdint.h>
#include<stddef.h>
uint32_t plc_api_demo_init(uint32_t rate, uint16_t channels, uint16_t width, uint32_t frameSample);
bool plc_api_demo_run(uint32_t id, uint8_t* in, int32_t in_len, int32_t* in_used, uint8_t* out, int32_t* p_out_len, uint16_t is_lost);
bool plc_api_demo_deinit(uint32_t id);






