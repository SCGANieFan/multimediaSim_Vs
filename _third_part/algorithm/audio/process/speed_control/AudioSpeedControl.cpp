#if 1
#include <new>
#include "AudioSpeedControlInner.h"

#ifdef WIN32
#define LOG(func,fmt,...) if(func) func("[%s](%d)" fmt, __func__, __LINE__, ##__VA_ARGS__)
#else
#define LOG(func,fmt,...) if(func) func("[%s](%d)" fmt, __func__, __LINE__, ##__VA_ARGS__)
#endif

void* AudioSpeedCtr_c::Create(AudioSpeedControlApiParam_t *param){
    if(!param
        ||!param->port.cb_malloc
        ||!param->port.cb_free
        ||!param->sampleRate
        ||!param->sampleWidth
        ||!param->channels){
        return NULL;
    }

    AudioSpeedCtr_c* pAudioSpeedCtr = (AudioSpeedCtr_c*)param->port.cb_malloc(sizeof(AudioSpeedCtr_c));
    if(!pAudioSpeedCtr){
        return NULL;
    }
    new(pAudioSpeedCtr)AudioSpeedCtr_c();
    int32_t ret = pAudioSpeedCtr->Init(param);
    if(ret!=AUDIO_SPEED_CTR_API_RET_SUCCESS){
        Destory(pAudioSpeedCtr);
        pAudioSpeedCtr=0;
    }
    return pAudioSpeedCtr;
}
int32_t AudioSpeedCtr_c::Set(void* hd, AudioSpeedCtrApiSet_e choose, void* val){
    if(!hd){
        return AUDIO_SPEED_CTR_API_RET_FAIL;
    }
    AudioSpeedCtr_c* pAudioSpeedCtr = (AudioSpeedCtr_c*)hd;
    switch(choose){
        case AudioSpeedCtrApiSet_e::AUDIO_SPEED_CTR_API_SET_SPEED:{
            float speed = *(float*)val;
            speed=speed<0.5f?0.5f:speed;
            speed=speed>2.0f?2.0f:speed;
            pAudioSpeedCtr->_speed = speed;
            break;
        }
        default:
            break;
    }
    return AUDIO_SPEED_CTR_API_RET_SUCCESS;
}
int32_t AudioSpeedCtr_c::Get(void* hd, AudioSpeedCtrApiGet_e choose, void* val){
#if 0
    if(!hd){
        return AUDIO_SPEED_CTR_API_RET_FAIL;
    }
    AudioSpeedCtr_c* pAudioSpeedCtr = (AudioSpeedCtr_c*)hd;
    switch(choose){
        case AudioSpeedCtrApiGet_e::AUDIO_SPEED_CTR_API_GET_MAX:{
            int32_t max = pAudioSpeedCtr->sampleRate * pAudioSpeedCtr->sampleWidth * pAudioSpeedCtr->channels;
            *(int32_t*)val = max;
            break;
        }
        default:
            break;
    }
#endif
    return AUDIO_SPEED_CTR_API_RET_SUCCESS;
}
int32_t AudioSpeedCtr_c::Run(void* hd, uint8_t* in, int32_t inSize, uint8_t*out, int32_t*outSize){
    if(!hd
        ||!in
        ||inSize<=0
        ||!out
        ||!outSize
        ||*outSize<=0)
    {
        return AUDIO_SPEED_CTR_API_RET_FAIL;
    }
    int32_t ret;
    AudioSpeedCtr_c* pAudioSpeedCtr = (AudioSpeedCtr_c*)hd;
#if 0
    ret = pAudioSpeedCtr->Receive(in,inSize);
    if(ret!=AUDIO_SPEED_CTR_API_RET_SUCCESS){
        return AUDIO_SPEED_CTR_API_RET_FAIL;
    }
    int32_t outSize0 = (int32_t)((float)inSize/pAudioSpeedCtr->_speed);
    ret = pAudioSpeedCtr->Generate(out,&outSize0);
    if(ret!=AUDIO_SPEED_CTR_API_RET_SUCCESS){
        return AUDIO_SPEED_CTR_API_RET_FAIL;
    }
    *outSize=outSize0;
#else
    ret = pAudioSpeedCtr->Process(in, inSize, out, outSize);
    if (ret != AUDIO_SPEED_CTR_API_RET_SUCCESS) {
        return AUDIO_SPEED_CTR_API_RET_FAIL;
    }
#endif
    return AUDIO_SPEED_CTR_API_RET_SUCCESS;
}
void AudioSpeedCtr_c::Destory(void* hd){
    if(!hd){
        return;
    }
    AudioSpeedCtr_c* pAudioSpeedCtr = (AudioSpeedCtr_c*)hd;
    AudioSpeedControlApiPort_t port = pAudioSpeedCtr->_port;
    if(pAudioSpeedCtr->_iBuf.Buff()) port.cb_free(pAudioSpeedCtr->_iBuf.Buff());
    if(pAudioSpeedCtr->_oBuf.Buff()) port.cb_free(pAudioSpeedCtr->_oBuf.Buff());
    pAudioSpeedCtr->~AudioSpeedCtr_c();
    port.cb_free(pAudioSpeedCtr);
}


