#include "gaapi_base.h"
using namespace ogg_gaapi_ns;

void* GaapiBase_c::operator new(size_t size, void* buf) {
    return buf;
}
void GaapiBase_c::operator delete(void* buf, size_t size) {
    return;
}
void GaapiBase_c::operator delete(void* buf, void* place) {
    return;
}
void GaapiBase_c::operator delete(void* buf) {
    return;
}

namespace ogg_gaapi_ns {
static inline void memset_add64_inner(uint32_t* p32, uint32_t v8, uint32_t length) {
    //assert((dst0&7)==0)
    int32_t lengthFrac = length & 15;
    int32_t lengthInt = length - lengthFrac;
    for (int32_t i = 0; i < lengthInt; i += 16) {
        *p32++ = 0;
        *p32++ = 0;
        *p32++ = 0;
        *p32++ = 0;
    }
    if (lengthFrac) {
        uint8_t* p8 = (uint8_t*)p32;
        for (int32_t i = lengthInt; i < length; i += 1) {
            *p8++ = 0;
        }
    }
}

void* gaapi_memset(void* dst, uint8_t val, int32_t length) {
    uint32_t addRem8 = (uint32_t)dst & 7;
    if (addRem8 == 0) {
        memset_add64_inner((uint32_t*)dst, val, length);
    }
    else {
        uint8_t* p8 = (uint8_t*)dst;
        int32_t pendLen = 8 - addRem8;
        if (length > pendLen) {
            for (int32_t i = 0; i < pendLen; i += 1) {
                *p8++ = val;
            }
            uint32_t* p32 = (uint32_t*)p8;
            length -= pendLen;
            memset_add64_inner(p32, val, length);
        }
        else {
            for (int32_t i = 0; i < length; i += 1) {
                *p8++ = val;
            }
        }
    }
    return 0;
}

void* gaapi_memcpy(void* dst, const void* src, int32_t length) {
    uint32_t addDstRem4 = (uint32_t)dst & 3;
    uint32_t addSrcRem4 = (uint32_t)src & 3;
    if (addDstRem4
        || addSrcRem4) {
        const uint8_t* pSrc8 = (const uint8_t*)src;
        uint8_t* pDst8 = (uint8_t*)dst;
        for (int32_t i = 0; i < length; i++) {
            *pDst8++ = *pSrc8++;
        }
    }
    else {
        const uint32_t* pSrc32 = (const uint32_t*)src;
        uint32_t* pDst32 = (uint32_t*)dst;
        int32_t lengthFrac = length & 15;
        int32_t lengthInt = length - lengthFrac;
        for (int32_t i = 0; i < lengthInt; i += 16) {
            *pDst32++ = *pSrc32++;
            *pDst32++ = *pSrc32++;
            *pDst32++ = *pSrc32++;
            *pDst32++ = *pSrc32++;
        }
        if (lengthFrac) {
            uint8_t* pSrc8 = (uint8_t*)pSrc32;
            uint8_t* pDst8 = (uint8_t*)pDst32;
            for (int32_t i = lengthInt; i < length; i += 1) {
                *pDst8++ = *pSrc8++;
            }
        }
    }
    return 0;
}

static inline void* memcpy_reverse_inner(void* dst, const void* src, int32_t length) {
    uint8_t* pSrc8 = (uint8_t*)src;
    uint8_t* pDst8 = (uint8_t*)dst;
    int32_t diffLen = pDst8 - pSrc8;
    pSrc8 += length - 1;
    pDst8 += length - 1;
    for (int32_t i = 0; i < length; i++) {
        *pDst8-- = *pSrc8--;
    }
    return 0;
}

void* gaapi_memmove(void* dst, const void* src, int32_t length) {
    if (src >= dst) {
        gaapi_memcpy(dst, src, length);
    }
    else {
        memcpy_reverse_inner(dst, src, length);
    }
    return 0;
}

}