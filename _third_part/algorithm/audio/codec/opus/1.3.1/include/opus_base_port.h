#ifndef __OPUS_BASE_PORT_H__
#define __OPUS_BASE_PORT_H__

#include <stdint.h>
#include <stddef.h>
//#ifdef __cplusplus
//extern "C" {
//#endif
	typedef struct {
		void* (*malloc_cb)(int size);
		void* (*realloc_cb)(void* rmem, int newsize);
		void(*free_cb)(void* buf);
	}OpusBasePort_t;
//#ifdef __cplusplus
//}
//#endif

#endif /*__OPUS_BASE_PORT_H__ */
