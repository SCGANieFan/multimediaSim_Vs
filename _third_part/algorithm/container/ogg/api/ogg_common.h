#pragma once
#include "gasf.h"

#define OGG_VERSION "3.0.0"

#define OGG_RET_SUCCESS              (0)
#define OGG_RET_FAIL                 (-1)
#define OGG_RET_MORE_DATA            (-2)
#define OGG_RET_GENERATE_NOT_FINISH  (-3)
#define OGG_RET_OUT_BUFF_NOT_ENOUGH  (-4)
#define OGG_RET_INPUT_FAIL           (-5)
#define OGG_RET_INNER_ERROR          (-6)
#define OGG_RET_FINISH               (-7)
#define OGG_RET_NOT_SUPPORT          (-8)
#define OGG_RET_INCOMPLETE           (-9)

#define LOG_OGG(fmt,...)		   LOG_GASF(fmt,##__VA_ARGS__)

namespace ogg_ns {
using OggRet_t = GASF_NAME_SPACE::GasfRet_t;

};