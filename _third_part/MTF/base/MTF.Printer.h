#pragma once
#include"MTF.Type.h"
#include"MTF.String.h"
#include <stdio.h>
namespace mtf_ns {

	
mtf_void PrintfOri(const char* format, ...);
mtf_void Printf(mtf_u16 ch, const char* format, ...);
mtf_void PrintfNoformat(const char* buf);

#define MTF_PRINT_CH_DEFAULT 9
#define MTF_PRINT_CH_ERROR 0
#define MTF_PRINT_CH_WARN 1
#define MTF_PRINT_CH_NOTE 2

#define MTF_PRINTORI(fmt,...)			PrintfOri(fmt, ##__VA_ARGS__)
//#define MTF_PRINT_DEMO(ch,fmt,...)		Pri4ntf(ch, "<%s>[%s](%d)" fmt "\n", MTF_String::BaseName(__FILE__), __func__, __LINE__, ##__VA_ARGS__)
#define MTF_PRINT_DEMO(ch,fmt,...)		printf("[%d]%s/%d#%s()" fmt "\n", ch, MTF_String::BaseName(__FILE__), __LINE__, __func__, ##__VA_ARGS__)
#define MTF_PRINT(fmt,...)				MTF_PRINT_DEMO(MTF_PRINT_CH_DEFAULT,fmt, ##__VA_ARGS__)
#define MTF_PRINT_ERR(fmt,...)			MTF_PRINT_DEMO(MTF_PRINT_CH_ERROR,fmt,##__VA_ARGS__)
#define MTF_PRINT_WARN(fmt,...)			MTF_PRINT_DEMO(MTF_PRINT_CH_WARN,fmt,##__VA_ARGS__)
#define MTF_PRINT_NOTE(fmt,...)			MTF_PRINT_DEMO(MTF_PRINT_CH_NOTE,fmt,##__VA_ARGS__)

};
