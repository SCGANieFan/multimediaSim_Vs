#pragma once

typedef void* multi_heap_handle_t;

multi_heap_handle_t heap_register(char* buf, int size);
void* heap_malloc(multi_heap_handle_t heap, int size);
void* heap_realloc(multi_heap_handle_t heap, void* rmem, int newsize);
void heap_free(multi_heap_handle_t heap, void* rmem);