int32_t AudioSpeedCtr_c::Init(AudioSpeedControlApiParam_t *param){
    memset(this,0,sizeof(AudioSpeedCtr_c));
    _port = param->port;
    _audioInfo.fs=param->sampleRate;
    _audioInfo.channels=param->channels;
    _audioInfo.width=param->sampleWidth;
    _audioInfo.BytePerSample=_audioInfo.channels*_audioInfo.width;
    const int32_t seekMs = 0;
    const int32_t overlapMs = 2;
    const int32_t constMs = 14;
    _seekSamples=seekMs * _audioInfo.fs / 1000;
    _overlapSamples=overlapMs * _audioInfo.fs / 1000;
    _constSamples=constMs * _audioInfo.fs / 1000;
    _speed = 1;

    _frameByte = _audioInfo.BytePerSample*(_seekSamples+_overlapSamples+_constSamples+_overlapSamples);
    int32_t bufByte = 2*_frameByte;
    int32_t bufByteMin = 0;
    bufByte=bufByte<bufByteMin?bufByteMin:bufByte;

    uint8_t* pBuf = (uint8_t*)_port.cb_malloc(bufByte);
    if(!pBuf){
        LOG(_port.cb_printf,"malloc buf fail\n");
        return AUDIO_SPEED_CTR_API_RET_FAIL;
    }
    memset(pBuf, 0, bufByte);
    _iBuf.Init(pBuf,bufByte,&_audioInfo);

    bufByte = _audioInfo.BytePerSample * _overlapSamples + 2 * _frameByte;
    //bufByte += 10*1024;
    pBuf = (uint8_t*)_port.cb_malloc(bufByte);
    if(!pBuf){
        LOG(_port.cb_printf,"malloc buf fail\n");
        _port.cb_free(_iBuf.Buff());
        return AUDIO_SPEED_CTR_API_RET_FAIL;
    }
    memset(pBuf, 0, bufByte);
    _oBuf.Init(pBuf,bufByte,&_audioInfo);
    _oBuf.Append(_audioInfo.BytePerSample * _overlapSamples + 1 * _frameByte);
    return AUDIO_SPEED_CTR_API_RET_SUCCESS;
}

int32_t AudioSpeedCtr_c::Receive(uint8_t* in, int32_t inSize){
    int32_t skipSize = (int32_t)(_speed * (_overlapSamples + _constSamples) * _audioInfo.width * _audioInfo.channels);
    int32_t inOff=0;
    while(1){
        int32_t appendSize = _iBuf.Append(in,inSize-inOff);
        inOff+=appendSize;
        if(_iBuf.Size()<MAX(_frameByte,skipSize)){
            break;
        }
        if(_oBuf.LeftSize()<(_constSamples+_overlapSamples)*_audioInfo.BytePerSample){
            if(inOff<inSize){
                return AUDIO_SPEED_CTR_API_RET_FAIL;
            }
            break;
        }
        int32_t bestLag;
        int32_t offset = 0;
        int32_t outOffset = 0;
#if 0
        bestLag = SeekBestLag((int8_t*)_iBuf.Data(), (int8_t*)&_oBuf.LeftData()[-_overlapSamples*_audioInfo.BytePerSample], _audioInfo.width, _seekSamples, _overlapSamples, _audioInfo.channels);
#else
        bestLag = 0;
#endif
        offset = bestLag * _audioInfo.BytePerSample;
        if (_oBuf.Size() < _overlapSamples * _audioInfo.BytePerSample) {
            int a1 = 1;
        }
#if 1
        overlap((int8_t*)&_oBuf.LeftData()[-_overlapSamples * _audioInfo.BytePerSample], (int8_t*)&_iBuf.Data()[offset], (int8_t*)&_oBuf.LeftData()[-_overlapSamples*_audioInfo.BytePerSample], _overlapSamples, _audioInfo.channels, _audioInfo.width);
#endif
        offset += _overlapSamples*_audioInfo.BytePerSample;

        //copy constSamples from in to out
        memcpy(&_oBuf.LeftData()[0], &_iBuf.Data()[offset], _constSamples*_audioInfo.BytePerSample);
        offset += _constSamples*_audioInfo.BytePerSample;
        outOffset += _constSamples*_audioInfo.BytePerSample;

        //updata bufferTemplate
        memcpy(&_oBuf.LeftData()[_constSamples*_audioInfo.BytePerSample], &_iBuf.Data()[offset], _overlapSamples*_audioInfo.BytePerSample);
        offset += skipSize;
        outOffset += _overlapSamples*_audioInfo.BytePerSample;

        _iBuf.Used(skipSize);
        _iBuf.ClearUsed();
        _oBuf.Append(outOffset);
        printf("skipSize:%d,outOffset:%d\n", skipSize,outOffset);
    }
    return AUDIO_SPEED_CTR_API_RET_SUCCESS;
}

