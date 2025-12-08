#include "MTF.Printer.h"
#include "MTF.Porting.h"
#include <stdio.h>
namespace mtf_ns {


mtf_void PrintfOri(const char* format, ...) {
    VaListPorting_t args;
    //VaStartPorting(args, format);
    va_start(args, format);
    char buf[256];
    VsprintfPorting(buf, "%lld[%u]%s\n", TimeMsPorting(), ThreadIdPorting(), args);
    LogNoFormatPorting(buf);
}

mtf_void Printf(mtf_u16 ch, const char* format, ...) {
    VaListPorting_t args;
    VaStartPorting(args, format);
    char buf[256];
    VsprintfPorting(buf, "%lld[%u][%u]", TimeMsPorting(), ch, ThreadIdPorting());
    VsprintfPorting(buf + MTF_String::StrLen(buf), format, args);
    LogNoFormatPorting(buf);
}

mtf_void PrintfNoformat(const char* buf) {
    LogNoFormatPorting(buf);
}

}