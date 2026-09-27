#pragma once
#include "smf_error.h"
#include "smf_debug.h"
#if 1
#define return_CatchException(v,exp) \
	return exp;
#elif 1
#define return_CatchException(v,exp) \
try {\
	return exp;\
}\
catch (smf_error_t err) {\
	debug_error_print(&err);\
	return v;\
}\
catch (...) {\
	dbgErrPSL("unknown exception");\
	return v;\
}
#else ///catch std exception
#include <exception>
#define return_CatchException(v,exp) \
try {\
	return exp;\
}\
catch (smf_error_t err) {\
	debug_error_print(&err);\
	return v;\
}\
catch(std::exception &exce){\
	dbgErrPSL(exce.what());\
}\
catch (...) {\
	dbgErrPSL("unknown exception");\
	return v;\
}
#endif

///-fexceptions
