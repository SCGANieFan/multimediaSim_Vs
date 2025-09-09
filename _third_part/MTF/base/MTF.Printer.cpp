#include<stdarg.h>
#include "MTF.Printer.h"

#ifdef _WIN32
#include<stdio.h>
#define printf_inner(fmt,...)		printf(fmt,##__VA_ARGS__)
#else
#define printf_ori(fmt,...)		
#endif

MTF_Printer::MTF_Printer()
{
}
MTF_Printer::~MTF_Printer()
{
}
mtf_void MTF_Printer::Printf(const char* _Format, ...)
{
    char buf[256];
    va_list args;
    va_start(args, _Format);
    vsprintf(buf, (const char*)_Format, args);
    va_end(args);
    printf_inner(buf);
}
