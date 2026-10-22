#pragma once
#include <stdio.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdarg.h>
#ifndef EXTERNC
#ifndef __cplusplus
#define EXTERNC
#else
#define EXTERNC extern "C"
#endif
#endif

#define NORMAL_ALGO_VERSION 20240308

typedef enum {
	NORMAL_ALGO_LOG_CHL_ERRER = 0xa,
	NORMAL_ALGO_LOG_CHL_INFO = 0xb,
	NORMAL_ALGO_LOG_CHL_TEST = 0xc,
}normal_algo_log_chl_e;

//algorithm setup
struct normal_algo_setup_t {
	void (*log)(uint32_t chl, const char* fmt, ...);
	void (*event)(void* msg, uint32_t size, void* event_pirv);
	void* event_priv;
	void* adapter;
	//input param
	uint32_t sample_rate;
	uint8_t sample_bits;
	uint8_t channels;
	bool interlaced;
	uint32_t frame_size;
	//ref param
	uint32_t ref_sample_rate;
	uint8_t ref_sample_bits;
	uint8_t ref_channels;
	bool ref_interlaced;
	uint32_t ref_frame_size;
	//output param
	uint32_t output_sample_rate;
	uint8_t output_sample_bits;
	uint8_t output_channels;
	bool output_interlaced;
	uint32_t output_frame_size;
	//others
	uint32_t others[6];
	void (*logp)(unsigned chl, const char* fmt, va_list* ap);
};

struct normal_algo_set_t {
	//set
	uint32_t keys;
	void *data;
	uint32_t size;
};

//for dynamic _start()
typedef struct {
	uint16_t size;
	uint16_t error;
	uint32_t version;
	//
	void (*log)(uint32_t chl, const char* fmt, ...);
	//
	bool (*get_mem_size)(uint32_t* handle_size, uint32_t* temp_size);
	bool (*open)(void* handle, void* setup);
	bool (*close)(void* handle);
	uint32_t(*get_error)(void* handle);
	bool (*process)(void* handle, void* input, uint32_t input_size, void* output, uint32_t output_size, void* temp_buff, uint32_t temp_size);
	bool (*process_x)(void* handle, void* data, uint32_t data_size, void* temp_buff, uint32_t temp_size);
	bool (*process_r)(void* handle, void* input, uint32_t input_size, void* ref, uint32_t ref_size, void* output, uint32_t output_size, void* temp_buff, uint32_t temp_size);
	bool (*set)(void* handle, uint32_t key, void* cfg_data, uint32_t cfg_len);
	bool (*get)(void* handle, uint32_t key, void* cfg_data, uint32_t cfg_len);
	bool (*get_kips)(void* handle, uint32_t* avg_kips, uint32_t* min_kips, uint32_t* max_kips);
}normal_algo_dl_t;

 /** get algorithm memroy request(Byte).
 *  @param sizes[out]:get handle/temp buffer size.
 *  @return true/false
 */
EXTERNC bool normal_algo_get_mem_size(uint32_t* handle_size,uint32_t* temp_size);

 /** open algorithm
 *  @param handle[in]: algorithm handle
 *  @param setup[in]: algorithm config
 *  @return tre/false
 */
EXTERNC bool normal_algo_open(void* handle, void* setup);

 /** close algorithm
 *  @param handle[in]: algorithm handle
 *  @return true/false
 */
EXTERNC bool normal_algo_close(void* handle);

 /** check error
 *  @param handle[in]: algorithm handle
 *  @return error id
 */
EXTERNC uint32_t normal_algo_get_error(void* handle);

 /** process data
*  @multiram hdl[in]: multi algorithm handle
*  @multiram input[in]: input addr
*  @multiram input_size[in]: input buff size
*  @multiram input[in]: output addr
*  @multiram input_size[in]: output buff size
*  @multiram temp_buff[in]: temp addr
*  @multiram temp_size[in]: temp buff size
*  @return true/false
*/
EXTERNC bool normal_algo_process(void* handle, void* input, uint32_t input_size, void* output, uint32_t output_size, void* temp_buff, uint32_t temp_size);

/** process data
*  @multiram hdl[in]: multi algorithm handle
*  @multiram input[in]: input addr
*  @multiram input_size[in]: input buff size
*  @multiram temp_buff[in]: temp addr
*  @multiram temp_size[in]: temp buff size
*  @return true/false
*/
EXTERNC bool normal_algo_process_x(void* handle, void* data, uint32_t size, void* temp_buff, uint32_t temp_size);

/** process data
*  @multiram hdl[in]: multi algorithm handle
*  @multiram input[in]: input addr
*  @multiram input_size[in]: input buff size
*  @multiram ref[in]: ref addr
*  @multiram ref_size[in]: ref buff size
*  @multiram temp_buff[in]: temp addr
*  @multiram temp_size[in]: temp buff size
*  @return true/false
*/
EXTERNC bool normal_algo_process_r(void* handle, void* input, uint32_t input_size, void* ref, uint32_t ref_size, void* output, uint32_t output_size, void* temp_buff, uint32_t temp_size);

/** set algorithm value
 *  @param handle[in]: algorithm handle
 *  @param key[in]: key
 *  @param cfg_data[in]: value pointer
 *  @param cfg_len[in]: value length
 *  @return true/false
 */
EXTERNC bool normal_algo_set(void* handle, uint32_t key, void* cfg_data, uint32_t cfg_len);

/** get algorithm value
 *  @param handle[in]: algorithm handle
 *  @param key[in]: key
 *  @param cfg_data[in]: value pointer
 *  @param cfg_len[in]: value length
 *  @return true/false
 */
EXTERNC bool normal_algo_get(void* handle, uint32_t key, void* cfg_data, uint32_t cfg_len);

/** get algorithm process kips(instructions count per millisecond)
 *  @param avg_kips[out]: average kips
 *  @param min_kips[out]: min kips
 *  @param max_kips[out]: max kips
 *  @return true/false
 */
EXTERNC bool normal_algo_get_kips(void* handle, uint32_t* avg_kips, uint32_t* min_kips, uint32_t* max_kips);

//
EXTERNC void* malloc(size_t size);
EXTERNC void* calloc(size_t nmemb, size_t size);
EXTERNC void* realloc(void* rmem, size_t newsize);
EXTERNC void free(void* rmem);
EXTERNC void printf_error(const char* fmt, ...);
EXTERNC void printf_info(const char* fmt, ...);
EXTERNC void printf_debug(const char* fmt, ...);
EXTERNC void send_event(void* msg, uint32_t size, void* event_pirv);
