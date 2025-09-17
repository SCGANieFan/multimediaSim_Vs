#include <stdio.h>
#include <stdarg.h>
#include "MTF.Porting.h"

// std
mtf_i32 VsprintfPorting(char* const buffer, char const* const format, VaListPorting_t argList) {
	return vsprintf(buffer, format, argList);
}

mtf_i32 VsprintfPorting(char* const buffer, char const* const format, ...) {
	VaListPorting_t args;
	VaStartPorting(args, format);
	return vsprintf(buffer, format, args);
}

mtf_i32 VsnprintfPorting(char* const buffer, mtf_i32 const bufferCount, char const* const format, char* argList) {
	return vsnprintf(buffer, bufferCount, format, argList);
}

mtf_i32 SnprintfPorting(char* const buffer, mtf_i32 const bufferCount, char const* const format, ...) {
	VaListPorting_t args;
	VaStartPorting(args, format);
	//return snprintf(buffer, bufferCount, format, args);
	return snprintf(buffer, bufferCount, format, *(mtf_u32*)args);
}
