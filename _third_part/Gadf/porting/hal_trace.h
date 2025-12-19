#pragma once
#include<stdio.h>

int hal_trace_output(const unsigned char* buf, unsigned int buf_len);
void hal_trace_printf(const char* fmt, ...);

#define TRACE(n,fmt,...) hal_trace_printf(fmt"\n", ##__VA_ARGS__)

