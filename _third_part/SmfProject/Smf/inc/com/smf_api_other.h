#pragma once
#include "smf_common.h"

EXTERNC void* smf_io_open(const char* fname, const char* para);
EXTERNC int smf_io_read(void* io, void* buff, int size);
EXTERNC int smf_io_write(void* io, void* buff, int size);
EXTERNC int smf_io_seek(void* io, int offset, int origin);
EXTERNC void smf_io_close(void* io);
EXTERNC int smf_io_tell(void* io);

/* sleep for(ms);
 * @param ms[in]: ms
 * @return: void
 */
EXTERNC void smf_sleep_for(uint32_t ms);
EXTERNC uint64_t smf_get_us64(void);

EXTERNC uint32_t smf_hash(const char* str);
EXTERNC uint32_t smf_hashx(const char* str, const char** next);
EXTERNC uint32_t smf_fcc32(const char* str);
EXTERNC uint64_t smf_fcc64(const char* str);

EXTERNC void* smf_addr_to_device(const void* addr);
EXTERNC void* smf_addr_to_cpu(const void* addr);
EXTERNC void* smf_addr_to_cache(const void* addr);
EXTERNC void* smf_addr_to_uncache(const void* addr);
EXTERNC void smf_cache_writeback(void* addr, uint32_t size);
EXTERNC void smf_cache_flush(void* addr, uint32_t size);
EXTERNC void smf_cache_invalid(void* addr, uint32_t size);
