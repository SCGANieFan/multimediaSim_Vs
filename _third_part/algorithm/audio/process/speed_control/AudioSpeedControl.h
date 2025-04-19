#pragma once
#include<stdint.h>

#define AUDIO_SPEED_CTR_API_RET_SUCCESS 0
#define AUDIO_SPEED_CTR_API_RET_FAIL -1

typedef struct AudioSpeedControlApiPort_t
{
    void* (*cb_malloc)(int size);
	void (*cb_free)(void* ptr);
	void (*cb_printf)(const char* fmt, ...);
};
typedef struct AudioSpeedControlApiParam_t
{
    AudioSpeedControlApiPort_t port;
    int32_t sampleRate;
    int32_t sampleWidth;
    int32_t channels;
};

enum class AudioSpeedCtrApiSet_e{
    AUDIO_SPEED_CTR_API_SET_SPEED=0,
    AUDIO_SPEED_CTR_API_SET_MAX,
};

enum class AudioSpeedCtrApiGet_e{
    AUDIO_SPEED_CTR_API_GET_MAX=0,
};

class AudioSpeedCtrApi_c
{
public:
    AudioSpeedCtrApi_c(){}
    ~AudioSpeedCtrApi_c(){}
public:
    static void* Create(AudioSpeedControlApiParam_t *param);
    static int32_t Set(void* hd, AudioSpeedCtrApiSet_e choose, void* val);
    static int32_t Get(void* hd, AudioSpeedCtrApiGet_e choose, void* val);
    static int32_t Run(void* hd, uint8_t* in, int32_t inSize, uint8_t*out, int32_t*outSize);
    static void Destory(void* hd);
};









