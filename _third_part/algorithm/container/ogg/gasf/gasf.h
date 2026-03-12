#pragma once
#include "gasf_base.h"
#include "gasf_data.h"

#if WIN32
#define LOG_GASF(fmt,...) Print("%s/%d#%s()" fmt "\n", Strrchr(__FILE__,'\\') + 1, __LINE__, __func__,  ##__VA_ARGS__)
#else
#define LOG_GASF(fmt,...) Print("%s/%d#%s()" fmt "\n", Strrchr(__FILE__,'/') + 1, __LINE__, __func__,  ##__VA_ARGS__)
#endif

#define GASF_RET_SUCCESS        (0)
#define GASF_RET_FAIL           (-1)

namespace GASF_NAME_SPACE {

typedef int32_t GasfRet_t;

struct GasfBasePort_t {
    void* (*malloc_cb)(uint32_t size) = 0;                  //dynamic memory alloc function pointer, it can not be null
    void* (*realloc_cb)(void* rmem, uint32_t newsize) = 0;  //dynamic memory realloc function pointer, it can not be null
    void(*free_cb)(void* buf) = 0;                          //dynamic memory free function pointer, it can not be null
    void(*print_cb)(const char* buf, uint32_t len) = 0;     //log print function pointer, if donot want print, it can be null
};

class Gasf_c :public GasfBase_c
{
public:
    Gasf_c();
    virtual ~Gasf_c();
public:
    template<class T>
    static void* CreateApi(GasfBasePort_t* bp) {
        if (!bp || !bp->malloc_cb || !bp->free_cb)return 0;
        Gasf_c* api = (Gasf_c*)bp->malloc_cb(sizeof(T));
        if (!api) { return 0; }
        gasf_memset(api, 0, sizeof(T));
        new(api) T();
        if (!CreateApiPost(bp, api)) {
            bp->free_cb(api);
            return 0;
        }
        return api;
    }
    static bool CreateApiPost(GasfBasePort_t* bp, Gasf_c* api);
    static GasfRet_t OpenApi(void* api);
    static GasfRet_t SetApi(void* api, const char* choose, void* val);
    static GasfRet_t GetApi(void* api, const char* choose, void* val);
    static GasfRet_t RunApi(void* api, GasfData_c& iData, GasfData_c& oData);
    static GasfRet_t ReceiveApi(void* api, GasfData_c& iData);
    static GasfRet_t GenerateApi(void* api, GasfData_c& oData);
    static GasfRet_t CloseApi(void* api);
    static bool DestoryeApi(void* api);

public:
    virtual GasfRet_t Open() { return GASF_RET_FAIL; }
    virtual GasfRet_t Set(uint32_t key, void* val) { return GASF_RET_FAIL; }
    virtual GasfRet_t Get(uint32_t key, void* val) { return GASF_RET_FAIL; }
    virtual GasfRet_t Run(GasfData_c& iData, GasfData_c& oData) { return GASF_RET_FAIL; }
    virtual GasfRet_t Receive(GasfData_c& iData) { return GASF_RET_FAIL; }
    virtual GasfRet_t Generate(GasfData_c& oData) { return GASF_RET_FAIL; }
    virtual GasfRet_t Close() { return GASF_RET_FAIL; }
protected:
    void Print(const char* fmt, ...);
protected:
    void* _magic0;
    GasfBasePort_t _bp;
    u8 _logBuf[128];
    void* _magic1;
public:
    static constexpr uint64_t Str2U64Noloss(const char* str) {
        if (!str) return 0;
        uint64_t u64 = 0;
        for (uint8_t n = 0; n < 8; n++) {
            if (str[n] == '\0')break;
            u64 = (u64 << 8) | str[n];
        }
        return u64;
    }
    static constexpr uint32_t Str2U32Loss(const char* str, uint32_t N = 32) {
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
    static constexpr uint32_t StrLen(const char* str, uint32_t N = 10 * 1024) {
        int32_t strLen = 0;
        N = N > 10 * 1024 ? 10 * 1024 : N;
        for (strLen = 0; strLen < (N + 1); strLen++) {
            if (str[strLen] == '\0') break;
        }
        return strLen;
    }
    static constexpr uint32_t Str2Key(const char* str) {
        return Str2U32Loss(str, 32);
    }
    static constexpr char* Strrchr(const char* str, char c) {
        int32_t strLen = (int32_t)StrLen(str);
        char* outStr = (char*)str;
        if (c == '\0') {
            return outStr + strLen;
        }
        for (int32_t n = strLen - 1; n >= 0; n--) {
            if (outStr[n] == c) {
                return (outStr + n);
            }
        }
        return outStr;
    }
};


};

