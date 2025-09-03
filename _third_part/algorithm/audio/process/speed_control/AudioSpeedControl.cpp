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
    if(pAudioSpeedCtr->_overlapBuf.Buff()) port.cb_free(pAudioSpeedCtr->_overlapBuf.Buff());
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
    const int32_t seekMs = 20;
    const int32_t overlapMs = 0;
    const int32_t constMs = 30;
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

    //bufByte = _audioInfo.BytePerSample * _overlapSamples + 2 * _frameByte;
    bufByte = 10*1024;
    //bufByte += 10*1024;
    pBuf = (uint8_t*)_port.cb_malloc(bufByte);
    if(!pBuf){
        LOG(_port.cb_printf,"malloc buf fail\n");
        _port.cb_free(_iBuf.Buff());
        return AUDIO_SPEED_CTR_API_RET_FAIL;
    }
    memset(pBuf, 0, bufByte);
    _oBuf.Init(pBuf,bufByte,&_audioInfo);
    //_oBuf.Used(5 * 1204);
    //_oBuf.Append(_audioInfo.BytePerSample * _overlapSamples + 1 * _frameByte);

    bufByte = 2 * 1024;
    pBuf = (uint8_t*)_port.cb_malloc(bufByte);
    if (!pBuf) {
        LOG(_port.cb_printf, "malloc buf fail\n");
        _port.cb_free(_iBuf.Buff());
        _port.cb_free(_oBuf.Buff());
        return AUDIO_SPEED_CTR_API_RET_FAIL;
    }
    memset(pBuf, 0, bufByte);
    _overlapBuf.Init(pBuf, bufByte, &_audioInfo);
    return AUDIO_SPEED_CTR_API_RET_SUCCESS;
}

#if 0
int32_t AudioSpeedCtr_c::Process(uint8_t* in, int32_t inSize, uint8_t* out, int32_t* outSize) {
    int32_t skipSize = (int32_t)(_speed * (_overlapSamples + _constSamples));
    skipSize = skipSize *_audioInfo.width * _audioInfo.channels;
    int32_t inOff = 0;
    int32_t outSizeMax = *outSize;
    //int32_t outSize0 = _audioInfo.BytePerSample * (int32_t)((float)inSize / (_audioInfo.BytePerSample * _speed));
    int32_t outSize0 = outSizeMax;
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
        
#if 1
        //bestLag = SeekBestLag((int8_t*)_iBuf.Data(), (int8_t*)&_oBuf.LeftData()[-_overlapSamples * _audioInfo.BytePerSample], _audioInfo.width, _seekSamples, _overlapSamples, _audioInfo.channels);
        bestLag = SeekBestLag((int8_t*)_iBuf.Data(), (int8_t*)&_oBuf.LeftData()[-_overlapSamples * _audioInfo.BytePerSample], _audioInfo.width, 10, _overlapSamples, _audioInfo.channels);
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
        //printf("skipSize:%d,outOffset:%d,bestLag:%d\n", skipSize, outInterface.Size(), bestLag);
    }
    _oCtr.AppendFully();
    //{printf("%d,%d,%d\n", outSize0,_oBuf.Size(), _oBuf.LeftSize()); }
    *outSize = outInterface.Size();
    return AUDIO_SPEED_CTR_API_RET_SUCCESS;
}
#else