int32_t AudioSpeedCtr_c::Generate(uint8_t* out, int32_t *outSize){
    // if(_oBuf.Size()<outSize){
    //     return AUDIO_SPEED_CTR_API_RET_FAIL;
    // }
    
    int32_t cpByte = _oBuf.Size() - _overlapSamples * _audioInfo.BytePerSample;
    if (cpByte < *outSize)
        int a = 1;
    cpByte=cpByte< *outSize ?cpByte: *outSize;


    memcpy(out,_oBuf.Data(),cpByte);
    _oBuf.Used(cpByte);
    _oBuf.ClearUsed();
    *outSize=cpByte;
    return AUDIO_SPEED_CTR_API_RET_SUCCESS;
}

int32_t AudioSpeedCtr_c::Process(uint8_t* in, int32_t inSize, uint8_t* out, int32_t* outSize) {
    int32_t skipSize = (int32_t)(_speed * (_overlapSamples + _constSamples) * _audioInfo.width * _audioInfo.channels);
    int32_t inOff = 0;
    int32_t outSizeMax = *outSize;
    int32_t outSize0 = (int32_t)((float)inSize / _speed);
    if (outSizeMax < outSize0) {
        return AUDIO_SPEED_CTR_API_RET_FAIL;
    }
    AudioBuff_c outInterface;
    outInterface.Init(out, outSize0, &_audioInfo);
    _oCtr.Init(&_oBuf, &outInterface, _overlapSamples * _audioInfo.BytePerSample);
    while (1) {
        int32_t appendSize = _iBuf.Append(in + inOff, inSize - inOff);
        inOff += appendSize;
        if (_iBuf.Size() < MAX(_frameByte, skipSize)) {
            _iBuf.ClearUsed();
            appendSize = _iBuf.Append(in + inOff, inSize - inOff);
            inOff += appendSize;
            if (_iBuf.Size() < MAX(_frameByte, skipSize)) {
                break;
            }
        }
        if (_oBuf.LeftSize()+ outInterface.LeftSize() < (_constSamples + _overlapSamples) * _audioInfo.BytePerSample) {
            _oBuf.ClearUsed();
            if (_oBuf.LeftSize()+ outInterface.LeftSize() < (_constSamples + _overlapSamples) * _audioInfo.BytePerSample) {
                if (inOff < inSize) {
                    return AUDIO_SPEED_CTR_API_RET_FAIL;
                }
                break;
            }
        }
        int32_t bestLag;
        int32_t offset = 0;
        
#if 0
        bestLag = SeekBestLag((int8_t*)_iBuf.Data(), (int8_t*)&_oBuf.LeftData()[-_overlapSamples * _audioInfo.BytePerSample], _audioInfo.width, _seekSamples, _overlapSamples, _audioInfo.channels);
#else
        bestLag = 0;
#endif
        offset = bestLag * _audioInfo.BytePerSample;
#if 1
        overlap((int8_t*)_oCtr.GetOverlap(), (int8_t*)&_iBuf.Data()[offset], (int8_t*)_oCtr.GetOverlap(), _overlapSamples, _audioInfo.channels, _audioInfo.width);
        /*overlap(
            (int8_t*)&_oBuf.LeftData()[-_overlapSamples*_audioInfo.BytePerSample], 
            (int8_t*)&_iBuf.Data()[offset], 
            (int8_t*)&_oBuf.LeftData()[-_overlapSamples * _audioInfo.BytePerSample],
            _overlapSamples, _audioInfo.channels, _audioInfo.width);*/
#endif
        offset += _overlapSamples * _audioInfo.BytePerSample;

        //copy constSamples from in to out
        _oCtr.Append(&_iBuf.Data()[offset], _constSamples * _audioInfo.BytePerSample);
        offset += _constSamples * _audioInfo.BytePerSample;

        //updata bufferTemplate
        _oCtr.Append(&_iBuf.Data()[offset], _overlapSamples * _audioInfo.BytePerSample);
        offset += skipSize;

        _iBuf.Used(skipSize);
        //printf("skipSize:%d,outOffset:%d\n", skipSize, outOffset);
    }
    _oCtr.AppendFully();
    *outSize = outInterface.Size();
    return AUDIO_SPEED_CTR_API_RET_SUCCESS;
}


void* AudioSpeedCtrApi_c::Create(AudioSpeedControlApiParam_t *param){
    return AudioSpeedCtr_c::Create(param);
}
int32_t AudioSpeedCtrApi_c::Set(void* hd, AudioSpeedCtrApiSet_e choose, void* val){
    return AudioSpeedCtr_c::Set(hd,choose,val);
}
int32_t AudioSpeedCtrApi_c::Get(void* hd, AudioSpeedCtrApiGet_e choose, void* val){
    return AudioSpeedCtr_c::Get(hd,choose,val);
}
int32_t AudioSpeedCtrApi_c::Run(void* hd, uint8_t* in, int32_t inSize, uint8_t*out, int32_t*outSize){
    return AudioSpeedCtr_c::Run(hd,in,inSize,out,outSize);
}
void AudioSpeedCtrApi_c::Destory(void* hd){
    return AudioSpeedCtr_c::Destory(hd);
}
#endif
