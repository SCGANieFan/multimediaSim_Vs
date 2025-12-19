#include <stdio.h>
#include <stdarg.h>
#include <mutex>
#include <thread>
#include"hal_trace.h"
#include <Windows.h>

class Trace_c {
public:
    Trace_c() {
        if (!fp) { fp = fopen("./log.txt", "wb+"); }
    }
    ~Trace_c() {
        if (fp) { fclose(fp); }
    }
private:
public:
    FILE* fp = 0;
};
static std::mutex printMtx;
Trace_c trace;
void hal_trace_printf(const char* fmt, ...) {
    std::lock_guard<std::mutex> lock(printMtx);
    if (fmt == NULL) return;
    char buf[256];
    va_list args;
    va_start(args, fmt);
    int len = vsnprintf(buf, sizeof(buf), fmt, args);
    va_end(args);
    if (len >= sizeof(buf)) {
        buf[sizeof(buf) - 1] = '\0';
    }
    if (trace.fp) {
        fprintf(trace.fp, "%s", buf);
        fflush(trace.fp);
    }
    OutputDebugStringA(buf);
    fwrite(buf, 1, len, stdout);
    fflush(stdout);
}

void hal_trace_printf_arg(const char* fmt, va_list args) {
	printMtx.lock();
	char buf[256];
	int len = vsprintf(buf, fmt, args);
	printf("%s", buf);
	printMtx.unlock();
}

int hal_trace_output(const unsigned char* buf, unsigned int buf_len) {
    printf("%s", buf);
    return 0;
}