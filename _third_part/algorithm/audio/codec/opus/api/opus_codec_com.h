#pragma once
#include "gasf.h"
#define OPUS_VERSION "1.2.1.0"

#define OPUS_RET_SUCCESS        (0)
#define OPUS_RET_FAIL           (-1)
#define LOG_OPUS LOG_GASF
#define LOG_OPUS_WITH_HANDLE LOG_GASF_WITH_HANDLE

class OpusCodec_c :public GASF_NAME_SPACE::Gasf_c {
public:
    OpusCodec_c() {}
    virtual ~OpusCodec_c() {}
public:
    static GASF_NAME_SPACE::GasfRet_t SetApi(void* api, const char* choose, void* val);
};

using OpusRet_t = GASF_NAME_SPACE::GasfRet_t;
using OpusData_c = GASF_NAME_SPACE::GasfData_c;


