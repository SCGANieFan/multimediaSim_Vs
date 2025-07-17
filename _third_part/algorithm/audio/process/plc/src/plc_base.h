#pragma once
#include <stdint.h>
#include <assert.h>
#include <string.h>


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

#define ALGO_ASSERT(s) assert(s)

#if 1
#if WIN32
#define LOG(func,fmt,...) if(func) func("<%s>[%s](%d)" fmt "\n", strrchr(__FILE__,'\\') + 1,__func__, __LINE__, ##__VA_ARGS__)
#else
#define LOG(func,fmt,...) if(func) func("<%s>[%s](%d)" fmt "\n", strrchr(__FILE__,'/') + 1,__func__, __LINE__, ##__VA_ARGS__)
#endif
#else
#define LOG(func,fmt,...) 
#endif

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


void* memcpy_m(void* dst, const void* src, i32 length);
void* memset_m(void* dst, u8 val, i32 length);
void* memmove_m(void* dst, const void* src, i32 length);
#define PLC_MEM_CPY(dst,src,size)	memcpy_m((void*)(dst),(const void*)(src),(i32)(size))
#define PLC_MEM_SET(dst,val,size)	memset_m((void*)(dst),(u8)(val),(i32)(size))
#define PLC_MEM_MOVE(dst,src,size) memmove_m((void*)(dst),(const void*)(src),(i32)(size))	



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