int32_t AudioSpeedCtr_c::SpeedUp(AudioBuff_c* buf) {
    int32_t appendSize = 100;
    buf->Throw(appendSize);
    return 0;
}
int32_t AudioSpeedCtr_c::SpeedDown(AudioBuff_c* buf, int32_t airSize) {
#if 0
    int32_t appendSize = 30*4;
    if (buf->LeftSize()<appendSize) {
        buf->Clear(buf->Used()>>1);
        if (buf->LeftSize() < appendSize) {
            printf("(%s)[%d] err\n", __func__, __LINE__);
            return AUDIO_SPEED_CTR_API_RET_FAIL;
        }
    }
    //memcpy(buf->LeftData(), 0, appendSize);
    memset(buf->LeftData(), 0, appendSize);
    buf->Append(appendSize);
    return AUDIO_SPEED_CTR_API_RET_SUCCESS;
    //return AUDIO_SPEED_CTR_API_RET_FAIL;
#else
#define FS 16000
#define SAMPLE_MS (FS/1000)
#define CH 1
#define WITCH 2

#define MATCH_MS 10
#define MATCH_SAMPLE (MATCH_MS*SAMPLE_MS)
#define MATCH_BYTE (MATCH_SAMPLE*CH*WITCH)

#define OVERLAP_MS 4
#define OVERLAP_SAMPLE (OVERLAP_MS*SAMPLE_MS)
#define OVERLAP_BYTE (OVERLAP_SAMPLE *CH*WITCH)

#define SEEK_MS_MIN (3)
#define SEEK_MS_MAX (30)
#define SEEK_SAMPLE_MIN (SEEK_MS_MIN*SAMPLE_MS)
#define SEEK_SAMPLE_MAX (SEEK_MS_MAX*SAMPLE_MS)
#define SEEK_BYTE_MIN (SEEK_SAMPLE_MIN*CH*WITCH)
#define SEEK_BYTE_MAX (SEEK_SAMPLE_MAX*CH*WITCH)

    if (_oBuf.Size() > airSize) {
        printf("(%s)[%d] err\n", __func__, __LINE__);
        return AUDIO_SPEED_CTR_API_RET_SUCCESS;
    }

    if (_oBuf.Used() < 2 * 1024) {
        printf("(%s)[%d] err\n", __func__, __LINE__);
        return AUDIO_SPEED_CTR_API_RET_FAIL;
    }
    if (_oBuf.Size() < MATCH_BYTE) {
        printf("(%s)[%d] err\n", __func__, __LINE__);
        return AUDIO_SPEED_CTR_API_RET_FAIL;
    }
    
    int32_t bestLag = 0;
#if 1
    bestLag = SeekBestLag(
        (int8_t*)&_oBuf.Data()[-SEEK_BYTE_MAX],
        (int8_t*)_oBuf.Data(),
        _audioInfo.width, 
        SEEK_SAMPLE_MAX - SEEK_SAMPLE_MIN,
        MATCH_SAMPLE,
        _audioInfo.channels);
    bestLag += 1;
#else
    bestLag = 0;
#endif
    int32_t lag0 = SEEK_BYTE_MAX - bestLag*CH* WITCH;
    //printf("(%s)[%d] lag0:%d\n", __func__, __LINE__, lag0);

    int32_t appendSize = lag0;
    //int32_t appendSize = 100;
    //appendSize = appendSize < buf->LeftSize() ? appendSize : buf->LeftSize();

    if (buf->LeftSize() < appendSize) {
        buf->Clear(buf->Used() >> 1);
        if (buf->LeftSize() < appendSize) {
            printf("(%s)[%d] err\n", __func__, __LINE__);
            return AUDIO_SPEED_CTR_API_RET_FAIL;
        }
    }
    int32_t enpandOff = -lag0 + buf->Size();


    _overlapBuf.Append(&buf->LeftData()[-OVERLAP_BYTE], OVERLAP_BYTE);
#if 1
    overlap(
        (int8_t*)&buf->LeftData()[-OVERLAP_BYTE],
        (int8_t*)&buf->Data()[enpandOff - OVERLAP_BYTE],
        (int8_t*)&buf->LeftData()[-OVERLAP_BYTE],
        OVERLAP_SAMPLE, _audioInfo.channels, _audioInfo.width);
#endif
    //memcpy(buf->LeftData(), &buf->Data()[enpandOff], appendSize);
    for (int32_t i = 0; i < appendSize; i++) {
        buf->LeftData()[i] = buf->Data()[enpandOff + i];
    }
    buf->Append(appendSize);
    
    while (buf->Size() < airSize)
    {
        if (buf->LeftSize() < appendSize) {
            buf->Clear(buf->Used() >> 1);
            if (buf->LeftSize() < appendSize) {
                printf("(%s)[%d] err\n", __func__, __LINE__);
                return AUDIO_SPEED_CTR_API_RET_FAIL;
            }
        }
        enpandOff += appendSize;
        //memcpy(buf->LeftData(), &buf->Data()[enpandOff], appendSize);
        for (int32_t i = 0; i < appendSize; i++) {
            buf->LeftData()[i] = buf->Data()[enpandOff + i];
        }
        //memset(buf->LeftData(), 0, appendSize);
        buf->Append(appendSize);
    }
    memcpy(&buf->LeftData()[-OVERLAP_BYTE], _overlapBuf.Data(), OVERLAP_BYTE);
    _overlapBuf.Used(_overlapBuf.Size());
    _overlapBuf.ClearUsed();
    
    return AUDIO_SPEED_CTR_API_RET_SUCCESS;
#endif
}

int32_t AudioSpeedCtr_c::Process(uint8_t* in, int32_t inSize, uint8_t* out, int32_t* outSize) {
#if 1
    int32_t outSizeMax = *outSize;
    int32_t outSize0 = _audioInfo.BytePerSample * (int32_t)((float)inSize / (_audioInfo.BytePerSample * _speed));
    if (outSizeMax < outSize0) {
        printf("err-1\n");
        return AUDIO_SPEED_CTR_API_RET_FAIL;
    }

    
    if (_oBuf.LeftSize() < inSize){
        //_oBuf.Clear(_oBuf.Used() - 5 * 1024);
        _oBuf.Clear(_oBuf.Used()>>1);
        if (_oBuf.LeftSize() < inSize) {
            printf("err0\n");
            return AUDIO_SPEED_CTR_API_RET_FAIL;
        }
    }
    //_oBuf.Append
    int32_t appendSize = _oBuf.Append(in, inSize);
    if (appendSize < inSize) {
        printf("(%s)[%d] err\n", __func__, __LINE__);
    }
    if (_speed > 1) {
        while (_oBuf.Size() > outSize0 + 1024) {
            SpeedUp(&_oBuf);
        }
    }
    else if (_speed < 1) {
        SpeedDown(&_oBuf, outSize0);
        
#if 0
        if () {
            printf("err1\n");
            return AUDIO_SPEED_CTR_API_RET_FAIL;
        }
#endif
    }
    if (_oBuf.Size() < outSize0) {
        *outSize = 0;
    }
    else {
#if 0
        static int flag = 0;
        if (flag) {
            *outSize = 0;
            return AUDIO_SPEED_CTR_API_RET_SUCCESS;
        }
        flag = 1;
#endif
        memcpy(out, _oBuf.Data(), outSize0);
        _oBuf.Used(outSize0);
        *outSize = outSize0;
    }
    return AUDIO_SPEED_CTR_API_RET_SUCCESS;
#else
    int32_t outSizeMax = *outSize;
    int32_t outSize0 = _audioInfo.BytePerSample * (int32_t)((float)inSize / (_audioInfo.BytePerSample * _speed));
    memcpy(out,in, inSize);
    memset(out+ inSize, 0, outSize0-inSize);
    *outSize = outSize0;
    return AUDIO_SPEED_CTR_API_RET_SUCCESS;
#endif
}

#endif
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
