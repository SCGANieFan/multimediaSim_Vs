#pragma once
#include <stdint.h>
#include <stddef.h>

namespace gaapi_ns {

#define GAAPI_RET_SUCCESS        (0)
#define GAAPI_RET_FAIL           (-1)

#if WIN32
#define LOG_GAAPI(fmt,...) if(_bp.print_cb) _bp.print_cb("<%s>[%s](%d)" fmt "\n", Strrchr_m(__FILE__,'\\') + 1,__func__, __LINE__, ##__VA_ARGS__)
#else
#define LOG_GAAPI(fmt,...) if(_bp.print_cb) _bp.print_cb("<%s>[%s](%d)" fmt "\n", Strrchr_m(__FILE__,'/') + 1,__func__, __LINE__, ##__VA_ARGS__)
#endif



typedef int32_t GaapiRet_t;

struct GaapiBasePort_t {
    void* (*malloc_cb)(uint32_t size) = 0;                  //dynamic memory alloc function pointer, it can not be null
    void* (*realloc_cb)(void* rmem, uint32_t newsize) = 0;  //dynamic memory realloc function pointer, it can not be null
    void(*free_cb)(void* buf) = 0;                          //dynamic memory free function pointer, it can not be null
    void(*print_cb)(const char* fmt, ...) = 0;              //log print function pointer, if donot want print, it can be null
};


class GaapiBase_c
{
public:
    GaapiBase_c() {}
    ~GaapiBase_c() {}
public:
    static void* operator new(size_t size, void* buf);
    static void operator delete(void* buf, size_t size);
    static void operator delete(void* buf, void* place);
    static void operator delete(void* buf);
public:
    static constexpr uint64_t Str2Key(const char* str) {
        uint64_t key = 0;
        for (uint8_t n = 0; n < 8; n++) {
            if (str[n] == '\0') {
                break;
            }
            key = (key << 8) | str[n];
        }
        return key;
    }
    static constexpr char* Strrchr_m(const char* str, char c) {
        int32_t strLen = 0;
        while (str[strLen++] != '\0');
        strLen -= 1;
        char* outStr = (char*)str;
        if (c == '\0') {
            return outStr + strLen + 1;
        }
        for (int32_t n = strLen; n > 0; n--) {
            if (outStr[n] == c) {
                return (outStr + n);
            }
        }
        return outStr;
    }
};

void* gaapi_memset(void* dst, uint8_t val, int32_t length);
void* gaapi_memcpy(void* dst, const void* src, int32_t length);
void* gaapi_memmove(void* dst, const void* src, int32_t length);
};

