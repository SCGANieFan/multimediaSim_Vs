#include "stdint.h"


typedef struct{
   void*(*malloc_cb)(int size);
   void*(*realloc_cb)(void *rmem, int newsize);
   void(*free_cb)(void *buf);
}OpusMemory_t;

void opus_memory_register_inner(OpusMemory_t *memory);
void *opus_malloc_inner(uint32_t size);
void *opus_realloc_inner(void *rmem, uint32_t newsize);
void *opus_calloc_inner(uint32_t count, uint32_t size);
void opus_free_inner(void *rmem);
