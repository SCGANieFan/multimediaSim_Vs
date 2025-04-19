#pragma once

#include<stdio.h>
#include<string.h>
#include<stdlib.h>
#include<stdint.h>
#include<math.h>
#include <cassert>

#include "AudioSpeedControl.h"

#define STATIC static
#define INLINE inline

#if 0
typedef char i8;
typedef short i16;
typedef int i32;
typedef long long i64;

typedef unsigned char u8;
typedef unsigned short u16;
typedef unsigned int u32;
typedef unsigned long long u64;

typedef float f32;
typedef double f64;
typedef bool b1;
#endif

typedef struct
{
    int32_t fs;
    int16_t channels;
    int16_t width;
    int32_t BytePerSample;
}AudioInfo_t;


class AudioBuff_c
{
public:
    AudioBuff_c(){}
    ~AudioBuff_c(){}
public:
    INLINE void Init(uint8_t *buf, int32_t max, AudioInfo_t *info){
        _buf = buf;
        _max = max;
        _info = info;
        _off=0;
        _size=0;
    }
    INLINE uint8_t* Buff(){return _buf;}
    INLINE uint8_t* Data(){return _buf+_off;}
    INLINE uint8_t* LeftData(){return _buf+_off+_size;}
    INLINE int32_t Size(){return _size;}
    INLINE int32_t LeftSize(){return _max-_off-_size;}
    INLINE int32_t Append(int32_t size){
        int32_t leftSize = LeftSize();
        if (size > leftSize) {
            ClearUsed();
            leftSize = LeftSize();
        }
        int32_t appendSize = size>leftSize?leftSize:size;
        _size+=appendSize;
        return appendSize;
    }
    INLINE int32_t Append(uint8_t *buf, int32_t size){
        int32_t leftSize = LeftSize();
        if (size > leftSize) {
            ClearUsed();
            leftSize = LeftSize();
        }
        int32_t appendSize = size>leftSize?leftSize:size;
        memcpy(LeftData(),buf,appendSize);
        _size+=appendSize;
        return appendSize;
    }
    INLINE void Used(int32_t used){
        _off+=used;
        _size-=used;
    }
    INLINE void ClearUsed(){
        if(_off){
            memmove(_buf,_buf+_off,_size);
            _off=0;
        }
    }
protected:
    uint8_t *_buf;
    int32_t _max;
    int32_t _off;
    int32_t _size;
    AudioInfo_t *_info;
};


class AudioSpeedCtrOutCtr_c {
public:
    AudioSpeedCtrOutCtr_c() {}
    ~AudioSpeedCtrOutCtr_c() {}
public:
    void Init(AudioBuff_c* oBufIner, AudioBuff_c* oBufInterface, int32_t overlapByte) {
        _oBufInner = oBufIner;
        _oBufInterface = oBufInterface;
        _overlapByte = overlapByte;
    }
    uint8_t* GetOverlap() {
        if (_oBufInner->Size() < _overlapByte) { return 0; }
        return &_oBufInner->LeftData()[-_overlapByte];
    }
    void Append(int32_t size) {
        _oBufInner->Append(size);
        AppendFully();
    }
    void Append(uint8_t* buf, int32_t size) {
        _oBufInner->Append(buf, size);
        AppendFully();
    }
    void AppendFully() {
        int32_t cpByte = _oBufInterface->LeftSize();
        int32_t remByte = _oBufInner->Size() - _overlapByte;
        if (cpByte && remByte) {
            cpByte = cpByte > remByte ? remByte : cpByte;
            _oBufInterface->Append(_oBufInner->Data(), cpByte);
            _oBufInner->Used(cpByte);
        }
    }
public:
    AudioBuff_c *_oBufInner;
    AudioBuff_c *_oBufInterface;
    int32_t _overlapByte;
};

class AudioSpeedCtr_c
{
public:
    AudioSpeedCtr_c(){}
    ~AudioSpeedCtr_c(){}
public:
    static void* Create(AudioSpeedControlApiParam_t *param);
    static int32_t Set(void* hd, AudioSpeedCtrApiSet_e choose, void* val);
    static int32_t Get(void* hd, AudioSpeedCtrApiGet_e choose, void* val);
    static int32_t Run(void* hd, uint8_t* in, int32_t inSize, uint8_t*out, int32_t*outSize);
    static void Destory(void* hd);
protected:
    int32_t Init(AudioSpeedControlApiParam_t *param);
    int32_t Receive(uint8_t* in, int32_t inSize);
    int32_t Generate(uint8_t* out, int32_t *outSize);
    int32_t Process(uint8_t* in, int32_t inSize, uint8_t* out, int32_t *outSize);
public:
    AudioSpeedControlApiPort_t _port;
#if 0
    int32_t _channels;
    int32_t _sampleRate;
    int32_t _sampleWidth;
#endif
    AudioInfo_t _audioInfo;
    int32_t _seekMs;
    int32_t _overlapMs;
    int32_t _constMs;
    int32_t _seekSamples;
    int32_t _overlapSamples;
    int32_t _constSamples;
    int32_t _frameByte;
    double _speed;
    AudioBuff_c _iBuf;
    AudioBuff_c _oBuf;
    AudioSpeedCtrOutCtr_c _oCtr;
    bool isBeginning;
    bool isEnd;
};

#define MAX(a,b) ((a)>(b)?(a):(b))
#define MIN(a,b) ((a)>(b)?(b):(a))

int32_t SeekBestLag(int8_t* in, int8_t* templateBuff, int32_t sampleWidth, int32_t seekSamples, int32_t accorelationSamples, int32_t channels);
int32_t overlap(int8_t* out, const int8_t* in, int8_t* bufferTemplate, int32_t overlapLen, int32_t channels, int32_t sampleWidth);