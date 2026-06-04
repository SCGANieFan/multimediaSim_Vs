#pragma once
#ifndef __CHIP_H__
#define __CHIP_H__
/** 
 * ChipCheck(): Chip ID verification, is easy to be crack and is recommended for use on interfaces such as open/close.
 * CpuCheck(): CPU ID verification, is difficult to be crack and can be embedded into various data processing.
 * [note] To reduce the risk of cracking, please use these two macros separately and do not use them in the same location! 
 * [note] Some CPUs do not have a unique processerID, CpuCheck() is defined as ChipCheck() by default.
 */
#include <stdint.h>

#define __ChipID (((*(uint32_t*)(0x40080000)) & 0x0000FFFF))
#define __ChipIDMask (((*(uint32_t*)(0x40080000)) & 0x0000FFF0))
#define __ChipIDMaskCheck(v) (__ChipIDMask==(v))

//#ifdef __hifi4__
#ifdef __XTENSA__
#ifdef __hifi_simulation__
#define ChipCheck() (1)
#define CpuCheck() (1)
#else
#include <xtensa/hal.h>
#define __ProcesserID (XT_RSR_PRID() & 0xFFFF)
#define ChipCheck() (__ChipIDMaskCheck(1600) || __ChipIDMaskCheck(1696) || __ChipIDMaskCheck(0x1600) || __ChipIDMaskCheck(0x1700))
#define CpuCheck() (__ProcesserID == 0x0272 || ChipCheck())
#endif
#endif

#ifdef __ZSP__
#define ChipCheck() (1)
#define CpuCheck() ChipCheck()
#endif

//#ifdef __a7__
#ifdef __ARM_ARCH_7A__
#define ChipCheck() (__ChipIDMaskCheck(2000))
#define CpuCheck() ChipCheck()
#endif

//#ifdef __m55__
#if (defined (__ARM_FEATURE_MVE) && (__ARM_FEATURE_MVE > 0U))
#define __ProcesserID ((*(uint32_t*)(0xE000ECFC)) & 0xFFFF0000)
#define __ProcesserCheck(v) (__ProcesserID==(v))
#define ChipCheck() (__ChipIDMaskCheck(1600) || __ChipIDMaskCheck(1696) || __ChipIDMaskCheck(0) || __ChipIDMaskCheck(3600) || __ChipIDMaskCheck(0x1600) || __ChipIDMaskCheck(0x1700))
#define CpuCheck() (__ProcesserCheck(0x20150000) || ChipCheck())
#endif

//#ifdef __m33__
#if (defined (__ARM_ARCH_8M_MAIN__) && (!defined (__ARM_FEATURE_MVE)))
#define __ProcesserCheck(v) (__ProcesserID==(v))
#define ChipCheck() (__ChipIDMaskCheck(0x1300) || __ChipIDMaskCheck(0x1500) || __ChipIDMaskCheck(0x1510) || __ChipIDMaskCheck(1600) || __ChipIDMaskCheck(1696) || __ChipIDMaskCheck(2000) || __ChipIDMaskCheck(0x2070) || __ChipIDMaskCheck(0x1600) || __ChipIDMaskCheck(0x1700))
#define CpuCheck() (__ProcesserCheck(0x20150000) || ChipCheck())
#endif

#ifdef __a55__
#define ChipCheck() (1)
#define CpuCheck() (1)
#endif

#ifdef WIN32
#define ChipCheck() (1)
#define CpuCheck() (1)
#endif

#ifdef __hifi5s__
#define ChipCheck() (1)
#define CpuCheck() (1)
#endif

///Chip ID verification is easy to crack and is recommended for use on interfaces such as open/close.
#ifndef ChipCheck
#define ChipCheck() (0)
#endif

///CPU ID verification is difficult to crack and can be embedded into various data processing.
#ifndef CpuCheck
#define CpuCheck() (0)
#endif

#ifndef __ProcesserID
#define __ProcesserID (0)
#endif

#endif
