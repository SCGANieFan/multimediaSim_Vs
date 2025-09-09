#pragma once
#include<stdint.h>

#if 1
typedef uint8_t mtf_u8;
typedef uint16_t mtf_u16;
typedef uint32_t mtf_u32;
typedef uint64_t mtf_u64;

typedef int8_t mtf_i8;
typedef int16_t mtf_i16;
typedef int32_t mtf_i32;
typedef int64_t mtf_i64;
#else
typedef unsigned char mtf_u8;
typedef unsigned short mtf_u16;
typedef unsigned int mtf_u32;
typedef unsigned long long mtf_u64;

typedef char mtf_i8;
typedef short mtf_i16;
typedef int mtf_i32;
typedef long long mtf_i64;
#endif
typedef float mtf_f32;
typedef double mtf_d64;
typedef bool mtf_bool;
typedef void mtf_void;




