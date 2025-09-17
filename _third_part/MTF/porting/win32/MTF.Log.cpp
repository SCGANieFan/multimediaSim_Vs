#include <stdio.h>
#include "MTF.String.h"
#include "MTF.Porting.h"

using namespace mtf_ns;

//mtf_void LogPorting(const char* buf) {
//    printf("%s", buf);
//}
#if 0
mtf_void LogPorting(const char* format, va_list args) {
    char buf[256];
    VsprintfPorting(buf, format, args);
    printf("%s", buf);
}
#endif
mtf_void LogPorting(const char* format, ...) {
    VaListPorting_t args;
    VaStartPorting(args, format);
#if 0
    if ((char*)((mtf_u32*)args)[-1] == format) {
        printf("%s", format);
    }
    else {
        
    }
#endif
    char buf[256];
    VsprintfPorting(buf, format, args);
    printf("%s", buf);
}
mtf_void LogNoFormatPorting(const char* buf) {
    printf("%s", buf);
}


