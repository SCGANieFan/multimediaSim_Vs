#pragma once
#include<stdint.h>
#include<stddef.h>
uint32_t plc_api_demo_init(uint32_t rate, uint16_t channels, uint16_t width, uint32_t frameSample);
bool plc_api_demo_run(uint32_t id, uint8_t* pmc_in, int32_t* pmc_in_byte, uint8_t* pcm_out, int32_t* pcm_out_byte, uint16_t is_lost);
bool plc_api_demo_deinit(uint32_t id);






