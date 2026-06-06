#pragma once
#include "gasf.h"

#define MP4_VERSION "1.0.0"

#define MP4_RET_SUCCESS              (0)
#define MP4_RET_FAIL                 (-1)

#define LOG_MP4(fmt,...)		   LOG_GASF(fmt,##__VA_ARGS__)

namespace mp4_ns {
using Mp4Ret_t = GASF_NAME_SPACE::GasfRet_t;
using Mp4Data_c = GASF_NAME_SPACE::GasfData_c;
using Mp4Base_c = GASF_NAME_SPACE::Gasf_c;
};