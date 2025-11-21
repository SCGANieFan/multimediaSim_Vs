#include <stdio.h>
#include <stdlib.h>
#include "heap_api.h"

multi_heap_handle_t heap_register(char* buf, int size) {
    return 0;
}

void *heap_malloc(multi_heap_handle_t heap,int size){
    return malloc(size);
}

void *heap_realloc(multi_heap_handle_t heap,void* rmem, int newsize){
    return realloc(rmem,newsize);
}

void heap_free(multi_heap_handle_t heap, void* rmem) {
    free(rmem);
}
