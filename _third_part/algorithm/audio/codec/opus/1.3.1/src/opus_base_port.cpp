#if 1
#include "opus_base_port.h"

#if WIN32
#include<stdio.h>
#define LOG(fmt,...) printf("[%s](%d)" fmt "\n",__func__, __LINE__, ##__VA_ARGS__)
char* global_stack_max = 0;
char* global_stack_ori = 0;
char* global_stack_log = 0;
#else
#define LOG(fmt,...)
#endif


#ifdef __cplusplus
extern "C" {
#endif
static inline void opus_memset_add64_inner(uint32_t* p32, uint32_t v8, size_t length) {
    //assert((dst0&7)==0)
#if 0
    uint32_t v32 = (v8 << 24) | (v8 << 16) || (v8 << 8) || v8;
    size_t lengthFrac = length & 15;
    size_t lengthInt = length - lengthFrac;
    for (size_t i = 0; i < lengthInt; i += 16) {
        *p32++ = v32;
        *p32++ = v32;
        *p32++ = v32;
        *p32++ = v32;
    }
    if (lengthFrac) {
        uint8_t* p8 = (uint8_t*)p32;
        for (size_t i = lengthInt; i < length; i += 1) {
            *p8++ = v8;
        }
    }
#else
    size_t lengthFrac = length & 15;
    size_t lengthInt = length - lengthFrac;
    for (size_t i = 0; i < lengthInt; i += 16) {
        *p32++ = 0;
        *p32++ = 0;
        *p32++ = 0;
        *p32++ = 0;
    }
    if (lengthFrac) {
        uint8_t* p8 = (uint8_t*)p32;
        for (size_t i = lengthInt; i < length; i += 1) {
            *p8++ = 0;
        }
    }
#endif
}

void* opus_memset_inner(void* dst0, int c0, size_t length){
    uint32_t addRem8 = (uint32_t)dst0 & 7;
    if(addRem8 == 0){
        //{static int num = 0; LOG("%d",num++); }
        opus_memset_add64_inner((uint32_t*)dst0, c0, length);
    }else{
        uint8_t*p8 = (uint8_t*)dst0;
        uint32_t pendLen = 8 - addRem8;
        if (length > pendLen) {
            //{static int num = 0; LOG("%d", num++); }
            for (uint32_t i = 0; i < pendLen; i += 1) {
                *p8++ = c0;
            }
            uint32_t* p32 = (uint32_t*)p8;
            length -= pendLen;
            opus_memset_add64_inner(p32, c0, length);
        }
        else {
            //{static int num = 0; LOG("%d", num++); }
            for (size_t i = 0; i < length; i += 1) {
                *p8++ = c0;
            }
        }
    }
    return 0;
}

void* opus_memcpy_inner(void* dst, const void* src, size_t length){
    uint32_t addDstRem4 = (uint32_t)dst & 3;
    uint32_t addSrcRem4 = (uint32_t)src & 3;
    if (addDstRem4
        || addSrcRem4) {
        //{static int num = 0; LOG("%d",num++); }
        uint8_t* pSrc8 = (uint8_t*)src;
        uint8_t* pDst8 = (uint8_t*)dst;
        for (size_t i = 0; i < length; i++) {
            *pDst8++ = *pSrc8++;
        }
    }
    else {
        //{static int num = 0; LOG("%d", num++); }
        uint32_t* pSrc32 = (uint32_t*)src;
        uint32_t* pDst32 = (uint32_t*)dst;
        size_t lengthFrac = length & 15;
        size_t lengthInt = length - lengthFrac;
        for (size_t i = 0; i < lengthInt; i+=16) {
            *pDst32++ = *pSrc32++;
            *pDst32++ = *pSrc32++;
            *pDst32++ = *pSrc32++;
            *pDst32++ = *pSrc32++;
        }
        if (lengthFrac) {
            //{static int num = 0; LOG("%d", num++); }
            uint8_t* pSrc8 = (uint8_t*)pSrc32;
            uint8_t* pDst8 = (uint8_t*)pDst32;
            for (size_t i = lengthInt; i < length; i += 1) {
                *pDst8++ = *pSrc8++;
            }
        }
    }
    return 0;
}

static inline void* opus_memcpy_reverse_inner(void* dst, const void* src, size_t length) {
    uint8_t* pSrc8 = (uint8_t*)src;
    uint8_t* pDst8 = (uint8_t*)dst;
    int32_t diffLen = pDst8 - pSrc8;
    pSrc8 += length - 1;
    pDst8 += length - 1;
    for (size_t i = 0; i < length; i++) {
        *pDst8-- = *pSrc8--;
    }
    return 0;
}

void* opus_memmove_inner(void* dst, const void* src, size_t length){
    if (src >= dst) {
        //{static int num = 0; LOG("%d", num++); }
        opus_memcpy_inner(dst, src, length);
    }
    else {
        //{static int num = 0; LOG("%d",num++); }
        opus_memcpy_reverse_inner(dst, src, length);
    }
    return 0;
}

#if 0
void* opus_malloc_inner(uint32_t size)
{
    {static int num = 0; LOG("%d",num++); }
    return opusBasePort.malloc_cb(size);
}

void* opus_realloc_inner(void* rmem, uint32_t newsize)
{
    {static int num = 0; LOG("%d",num++); }
    return opusBasePort.realloc_cb(rmem, newsize);
}

void* opus_calloc_inner(uint32_t count, uint32_t size)
{
    {static int num = 0; LOG("%d",num++); }
    void* ptr = opus_malloc_inner(count * size);
    if (ptr != NULL)
        opus_memset_inner(ptr, 0, count * size);
    return ptr;
}

void opus_free_inner(void* buf)
{
    {static int num = 0; LOG("%d",num++); }
    opusBasePort.free_cb(buf);
}
#endif
#ifdef __cplusplus
}
#endif


#endif