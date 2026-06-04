/*
 * SmfPorting.h
 *
 *  Created on: 2024/8/22
 *      Author: wangjg
 */
#pragma once
#include <stdint.h>
#include <stdbool.h>

#ifndef EXTERNC
#ifndef __cplusplus
#define EXTERNC
#else
#define EXTERNC extern "C"
#endif
#endif

#define CPU_FREQ_MAX 0xffffffff
#define CPU_FREQ_MIN 0

namespace smf {
	//
	bool smf_initialize(bool reinit);
	//cpu
	uint32_t get_cpu_idx();
	uint64_t get_cpu_name64();
	const char* get_cpu_name();
	uint32_t get_cpu_freq();//hz
	bool set_cpu_freq(uint32_t hz);//hz
	uint32_t get_cpu_usage();
	uint32_t get_cpu_cycle();
	uint64_t get64_cpu_cycle();
	const char* get_platform();
	bool cpu_open(uint8_t cpuid);
	bool cpu_close(uint8_t cpuid);
	uint32_t get_workloads();

	//time
	void timer_sync();
	uint32_t get_ms();
	uint32_t get_us();
	uint64_t get64_ms();
	uint64_t get64_us();	
	uint32_t get_timestamp();
	uint32_t get_ticks();
	uint64_t get64_ticks();
	int64_t get_local_ms();
	int64_t get_local_us();
	//void set64_us(uint64_t us);
	//void set64_ticks(uint64_t ticks);
	//
	//void* get_flash_addr(uint32_t* size);
	//void* get_param_addr(uint32_t* size);
	//void* get_item_addr(uint32_t* size);
	//void* get_dtcm_addr(uint32_t* size);
	//void* get_sram_addr(uint32_t* size);
	//rtc
	uint32_t get_rtc_ms();
	uint64_t get64_rtc_ms();
	char* rtc_printf(char* ptr, char* end);
	char* rtc_printf(char* ptr, char* end, const char* pattern, void* others = 0);
	char* rtc_printf_short(char* ptr, char* end);
	//claibrate
	//void smf_calibrate_rtc(uint32_t systime, uint32_t rtc);
	//void smf_calibrate_local_time(uint32_t systime, uint64_t local);
	//irq
	int intr_lock();
	void intr_unlock(int v);
	bool intr_locked();
	//bus
	void bus_lock(void);
	void bus_unlock(void);
	//os
	bool os_lock(void);
	void os_unlock(void);
	//os
	void yield();
	void sleep_for(uint32_t ms);
	void sleep_until(uint64_t ms);
	//int signal_wait(uint32_t mask, uint32_t timeout_ms);
	//int signal_set(uint32_t threadid, uint32_t mask);
	void* get_thread_id();
	int get_thread_idx();
	//cache
	void cache_invalid(void* buff, int size);
	void cache_writeback(void* buff, int size);
	void cache_flush(void* buff, int size);
	void icache_invalid(void* buff, int size);
	int cache_line_length();
	void* address_to_uncache(void*);
	void* address_to_cache(void*);
	//addr & size allgn 64bits
	void* memcpy32(void* dst0, void* src0, int size);
	void* memcpy64(void* dst0, void* src0, int size);
	void* memset64(void* dst0, int ch, int size);
	void* memset64_0(void* dst0, int size);
	void memcpyByCache(void* dst, void* src, int size);
	void memcpyByCache_2(void* dst, void* src, int size);
	//
	void overlay_load(const char* type, const char* keys);
	bool overlay_load(uint32_t id);
	bool overlay_unload_all();
	//
	bool smf_message_init(bool master);
	//bool smf_message_master_init();
	//bool smf_message_master_deinit();
	//bool smf_ipc_open(uint8_t cpuid);
	//bool smf_ipc_close(uint8_t cpuid);
	//
	void* addr_remap_to_dev(const void* addr);
	void* addr_remap_to_cpu(const void* addr);
	//
	bool sco_init();
	bool sco_start(void(*)(uint32_t rx_addr, uint32_t tx_addr, uint32_t len, unsigned char rx_invalid));
	bool sco_stop(void(*)(uint32_t rx_addr, uint32_t tx_addr, uint32_t len, unsigned char rx_invalid));
}
