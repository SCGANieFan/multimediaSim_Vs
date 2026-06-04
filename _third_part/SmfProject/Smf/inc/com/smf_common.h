#pragma once
#ifndef __SMF_COMMON_H__
#define __SMF_COMMON_H__
#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#ifndef EXTERNC
#ifndef __cplusplus
#define EXTERNC
#else
#define EXTERNC extern "C"
#endif
#endif

#ifdef WIN32
#define WEAK
#else
#define WEAK __attribute__((weak))
#endif

#define TO_STR_(s) # s
#define TO_STR(s)  TO_STR_(s)

//#ifdef __cplusplus
//#if __cplusplus < 201103L
//#define override
//#define final
//#endif
//#endif
typedef enum smf_direction_e {
	smf_direction_upward=0,
	smf_direction_downward,
	smf_direction_forward,
	smf_direction_backward,
}smf_direction_e;

typedef struct smf_message_t {
	uint64_t id;
	uint32_t data[4];
	//
	uint16_t sub;
	bool is_processed : 1;
	bool is_notify : 1;
	uint8_t direction : 2;//smf_direction_e
	uint8_t is_error : 1;
	uint8_t is_response : 1;
	uint8_t flags : 2;
	uint8_t result;
	//
	void* priv;
	void* creater;
	void* laster;
}smf_message_t;
typedef bool(*cb_message_t)(smf_message_t* msg);

typedef struct smf_progress_t {
	uint32_t start;
	uint32_t current;
	uint32_t end;
}smf_progress_t;
//
typedef struct {
	uint32_t playtime;
	uint32_t frameindex;
	uint32_t position;
}smf_stream_status_t;

typedef struct smf_buffer_t {
	uint16_t max;
	uint16_t offset;
	uint16_t size;
	uint8_t falgs;
	uint8_t refs;
	void* buff;
}smf_buffer_t;
//
typedef struct {
	void* data;
	unsigned size;
}smf_pair_t;

typedef struct {
	uint16_t data;
	uint16_t size;
}smf_pair16_t;

typedef struct {
	uint32_t data;
	uint32_t size;
}smf_pair32_t;
//
typedef struct {
	void* func;
	void* priv;
}smf_callback_t;
//
typedef struct {
	unsigned keys;
	void* data;
	unsigned size;
	void* priv;
	void (*uninit)(void*priv);
	void* other;
}smf_param_t;
//
typedef struct {
	unsigned keys;
	unsigned vals;
}smf_keys_value_t;
//
typedef union {
	int8_t i8[4];
	int16_t i16[2];
	int32_t i32;
	uint8_t u8[4];
	uint16_t u16[2];
	uint32_t u32;
}smf_int8x4_t;
//
typedef union {
	int8_t  i8[8];
	int16_t i16[4];
	int32_t i32[2];
	int64_t i64;
	uint8_t u8[8];
	uint16_t u16[4];
	uint32_t u32[2];
	uint64_t u64;
}smf_int8x8_t;

typedef struct smf_fifo_t {
	uint64_t data;
	uint64_t wi;
	uint64_t ri;
	uint32_t max;
	uint32_t timestamp;
}smf_fifo_t, smf_ring_t;

typedef struct {
	uint8_t* begin;
	uint8_t* end;
	uint8_t* ptr;
	uint8_t* ptr_end;
}smf_shared_pool_t;

typedef struct {
	uint64_t ptr;
	int64_t oft;
	uint32_t size;
	uint32_t where;
}smf_io_read_para_t, smf_io_write_para_t;

typedef void (*smf_hook_cb)(uint32_t pointer, void* data, uint32_t size, void* priv);
typedef void (*smf_hook_cb3)(void* data, uint32_t size, void* priv);
typedef struct {
	char path[32];
}smf_hook_t;

typedef struct {
	int num;
	int den;
}smf_fraction_t;

typedef struct {
	uint32_t keys;
	void* data;
	uint32_t size;
	union {
		uint32_t flags;
		struct {
			bool enable : 1;
			bool is_static : 1;
			bool no_cache : 1;
			bool pass_val : 1;
			bool is_save : 1;
			bool is_free : 1;
		};
	};
}smf_params_t;

typedef struct {
	uint32_t freq_mhz;
	int avg_mips;
	int max_mips;
	int min_mips;
}smf_mips_t;

typedef struct {
	uint32_t res;
	void* data;
	uint32_t size;
}smf_wakeup_t;

typedef struct {
	void* data;
	uint32_t size;
}smf_extra_t;

