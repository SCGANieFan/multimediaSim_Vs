#pragma once
#include <stdbool.h>
#include <stdint.h>
#include <stdarg.h>
#define SMF_LIBS_VERSION 20250619

typedef enum {
	SMF_LIBS_LOG_ERRER = 0xa,
	SMF_LIBS_LOG_INFO = 0xb,
	SMF_LIBS_LOG_TEST = 0xc,
}smf_libs_log_chl_e;

typedef enum {
	SMF_LIBS_SETUP_FLAGS_DOWNWARD = 1<<0,
	SMF_LIBS_SETUP_FLAGS_INPUT = 1<<1,
	SMF_LIBS_SETUP_FLAGS_INPUT_REFS = 1<<2,
	SMF_LIBS_SETUP_FLAGS_OUTPUT = 1<<3,
}smf_libs_setup_flags_e;

typedef enum {
	SMF_LIBS_EVENT_SUCESS = 0x10,
}smf_libs_event_e;

typedef struct smf_libs_buffer_t {
	void* addr;
	uint32_t size;
}smf_libs_buffer_t;

typedef struct smf_libs_set_t {
	void* addr;
	uint32_t size;
}smf_libs_set_t;

typedef struct smf_libs_pcm_t{
	uint32_t sample_rate;
	uint8_t sample_bits;
	uint8_t sample_width;
	uint8_t channels;
	bool enable : 1;
	bool interlaced : 1;
	uint8_t revs : 6;
	uint32_t frame_size;
}smf_libs_pcm_t;

typedef struct smf_libs_kips_t{
	uint32_t avg;
	uint32_t min;
	uint32_t max;
}smf_libs_kips_t;

struct smf_libs_t;
typedef bool (*cb_smf_entry)(struct smf_libs_t* libs);
typedef bool (*cb_smf_exit)(struct smf_libs_t* libs);
struct smf_libs_open_t;
typedef void (*cb_smf_libs_log)(uint32_t chl, const char* fmt, va_list* ap);
typedef void (*cb_smf_libs_dump)(uint32_t chl, const void* data, int size);
typedef void (*cb_smf_libs_event)(uint32_t evtid, void* data, uint32_t size, void* pirv);
typedef bool (*cb_smf_libs_get_mem_size)(uint32_t* heap_size, uint32_t* temp_size);
typedef void* (*cb_smf_libs_open)(struct smf_libs_open_t* setup);
typedef bool (*cb_smf_libs_close)(void* handle);
typedef uint32_t(*cb_smf_libs_get_error)(void* handle);
typedef bool (*cb_smf_libs_process)(void* handle
	, void* input, uint32_t input_size
	, void* output, uint32_t output_size
	, void* ref, uint32_t ref_size
	, void* temp_buff, uint32_t temp_size
	);
typedef bool (*cb_smf_libs_set)(void* handle, smf_libs_set_t* para);
typedef bool (*cb_smf_libs_get)(void* handle, smf_libs_set_t* para);

//libsrithm setup
typedef struct smf_libs_open_t {
	uint32_t flags;
	//
	smf_libs_buffer_t heap;
	smf_libs_set_t init;
	//input param
	smf_libs_pcm_t input;
	smf_libs_pcm_t output;
	smf_libs_pcm_t refs;
	//output param
	smf_libs_kips_t kips;
	//others
	uint32_t others[6];
}smf_libs_open_t;

//for dynamic _start()
typedef struct smf_libs_t {
	void* priv;
	uint16_t size;
	uint16_t error;
	uint32_t version;
	//
	cb_smf_libs_log log;
	cb_smf_libs_dump dump;
	cb_smf_libs_event event;
	//
	cb_smf_libs_get_mem_size get_mem_size;
	cb_smf_libs_open open;
	cb_smf_libs_close close;
	cb_smf_libs_get_error get_error;
	cb_smf_libs_process process;
	cb_smf_libs_set set;
	cb_smf_libs_get get;
}smf_libs_t;


#ifndef EXTERNC
#ifndef __cplusplus
#define EXTERNC
#else
#define EXTERNC extern "C"
#endif
#endif
EXTERNC void dbgPrintfc(unsigned chl, const char* fmt, ...);
EXTERNC void smf_libs_event(uint32_t evtid, void* data, uint32_t size, void* priv);

