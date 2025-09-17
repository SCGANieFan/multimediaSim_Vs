#pragma once

//#include <stdio.h>
#include <stdarg.h>


// std
typedef va_list VaListPorting_t;
#define VaStartPorting(ap,x) va_start(ap,x)
#define VaArgPorting(ap,t) va_arg(ap, t)
#define VaEndPorting(ap) va_end(ap)
#define VaCopyPorting(destination, source) va_copy(destination, source)

