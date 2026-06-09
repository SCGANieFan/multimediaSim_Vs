#pragma once
#include "gasf_base.h"
#include "gasf_data.h"

#ifdef _WIN32
#define GASF_PATH_SEP '\\'
#else
#define GASF_PATH_SEP '/'
#endif

#define LOG_GASF(fmt,...) Print("%s/%d#%s()" fmt "\n", Strrchr(__FILE__,GASF_PATH_SEP) + 1, __LINE__, __func__,  ##__VA_ARGS__)
#define LOG_GASF_WITH_HANDLE(hd,fmt,...) if(hd) ((Gasf_c*)hd)->Print("%s/%d#%s()" fmt "\n", Strrchr(__FILE__,GASF_PATH_SEP) + 1, __LINE__, __func__,  ##__VA_ARGS__)

#define GASF_RET_SUCCESS        (0)
#define GASF_RET_FAIL           (-1)

namespace GASF_NAME_SPACE {

typedef int32_t GasfRet_t;

struct GasfBasePort_t {
    void* (*malloc_cb)(uint32_t size) = nullptr;                  //dynamic memory alloc function pointer, it can not be null
    void* (*realloc_cb)(void* rmem, uint32_t newsize) = nullptr;  //dynamic memory realloc function pointer, it can not be null
    void(*free_cb)(void* buf) = nullptr;                          //dynamic memory free function pointer, it can not be null
    void(*print_cb)(const char* buf, uint32_t len) = nullptr;     //log print function pointer, if donot want print, it can be null
    static constexpr GasfBasePort_t Default() {
        return GasfBasePort_t{
            nullptr, nullptr, nullptr, nullptr
        };
    }
};

class Gasf_c :public GasfBase_c
{
public:
    Gasf_c();
    virtual ~Gasf_c();
public:
    template<class T>
    static void* CreateApi(GasfBasePort_t* bp) noexcept {
        CHECK_TYPE_IS_DERIVED_FROM_GASF(T);
        if (!bp || !bp->malloc_cb || !bp->free_cb)return 0;
        Gasf_c* api = (Gasf_c*)bp->malloc_cb(sizeof(T));
        if (!api) { return 0; }
        gasf_memset(api, 0, sizeof(T));
        new(api) T();
        if (!CreateApiPost(bp, api)) {
            api->~Gasf_c();
            bp->free_cb(api);
            return 0;
        }
        return api;
    }
    static bool CreateApiPost(GasfBasePort_t* bp, Gasf_c* api)noexcept;
    static GasfRet_t OpenApi(void* api)noexcept;
    static GasfRet_t SetApi(void* api, const char* choose, void* val)noexcept;
    static GasfRet_t GetApi(void* api, const char* choose, void* val)noexcept;
    static GasfRet_t RunApi(void* api, GasfData_c& iData, GasfData_c& oData)noexcept;
    static GasfRet_t ReceiveApi(void* api, GasfData_c& iData)noexcept;
    static GasfRet_t GenerateApi(void* api, GasfData_c& oData)noexcept;
    static GasfRet_t CloseApi(void* api)noexcept;
    static bool DestroyApi(void* api)noexcept;

public:
    virtual GasfRet_t Open() noexcept { return GASF_RET_FAIL; }
    virtual GasfRet_t Set(uint32_t key, void* val) noexcept {
        (void)key; (void)val;
        return GASF_RET_FAIL;
    }
    virtual GasfRet_t Get(uint32_t key, void* val) noexcept {
        (void)key; (void)val;
        return GASF_RET_FAIL;
    }
    virtual GasfRet_t Run(GasfData_c& iData, GasfData_c& oData) noexcept {
        (void)iData; (void)oData;
        return GASF_RET_FAIL;
    }
    virtual GasfRet_t Receive(GasfData_c& iData) noexcept {
        (void)iData;
        return GASF_RET_FAIL;
    }
    virtual GasfRet_t Generate(GasfData_c& oData) noexcept {
        (void)oData;
        return GASF_RET_FAIL;
    }
    virtual GasfRet_t Close() noexcept {return GASF_RET_FAIL; }
protected:
public:
    void Print(const char* fmt, ...)noexcept;
protected:
    void* _magic0;
    GasfBasePort_t _bp;
    u8 _logBuf[128];
    void* _magic1;
public:
    static constexpr uint64_t Str2U64Noloss(const char* str) noexcept {
        if (!str) return 0;
        uint64_t val = 0;
        for (uint8_t n = 0; n < 8; n++) {
            if (str[n] == '\0')break;
            val = (val << 8) | str[n];
        }
        return val;
    }
    static constexpr uint32_t Str2U32Loss(const char* str, uint32_t N = 16) noexcept {
        if (!str) return 0;
        const uint32_t FNV_PRIME = 16777619;
        const uint32_t FNV_OFFSET_BASIS = 2166136261;
        uint32_t key = FNV_OFFSET_BASIS;
        N = N > 128 ? 128 : N;
        for (u32 n = 0; n < N; n++) {
            uint32_t tmp = *str++;
            if (!tmp) break;
            key ^= tmp;
            key *= FNV_PRIME;
        }
        return key;
    }
    static constexpr uint32_t StrLen(const char* str, uint32_t N = 10 * 1024) noexcept {
        int32_t strLen = 0;
        N = N > 10 * 1024 ? 10 * 1024 : N;
        for (strLen = 0; strLen < (N + 1); strLen++) {
            if (str[strLen] == '\0') break;
        }
        return strLen;
    }
    static constexpr uint32_t Str2Key(const char* str) noexcept {
        return Str2U32Loss(str, 16);
    }
    static constexpr const char* Strrchr(const char* str, char c) noexcept {
#if 0
    if (!str) return str;
    const char* lastMatch = str;
    for (;;) {
        if (*str == c) {
            lastMatch = str;
        }
        if (*str == '\0') {
            break;
        }
        str++;
    }
    return lastMatch;
#else
        int32_t strLen = (int32_t)StrLen(str);
        const char* outStr = str;
        if (c == '\0') {
            return outStr + strLen;
        }
        for (int32_t n = strLen - 1; n >= 0; n--) {
            if (outStr[n] == c) {
                return (outStr + n);
            }
        }
        return outStr;
#endif
    }
};


};

