#pragma once
#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#ifndef EXTERNC
#ifndef __cplusplus
#define EXTERNC
#else
#define EXTERNC extern "C"
#endif
#endif



#define returnIfErrC(ret,v) if(v) return ret;
#define returnIfErrCS(ret,v)  if(v) return ret;
#define dbgTestPXL(fmt,...) printf("%s/%d#%s()" fmt "\n", strrchr(__FILE__,'\\') + 1, __LINE__, __func__,  ##__VA_ARGS__)



