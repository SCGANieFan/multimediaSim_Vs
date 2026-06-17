#pragma once
#ifndef __SMF_API_LOG_H__
#define __SMF_API_LOG_H__
#include "smf_common.h"

//
typedef int (*smf_log_send_cb_t)(const char* str);
typedef int (*smf_log_send_cb1_t)(const char* str, int size);
typedef int (*smf_log_send_cb2_t)(const char* str, int size, int channel);
typedef int (*smf_log_send_cb3_t)(const char* str, int size, int channel, void* priv);

/*log*/
EXTERNC void smf_log_init(void* tempbuff, int size);
EXTERNC void smf_log_redirect(void (*send)(const char* str));
EXTERNC void smf_log_redirect1(void (*send)(const char* str, int size));
EXTERNC void smf_log_redirect2(void (*send)(const char* str, int size, int channel));
EXTERNC void smf_log_redirect3(void (*send)(const char* str, int size, int channel, void* priv), void* priv);
EXTERNC void smf_log_set_channels_map(uint32_t chmaps);
EXTERNC uint32_t smf_log_get_channels_map(void);
EXTERNC void smf_log_set_channels(int channel, ...);
EXTERNC void smf_log_enable_channels(int channel, ...);
EXTERNC void smf_log_disable_channels(int channel, ...);
EXTERNC void smf_log_disable_channel(int channel);
EXTERNC void smf_log_enable_channel(int channel);
EXTERNC void smf_log_disable_channels_map(uint32_t chmaps);
EXTERNC void smf_log_enable_channels_map(uint32_t chmaps);
EXTERNC void smf_log_disable_output_channels(uint32_t channels_mask);//old api
EXTERNC void smf_log_disable_output_channel(int channel);//old api
EXTERNC void smf_log_enable_output_channel(int channel);//old api
/*background log client*/
EXTERNC void smf_log_redirect_to_cache(void);
EXTERNC void smf_log_redirect_to_background(void* buff, int size);
EXTERNC void smf_log_redirect_to_background1(int size);
/*background log service*/
EXTERNC bool smf_log_background_service(uint32_t interval_ms);
EXTERNC void smf_log_background_save(const char* url, uint32_t duration_ms);
/*notify log*/
EXTERNC void smf_notify_log_enable(uint32_t delay);
EXTERNC void smf_shm_log_register_cb(void (*cb)(const char* str, int size));
//
#endif
