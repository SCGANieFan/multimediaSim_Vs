#pragma once
#include <stdint.h>
#include <stddef.h>
#include <assert.h>

namespace plc_gaapi_ns {

#define GAAPI_RET_SUCCESS        (0)
#define GAAPI_RET_FAIL           (-1)

#if WIN32
#define LOG_GAAPI(fmt,...) if(_bp.print_cb) _bp.print_cb("<%s>[%s](%d)" fmt "\n", Strrchr_m(__FILE__,'\\') + 1,__func__, __LINE__, ##__VA_ARGS__)
#else
#define LOG_GAAPI(fmt,...) if(_bp.print_cb) _bp.print_cb("<%s>[%s](%d)" fmt "\n", Strrchr_m(__FILE__,'/') + 1,__func__, __LINE__, ##__VA_ARGS__)
#endif


#ifndef EXTERNC
#ifndef __cplusplus
#define EXTERNC
#else
#define EXTERNC extern "C"
#endif
#endif

#define STATIC static
#define INLINE inline

#define ALIGEN8(x) (((x+7)>>3)<<3)

#define MAX(a,b) ((a)>(b)?(a):(b))
#define MIN(a,b) ((a)>(b)?(b):(a))

#define ASSERT(s) assert(s)


typedef unsigned char u8;
typedef unsigned short u16;
typedef unsigned int u32;
typedef unsigned long long u64;

typedef char i8;
typedef short i16;
typedef int i32;
typedef long long i64;

struct i24 {
    u8 x[3];
    i24(i32 in) { *this = in; }
    inline i32 operator+(i24& in) { return (i32)*this + (i32)in; }
    inline i32 operator<<(i32 shf) { return (i32)*this << shf; }
    inline i32 operator>>(i32 shf) { return (i32)*this >> shf; }
    inline i32 operator<<=(i32 shf) { return *this = (i32)*this << shf; }
    inline i32 operator>>=(i32 shf) { return *this = (i32)*this >> shf; }
    inline i32 operator=(i32 in) { x[0] = ((u8*)&in)[0]; x[1] = ((u8*)&in)[1]; x[2] = ((u8*)&in)[2]; return in; }
    inline i32 operator=(i24& in) { x[0] = in.x[0]; x[1] = in.x[1]; x[2] = in.x[2]; return in; }
    inline operator i32()const { return (i32)((x[2] << 24) | (x[1] << 16) | (x[0] << 8)) >> 8; }
};

struct u24 {
    u8 x[3];
    u24(u32 in) { *this = in; }
    inline u32 operator+(u24& in) { return (u32)*this + (u32)in; }
    inline u32 operator<<(i32 shf) { return (u32)*this << shf; }
    inline u32 operator>>(i32 shf) { return (u32)*this >> shf; }
    inline u32 operator<<=(i32 shf) { return *this = (u32)*this << shf; }
    inline u32 operator>>=(i32 shf) { return *this = (u32)*this >> shf; }
    inline u32 operator=(u32 in) { x[0] = ((u8*)&in)[0]; x[1] = ((u8*)&in)[1]; x[2] = ((u8*)&in)[2]; return in; }
    inline u32 operator=(u24& in) { x[0] = in.x[0]; x[1] = in.x[1]; x[2] = in.x[2]; return in; }
    inline operator u32()const { return (u32)((x[2] << 24) | (x[1] << 16) | (x[0] << 8)) >> 8; }
};

typedef float f32;
typedef double f64;
typedef bool b1;





STATIC INLINE b1 IsFloatNan(f32 v) {
    u32* intPtr = reinterpret_cast<u32*>(&v);
    u32 intValue = *intPtr;
    const u32 expMask = 0x7F800000;
    const u32 fracMask = 0x007FFFFF;
    return (intValue & expMask) == expMask && (intValue & fracMask) != 0;
}
STATIC INLINE b1 IsFloatInf(f32 v) {
    u32* intPtr = reinterpret_cast<u32*>(&v);
    u32 intValue = *intPtr;
    const u32 expMask = 0x7F800000;
    const u32 fracMask = 0x007FFFFF;
    return (intValue & expMask) == expMask && (intValue & fracMask) == 0;
}
STATIC INLINE b1 IsFloatValid(f32 v) {
    return !(IsFloatInf(v) || IsFloatNan(v));
}
template<class T>
INLINE constexpr i32 Width() { return 0; }
template<> INLINE constexpr i32 Width<b1>() { return 0; }
template<> INLINE constexpr i32 Width<u8>() { return 1; }
template<> INLINE constexpr i32 Width<i8>() { return 1; }
template<> INLINE constexpr i32 Width<u16>() { return 2; }
template<> INLINE constexpr i32 Width<i16>() { return 2; }
template<> INLINE constexpr i32 Width<u24>() { return 3; }
template<> INLINE constexpr i32 Width<i24>() { return 3; }
template<> INLINE constexpr i32 Width<u32>() { return 4; }
template<> INLINE constexpr i32 Width<i32>() { return 4; }
template<> INLINE constexpr i32 Width<f32>() { return 4; }
template<> INLINE constexpr i32 Width<u64>() { return 8; }
template<> INLINE constexpr i32 Width<i64>() { return 8; }
template<> INLINE constexpr i32 Width<f64>() { return 8; }

template<class T>
INLINE constexpr bool IsF64() { return false; }
template<> INLINE constexpr bool IsF64<f64>() { return true; }

template<class T>
INLINE constexpr bool IsF32() { return false; }
template<> INLINE constexpr bool IsF32<f32>() { return true; }

template<class T>
INLINE constexpr bool IsFloat() { return false; }
template<> INLINE constexpr bool IsFloat<f32>() { return true; }
template<> INLINE constexpr bool IsFloat<f64>() { return true; }

template<class T>
INLINE constexpr bool IsI64() { return false; }
template<> INLINE constexpr bool IsI64<i64>() { return true; }

template<class T>
INLINE constexpr bool IsI32() { return false; }
template<> INLINE constexpr bool IsI32<i32>() { return true; }

template<class T>
INLINE constexpr bool IsI24() { return false; }
template<> INLINE constexpr bool IsI24<i24>() { return true; }

template<class T>
INLINE constexpr bool IsI16() { return false; }
template<> INLINE constexpr bool IsI16<i16>() { return true; }

template<class T>
INLINE constexpr bool IsI8() { return false; }
template<> INLINE constexpr bool IsI8<i8>() { return true; }

template<class T>
INLINE constexpr bool IsU64() { return false; }
template<> INLINE constexpr bool IsU64<u64>() { return true; }

template<class T>
INLINE constexpr bool IsU32() { return false; }
template<> INLINE constexpr bool IsU32<u32>() { return true; }

template<class T>
INLINE constexpr bool IsU24() { return false; }
template<> INLINE constexpr bool IsU24<u24>() { return true; }

template<class T>
INLINE constexpr bool IsU16() { return false; }
template<> INLINE constexpr bool IsU16<u16>() { return true; }

template<class T>
INLINE constexpr bool IsU8() { return false; }
template<> INLINE constexpr bool IsU8<u8>() { return true; }

template<class T>
INLINE constexpr bool IsB1() { return false; }
template<> INLINE constexpr bool IsB1<b1>() { return true; }

template<class T>
INLINE constexpr bool IsWidth64() { return false; }
template<> INLINE constexpr bool IsWidth64<f64>() { return true; }
template<> INLINE constexpr bool IsWidth64<i64>() { return true; }
template<> INLINE constexpr bool IsWidth64<u64>() { return true; }

template<class T>
INLINE constexpr bool IsWidth32() { return false; }
template<> INLINE constexpr bool IsWidth32<f32>() { return true; }
template<> INLINE constexpr bool IsWidth32<i32>() { return true; }
template<> INLINE constexpr bool IsWidth32<u32>() { return true; }

template<class T>
INLINE constexpr bool IsWidth24() { return false; }
template<> INLINE constexpr bool IsWidth24<i24>() { return true; }
template<> INLINE constexpr bool IsWidth24<u24>() { return true; }

template<class T>
INLINE constexpr bool IsWidth16() { return false; }
template<> INLINE constexpr bool IsWidth16<i16>() { return true; }
template<> INLINE constexpr bool IsWidth16<u16>() { return true; }

template<class T>
INLINE constexpr bool IsWidth8() { return false; }
template<> INLINE constexpr bool IsWidth8<i8>() { return true; }
template<> INLINE constexpr bool IsWidth8<u8>() { return true; }




typedef int32_t GaapiRet_t;

struct GaapiBasePort_t {
    void* (*malloc_cb)(uint32_t size) = 0;                  //dynamic memory alloc function pointer, it can not be null
    void* (*realloc_cb)(void* rmem, uint32_t newsize) = 0;  //dynamic memory realloc function pointer, it can not be null
    void(*free_cb)(void* buf) = 0;                          //dynamic memory free function pointer, it can not be null
    void(*print_cb)(const char* fmt, ...) = 0;              //log print function pointer, if donot want print, it can be null
};


class GaapiBase_c
{
public:
    GaapiBase_c() {}
    ~GaapiBase_c() {}
public:
    static void* operator new(size_t size, void* buf);
    static void operator delete(void* buf, size_t size);
    static void operator delete(void* buf, void* place);
    static void operator delete(void* buf);
public:
    static constexpr uint64_t Str2U64(const char* str) {
        if (!str) return 0;
        uint64_t u64 = 0;
        for (uint8_t n = 0; n < 8; n++) {
            if (str[n] == '\0') {
                break;
            }
            u64 = (u64 << 8) | str[n];
        }
        return u64;
    }
    static constexpr uint32_t Str2Key(const char* str) {
        if (!str) return 0;
        const uint32_t FNV_PRIME = 16777619;
        const uint32_t FNV_OFFSET_BASIS = 2166136261;
        uint32_t key = FNV_OFFSET_BASIS;
        while (1) {
            uint32_t tmp = *str++;
            if (!tmp) break;
            key ^= tmp;
            key *= FNV_PRIME;
        }
        return key;
    }
    static constexpr char* Strrchr_m(const char* str, char c) {
        int32_t strLen = 0;
        while (str[strLen++] != '\0');
        strLen -= 1;
        char* outStr = (char*)str;
        if (c == '\0') {
            return outStr + strLen + 1;
        }
        for (int32_t n = strLen; n > 0; n--) {
            if (outStr[n] == c) {
                return (outStr + n);
            }
        }
        return outStr;
    }
};

void* gaapi_memset(void* dst, uint8_t val, int32_t length);
void* gaapi_memcpy(void* dst, const void* src, int32_t length);
void* gaapi_memmove(void* dst, const void* src, int32_t length);
};

