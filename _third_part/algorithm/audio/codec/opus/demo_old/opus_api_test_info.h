#pragma once

#include <stdint.h>

// other param
struct BaseInfo_t {
    const char* type;
    const char* key;
};

struct EncInfo_t :public BaseInfo_t {
    // pcm info
    uint8_t* pcm;
    uint32_t pcmByte;
    uint32_t rate;
    uint16_t ch;
    uint16_t width;
    // encoded data info
    uint8_t* out;
    uint32_t outByte;
    // enc param
    uint32_t bitrate;
    uint16_t frame0p1Ms; //0.1ms
    // sim param
    bool is_save_encoded_out;
    bool is_save_encoded_out_file;
    bool is_save_encoded_out_array;
};

struct DecInfo_t :public BaseInfo_t {
    // encoded data info
    uint8_t* encoedData;
    uint32_t encoedDataByte;
    // decoded data info
    uint8_t* pcm;
    uint32_t pcmByte;
    // dec param
    uint32_t rate;
    uint16_t ch;
    // ret info
    uint16_t width;
    // sim param
    bool is_save_pcm_out;
};

bool register_opus_test_info(void* info);
bool get_opus_test_info(const char* type, void* info);

#define OPUS_API_TEST_INFO_SAVE_OUT (true)
#define OPUS_API_TEST_INFO_SAVE_OUT_BY_FILE ((true)&&(OPUS_API_TEST_INFO_SAVE_OUT))
#define OPUS_API_TEST_INFO_SAVE_OUT_BY_ARRAY ((true)&&(!OPUS_API_TEST_INFO_SAVE_OUT_BY_FILE))