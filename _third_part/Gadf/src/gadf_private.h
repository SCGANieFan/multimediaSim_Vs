#pragma once
#include<stdint.h>
#include<stddef.h>
#include"gadf_porting_api.h"

#if 1
#if WIN32
#define LOG(fmt,...) if(_bp._print) _bp._print("<%s>[%s](%d,%u)" fmt "\n", Strrchr_m(__FILE__,'\\') + 1,__func__, __LINE__, GadfThreadId(), ##__VA_ARGS__)
#else
#define LOG(fmt,...) if(_bp._print) _bp._print("<%s>[%s](%d,%u)" fmt "\n", Strrchr_m(__FILE__,'/') + 1,__func__, __LINE__, GadfThreadId(), ##__VA_ARGS__)
#endif
#else
#if WIN32
#define LOG(fmt,...) if(_bp._print) _bp._print("<%s>[%s](%d)" fmt "\n", Strrchr_m(__FILE__,'\\') + 1,__func__, __LINE__, ##__VA_ARGS__)
#else
#define LOG(fmt,...) if(_bp._print) _bp._print("<%s>[%s](%d)" fmt "\n", Strrchr_m(__FILE__,'/') + 1,__func__, __LINE__, ##__VA_ARGS__)
#endif
#endif