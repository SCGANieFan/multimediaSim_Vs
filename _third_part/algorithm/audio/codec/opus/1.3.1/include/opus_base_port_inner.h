#ifndef __OPUS_BASE_PORT_INNER_H__
#define __OPUS_BASE_PORT_INNER_H__

#include <stdint.h>
#include <stddef.h>
#ifdef __cplusplus
extern "C" {
#endif
void *opus_memset_inner(void* dst0, int c0, size_t length);
void* opus_memcpy_inner(void* dst, const void* src, size_t length);
void* opus_memmove_inner(void* dst, const void* src, size_t length);
#ifdef __cplusplus
}
#endif


#endif /*__OPUS_BASE_PORT_H__ */
