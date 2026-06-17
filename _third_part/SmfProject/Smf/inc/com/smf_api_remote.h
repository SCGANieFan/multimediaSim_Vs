#pragma once
#ifndef __SMF_API_REMOTE_H__
#define __SMF_API_REMOTE_H__
#include "smf_common.h"

EXTERNC bool smf_remote_set(uint8_t cpuid, const char* path, const void* params, int size);
EXTERNC bool smf_remote_set32(uint8_t cpuid, const char* path, int vals);
EXTERNC bool smf_remote_config(uint8_t cpuid, const char* script, const void* params, int size);

EXTERNC bool smf_remote_assert(uint8_t cpuid);
EXTERNC bool smf_remote_exception(uint8_t cpuid);

EXTERNC bool smf_remote_log_mask(uint8_t cpuid, uint32_t chmask);
EXTERNC bool smf_remote_log_mask_enable(uint8_t cpuid, uint32_t chmask);
EXTERNC bool smf_remote_log_mask_disable(uint8_t cpuid, uint32_t chmask);
EXTERNC bool smf_remote_log_enable(uint8_t cpuid, uint32_t channel);
EXTERNC bool smf_remote_log_disable(uint8_t cpuid, uint32_t channel);
EXTERNC bool smf_remote_notify_log(uint8_t cpuid, bool enable);

EXTERNC bool smf_remote_pool_add(uint8_t cpuid, const char* name, void* addr, uint32_t size, uint32_t thr);
EXTERNC bool smf_remote_pool_add_buffer(uint8_t cpuid, const char* name, void* addr, uint32_t size);
EXTERNC bool smf_remote_pool_add_buffers(uint8_t cpuid, const char* name, smf_pair_t* pairs, uint32_t cnt);
EXTERNC bool smf_remote_pool_add_alias(uint8_t cpuid, const char* name, const char* src);
EXTERNC bool smf_remote_pool_remove(uint8_t cpuid, const char* name);
#endif