typedef struct {
	int result;
	uint32_t data;
	uint32_t size;
}smf_vad_t;

typedef struct {
	union {
		uint64_t halName;
		uint8_t halId;
	};
	uint16_t blksize;
	uint8_t blknum;
	uint8_t blknumStart;
	uint32_t chmap;
	uint16_t fsCycles;
	uint8_t slotCycles;
	struct {
		bool master : 1;
		bool setId : 1;
		uint8_t rev0 : 6;
	};
	uint32_t fsize;
	uint8_t vol;
	uint32_t rev1:24;
	uint32_t rev;
}smf_dma_param_t; // 32byte

typedef struct {
	uint32_t idx;
}smf_frameindex_t;

typedef struct {
	uint32_t ms;
}smf_timepoint32_t;

typedef struct {
	int32_t ms;
}smf_duration32_t;

typedef struct {
	uint64_t us;
}smf_timepoint64_t;

typedef struct {
	int64_t us;
}smf_duration64_t;

typedef struct {
	int8_t val;
}smf_percentage_t;

typedef struct smf_region_t {
	uint32_t min;
	uint32_t current;
	uint32_t max;
}smf_region_t;

typedef struct smf_value_t {
	uint32_t avg;
	uint32_t min;
	uint32_t max;
	uint32_t cnt;
}smf_value_t;

struct smf_media_def_t;
typedef struct {
	const char* url;
	smf_pair_t title;
	smf_pair_t artist;
	smf_pair_t album;
	struct smf_media_def_t* media;
	smf_progress_t* timepoint;
	smf_progress_t* frameindex;
}smf_meta_info_t;

struct smf_frame_t;
struct smf_message_t;
typedef bool (*smf_cb_process)(void* priv, struct smf_frame_t*ifrm, struct smf_frame_t*ofrm);
typedef bool (*smf_cb_input)(void* priv, struct smf_frame_t*frm);
typedef bool (*smf_cb_output)(void* priv, struct smf_frame_t*frm);
typedef bool (*smf_cb_proc)(void* priv, void* data, uint32_t size);
typedef bool (*smf_cb_open)(void* priv, void* object, struct smf_media_def_t* media, void* para);
typedef bool (*smf_cb_close)(void* priv);
typedef bool (*smf_cb_start)(void* priv, void* object, struct smf_media_def_t* media);
typedef bool (*smf_cb_stop)(void* priv);
typedef bool (*smf_cb_pause)(void* priv);
typedef bool (*smf_cb_resume)(void* priv);
typedef bool (*smf_cb_set)(void* priv, uint32_t key, void* val);
typedef bool (*smf_cb_get)(void* priv, uint32_t key, void* val);
typedef bool (*smf_cb_msg)(void* priv, struct smf_message_t* msg);
typedef void (*smf_cb_entry)(int argc, char** argv);
typedef void (*smf_cb_data)(void* priv, uint32_t keys, void* data, int size);
typedef bool (*smf_cb_para)(void* para);
typedef bool (*smf_cb_para2)(void* priv, void* para);
typedef bool (*smf_cb_priv)(void* priv);
typedef void (*smf_cb_priv0)(void* priv);
typedef bool (*smf_cb_name2)(void* priv, const char* name);
typedef void* (*smf_cb_malloc)(uint32_t);
typedef void* (*smf_cb_realloc)(void*, uint32_t);
typedef bool (*smf_cb_hold)(void*);
typedef bool (*smf_cb_free)(void*);
typedef uint32_t (*smf_cb_getsize)(void);
typedef uint32_t (*smf_cb_gettotal)(void);

typedef struct smf_cb_element_t {
	void* priv;
	smf_cb_open open;
	smf_cb_close close;
	smf_cb_start start;
	smf_cb_stop stop;
	smf_cb_pause pause;
	smf_cb_resume resume;
	smf_cb_set set;
	smf_cb_get get;
	smf_cb_msg msg;
	smf_cb_process process;
	smf_cb_proc proc;
	smf_cb_input input;
	smf_cb_output output;
}smf_cb_element_t;

typedef struct smf_msg_eptname_cpu_t {
	const char* ept_name;
	const char* cpu_name;
	uint8_t cpuid;
	uint8_t master;
}smf_msg_eptname_cpu_t;

typedef struct smf_display_t {
	uint32_t addr;
	uint32_t size;
	uint16_t width;
	uint16_t height;
	uint8_t format;
	int8_t refs;
	uint8_t index;
	uint8_t lock;
}smf_display_t;
#endif
