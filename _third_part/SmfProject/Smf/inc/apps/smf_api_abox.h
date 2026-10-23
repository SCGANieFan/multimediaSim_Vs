#pragma once
#ifndef __SMF_API_ABOX_H__
#define __SMF_API_ABOX_H__
#include "smf_common.h"

typedef enum{
    SMF_ABOX_STATUS_NULL = 0,
    SMF_ABOX_STATUS_START = 2,
     SMF_ABOX_STATUS_STOP = 3,
}SMF_ABOX_STATUS;

typedef void (*SmfAboxCallback)(SMF_ABOX_STATUS status);

EXTERNC bool smf_msg_service_open(void);
EXTERNC bool smf_msg_service_close(void);
EXTERNC bool smf_abox_prompt_play(bool onoff, void* status);
EXTERNC uint64_t smf_abox_add_lineinsouce(void);
EXTERNC bool smf_abox_remove_lineinsouce(void);
EXTERNC uint64_t smf_abox_add_filesouce(const char* fname, uint32_t position);
EXTERNC bool smf_abox_remove_filesouce(uint64_t id);
EXTERNC uint64_t smf_abox_add_btsouce(const char* codec);
EXTERNC bool smf_abox_remove_btsouce(void);
EXTERNC bool smf_abox_btsouce_receive_data(void* data, int size, unsigned index, unsigned timestamp);
EXTERNC uint64_t smf_abox_add_blesouce(uint8_t con_lid, uint32_t context_type);
EXTERNC bool smf_abox_remove_blesouce(void);

EXTERNC bool smf_abox_add_afsink(uint8_t sync_type);
EXTERNC bool smf_abox_remove_afsink(void);
EXTERNC bool smf_abox_add_blesink(void* event);
EXTERNC bool smf_abox_remove_blesink(void);
EXTERNC bool smf_abox_play(uint64_t id);
EXTERNC bool smf_abox_stop(uint64_t id);
EXTERNC bool smf_abox_pause(uint64_t id);
EXTERNC bool smf_abox_resume(uint64_t id);
EXTERNC bool smf_abox_set_volume(uint64_t id, uint16_t volume);
EXTERNC bool smf_abox_set_global_volume(uint32_t volume);
EXTERNC bool smf_abox_set_mute(uint64_t id, bool mute);
EXTERNC uint32_t smf_abox_get_status(uint64_t id);//runing=2,pause=3
EXTERNC uint32_t smf_abox_get_currrent_postion(uint64_t id);
EXTERNC bool smf_abox_stream_push(uint64_t id, void* buff, int size);
EXTERNC uint64_t smf_abox_stream_open(const char* media_type);
EXTERNC bool smf_abox_register_status_callback(uint64_t id, SmfAboxCallback callback);
EXTERNC bool smf_abox_set_speed(uint64_t id, uint32_t speed);
EXTERNC bool smf_abox_enable_single_loop(uint64_t id,bool loop);
#endif
