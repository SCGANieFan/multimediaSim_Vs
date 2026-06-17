#pragma once
#ifndef __SMF_API_POOL_H__
#define __SMF_API_POOL_H__
#include "smf_common.h"

EXTERNC void smf_std_pool_register(void);

/** print memory pool information.
 */
EXTERNC void smf_pool_print(void);

/**/
EXTERNC void* smf_register_pool(const char* name, void* buff, unsigned size, unsigned thr);

/**/
EXTERNC void* smf_register_pool_alias(const char* alias, const char* name);

/**/
EXTERNC void* smf_register_pool_text(const char* name, void* buff, unsigned size);

/**/
EXTERNC void* smf_register_pool_callback(const char* name,
	void* (*cbAlloc)(uint32_t size), void(*cbFree)(void* ptr),
	uint32_t(*cbGetSize)(void), uint32_t(*cbGetTotal)(void));

/**/
EXTERNC void* smf_register_globle_pool(void* buff, int size);
#define smf_register_globle_pool_with_buffer(buff, size) smf_register_globle_pool(buff, size)

/**/
EXTERNC void smf_designate_globle_pool(const char* name);

/**/
EXTERNC void smf_register_backup_pool(const char* name, const char* bkpool);

/**
 * @brief alloc a buffer from gbl pool, and convert to uncahce address.
 * @param size[in]: the buff size
 * @return the addr of buffer.
*/
EXTERNC void* smf_malloc_uncache(uint32_t size);

/**
 * @brief free a uncache address with gbl pool
 * @param addr[in]: the uncache addr 
 * @return true/false
*/
EXTERNC bool smf_free_uncache(void* addr);

/**/
EXTERNC void* smf_malloc(uint32_t size);
EXTERNC void* smf_realloc(void* ptr, uint32_t size);
EXTERNC void* smf_calloc(uint32_t num, uint32_t size);
EXTERNC void smf_free(void* ptr);

/**/
EXTERNC void* smf_find_malloc(const char* pool, int size);
EXTERNC void* smf_find_realloc(const char* pool, void* ptr, int size);
EXTERNC void* smf_find_calloc(const char* pool, int count, int size);
EXTERNC void smf_find_free(const char* pool, void* ptr);

/**/
EXTERNC void* smf_pool_get(const char* plname);
EXTERNC void* smf_pool_malloc(void* pool, int size);
EXTERNC void* smf_pool_realloc(void* pool, void* ptr, int size);
EXTERNC void* smf_pool_calloc(void* pool, int count, int size);
EXTERNC void smf_pool_free(void* pool, void* ptr);

/**/
EXTERNC void* smf_debug_malloc(void* pool, int size, const char* file, int line);
EXTERNC void* smf_debug_realloc(void* pool, void* ptr, int size, const char* file, int line);
EXTERNC void* smf_debug_calloc(void* pool, int count, int size, const char* file, int line);
EXTERNC void smf_debug_free(void* pool, void* ptr, const char* file, int line);
#define smf_dbg_malloc(pool,size) smf_debug_malloc(pool,size,__FILE__,__LINE__)
#define smf_dbg_realloc(pool,ptr,size) smf_debug_realloc(pool,ptr,size,__FILE__,__LINE__)
#define smf_dbg_calloc(pool,num,size) smf_debug_calloc(pool,num,size,__FILE__,__LINE__)
#define smf_dbg_free(pool,ptr) smf_debug_free(pool,ptr,__FILE__,__LINE__)

/**/
EXTERNC void* smf_find_pool_alloc(const char* plname, uint32_t size);
EXTERNC void smf_find_pool_free(const char* plname, void* ptr);
#endif
