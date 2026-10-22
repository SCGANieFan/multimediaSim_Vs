#pragma once
#include "smf_common.h"
#include "smf_codec_yuv.h"
#include "smf_codec_rgb.h"

EXTERNC smf_display_t* smf_get_display(uint8_t layer);

typedef struct smf_display_config_t {
    uint32_t width;
    uint32_t height;
    uint8_t format;//rgb_format_e yuv_format_e
    uint8_t alpha;/*0~255*/
    uint8_t layer;
    uint8_t res;
}smf_display_config_t;
//porting or customer
EXTERNC void* smf_display_open(void* para);
EXTERNC bool smf_display_close(void* handle);
EXTERNC bool smf_display_config(void* handle, smf_display_config_t*cfg);
EXTERNC bool smf_display_trigger(void* handle, void* fbaddr, uint32_t fbsize);
