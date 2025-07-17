#include "plc_base.h"
static inline void memset_add64_inner(u32* p32, u32 v8, i32 length) {
    //assert((dst0&7)==0)
#if 0
    u32 v32 = (v8 << 24) | (v8 << 16) || (v8 << 8) || v8;
    i32 lengthFrac = length & 15;
    i32 lengthInt = length - lengthFrac;
    for (i32 i = 0; i < lengthInt; i += 16) {
        *p32++ = v32;
        *p32++ = v32;
        *p32++ = v32;
        *p32++ = v32;
    }
    if (lengthFrac) {
        u8* p8 = (u8*)p32;
        for (i32 i = lengthInt; i < length; i += 1) {
            *p8++ = v8;
        }
    }
#else
    i32 lengthFrac = length & 15;
    i32 lengthInt = length - lengthFrac;
    for (i32 i = 0; i < lengthInt; i += 16) {
        *p32++ = 0;
        *p32++ = 0;
        *p32++ = 0;
        *p32++ = 0;
    }
    if (lengthFrac) {
        u8* p8 = (u8*)p32;
        for (i32 i = lengthInt; i < length; i += 1) {
            *p8++ = 0;
        }
    }
#endif
}

void* memset_m(void* dst, u8 val, i32 length) {
    u32 addRem8 = (u32)dst & 7;
    if (addRem8 == 0) {
        memset_add64_inner((u32*)dst, val, length);
    }
    else {
        u8* p8 = (u8*)dst;
        i32 pendLen = 8 - addRem8;
        if (length > pendLen) {
            for (i32 i = 0; i < pendLen; i += 1) {
                *p8++ = val;
            }
            u32* p32 = (u32*)p8;
            length -= pendLen;
            memset_add64_inner(p32, val, length);
        }
        else {
            for (i32 i = 0; i < length; i += 1) {
                *p8++ = val;
            }
        }
    }
    return 0;
}

void* memcpy_m(void* dst, const void* src, i32 length) {
    u32 addDstRem4 = (u32)dst & 3;
    u32 addSrcRem4 = (u32)src & 3;
    if (addDstRem4
        || addSrcRem4) {
        const u8* pSrc8 = (const u8*)src;
        u8* pDst8 = (u8*)dst;
        for (i32 i = 0; i < length; i++) {
            *pDst8++ = *pSrc8++;
        }
    }
    else {
        const u32* pSrc32 = (const u32*)src;
        u32* pDst32 = (u32*)dst;
        i32 lengthFrac = length & 15;
        i32 lengthInt = length - lengthFrac;
        for (i32 i = 0; i < lengthInt; i += 16) {
            *pDst32++ = *pSrc32++;
            *pDst32++ = *pSrc32++;
            *pDst32++ = *pSrc32++;
            *pDst32++ = *pSrc32++;
        }
        if (lengthFrac) {
            u8* pSrc8 = (u8*)pSrc32;
            u8* pDst8 = (u8*)pDst32;
            for (i32 i = lengthInt; i < length; i += 1) {
                *pDst8++ = *pSrc8++;
            }
        }
    }
    return 0;
}

static inline void* memcpy_reverse_inner(void* dst, const void* src, i32 length) {
    u8* pSrc8 = (u8*)src;
    u8* pDst8 = (u8*)dst;
    i32 diffLen = pDst8 - pSrc8;
    pSrc8 += length - 1;
    pDst8 += length - 1;
    for (i32 i = 0; i < length; i++) {
        *pDst8-- = *pSrc8--;
    }
    return 0;
}

void* memmove_m(void* dst, const void* src, i32 length) {
    if (src >= dst) {
        memcpy_m(dst, src, length);
    }
    else {
        memcpy_reverse_inner(dst, src, length);
    }
    return 0;
}
