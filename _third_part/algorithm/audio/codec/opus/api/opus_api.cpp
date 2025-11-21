#include <stdint.h>
#include "opus_api_private.h"
#include "opus_api.h"

const static char VERSION[] = "1.0.0.8";

//OpusApiEnc_c
inline OpusApiRet_t OpusApiEnc_c::Create(void** pHd, OpusApi_BasePort_t* basePort, int fs, int channels, bool isWithHead, OpusApi_EncChoose_c choose) {
    if (!basePort) {
        return OPUS_API_RET_FAIL;
    }
    LOG(basePort->print_cb, "opus api enc (%s) %p,%d,%d,%d,%d", VERSION, pHd, fs, channels, isWithHead, choose);
    //check
    if (!pHd ||
        choose >= OpusApi_EncChoose_c::OPUS_API_ENC_CHOOSE_MAX) {
        return OPUS_API_RET_FAIL;
    }
    *pHd = 0;
    int size = sizeof(OpusApiEnc_c);
    OpusApiEnc_c* encApi = (OpusApiEnc_c*)basePort->malloc_cb(size);
    if (!encApi) {
        LOG(basePort->print_cb, "opus api enc malloc fail, %d", size);
        return OPUS_API_RET_FAIL;
    }
    new(encApi) OpusApiEnc_c();
    encApi->_basePort = *basePort;

    uint32_t id = OPUS_ENCODE_TO_ID(choose);
    encApi->_enc = (OpusEnc_c*)OpusCodecCreaterDoCreate(id, &encApi->_basePort);
    if (!encApi->_enc) {
        OpusEncoderNoneRegister();
        encApi->_enc = (OpusEnc_c*)OpusCodecCreaterDoCreate(OPUS_ENCODE_TO_ID(OpusApi_EncChoose_c::OPUS_API_ENC_CHOOSE_NONE), &encApi->_basePort);
        if (!encApi->_enc) {
            LOG(encApi->_basePort.print_cb, "opus api enc choose error, %d", choose);
            return OPUS_API_RET_FAIL;
        }
    }
    OpusApiRet_t ret = encApi->_enc->Init(&encApi->_basePort, fs, channels, isWithHead);
    if (ret != OPUS_API_RET_SUCCESS) {
        Destory(encApi);
        return ret;
    }
    LOG(encApi->_basePort.print_cb, "opus api enc create success, %p, %p", encApi, encApi->_enc);
    *pHd = encApi;
    return OPUS_API_RET_SUCCESS;
}
inline OpusApiRet_t OpusApiEnc_c::Destory(void* hd) {
    if (!hd) {
        return OPUS_API_RET_FAIL;
    }
    OpusApiEnc_c* encApi = (OpusApiEnc_c*)hd;
    OpusApi_BasePort_t basePort = encApi->_basePort;
    LOG(basePort.print_cb, "opus api enc destory %p", hd);
    if (encApi->_enc) {
        encApi->_enc->Deinit();
        basePort.free_cb(encApi->_enc);
    }
    encApi->~OpusApiEnc_c();
    basePort.free_cb(encApi);
    return OPUS_API_RET_SUCCESS;
}
inline OpusApiRet_t OpusApiEnc_c::Set(void* hd, OpusApi_EncSetChhoose_e choose, void* val) {
    if (!hd)
        return OPUS_API_RET_FAIL;
    OpusApiEnc_c* encApi = (OpusApiEnc_c*)hd;
    LOG(encApi->_basePort.print_cb, "opus api enc set %p,%d,%d", encApi, choose, (uint32_t)val);
    return encApi->_enc->Set(choose, val);
}
inline OpusApiRet_t OpusApiEnc_c::Get(void* hd, OpusApi_EncGetChhoose_e choose, void* val) {
    if (!hd)
        return OPUS_API_RET_FAIL;
    OpusApiEnc_c* encApi = (OpusApiEnc_c*)hd;
    LOG(encApi->_basePort.print_cb, "opus api enc get %p,%d,%d", encApi, choose, (uint32_t)val);
    return encApi->_enc->Get(choose, val);
}
inline OpusApiRet_t OpusApiEnc_c::Run(void* hd, short* in, int inSample, unsigned char* out, int* outByte) {
    if (!hd)
        return OPUS_API_RET_FAIL;
    OpusApiEnc_c* encApi = (OpusApiEnc_c*)hd;
    if (!in
        || inSample <= 0
        || !out
        || !outByte
        || *outByte <= 0) {
        LOG(encApi->_basePort.print_cb, "opus api enc, %p,%d,%p,%p", in, inSample, out, outByte);
        return OPUS_API_RET_FAIL;
    }
    return encApi->_enc->Run(in, inSample, out, outByte);
}


//OpusApiDec_c
inline OpusApiRet_t OpusApiDec_c::Create(void** pHd, OpusApi_BasePort_t* basePort, int fs, int channels, OpusApi_DecChoose_c choose) {
    if (!basePort)
        return OPUS_API_RET_FAIL;
    LOG(basePort->print_cb, "opus api dec (%s) %p,%d,%d,%d", VERSION, pHd, fs, channels, choose);
    if (!pHd
        || choose > OpusApi_DecChoose_c::OPUS_API_DEC_CHOOSE_MAX)
        return OPUS_API_RET_FAIL;
    *pHd = 0;
    int size = sizeof(OpusApiDec_c);
    OpusApiDec_c* decApi = (OpusApiDec_c*)basePort->malloc_cb(size);
    if (!decApi) {
        LOG(basePort->print_cb, "opus api dec malloc fail %d", size);
        return OPUS_API_RET_FAIL;
    }
    new(decApi) OpusApiDec_c();
    decApi->_basePort = *basePort;
    uint32_t id = OPUS_DECODE_TO_ID(choose);
    decApi->_dec = (OpusDec_c*)OpusCodecCreaterDoCreate(id, &decApi->_basePort);
    if (!decApi->_dec) {
        OpusDecoderNoneRegister();
        decApi->_dec = (OpusDec_c*)OpusCodecCreaterDoCreate(OPUS_DECODE_TO_ID(OpusApi_DecChoose_c::OPUS_API_DEC_CHOOSE_NONE), &decApi->_basePort);
        if (!decApi->_dec) {
            LOG(decApi->_basePort.print_cb, "opus api dec choose error, %d", choose);
            return OPUS_API_RET_FAIL;
        }
    }
    OpusApiRet_t ret = decApi->_dec->Init(&decApi->_basePort, fs, channels);
    if (ret != OPUS_API_RET_SUCCESS) {
        Destory(decApi);
        return ret;
    }
    *pHd = decApi;
    LOG(decApi->_basePort.print_cb, "opus api dec create success, %p, %p", decApi, decApi->_dec);
    return OPUS_API_RET_SUCCESS;
}
inline OpusApiRet_t OpusApiDec_c::Destory(void* hd) {
    if (!hd)
        return OPUS_API_RET_FAIL;
    OpusApiDec_c* decApi = (OpusApiDec_c*)hd;
    OpusApi_BasePort_t basePort = decApi->_basePort;
    LOG(basePort.print_cb, "opus api dec destory %p", hd);
    if (decApi->_dec) {
        decApi->_dec->Deinit();
        basePort.free_cb(decApi->_dec);
    }
    decApi->~OpusApiDec_c();
    basePort.free_cb(decApi);
    return OPUS_API_RET_SUCCESS;
}
inline OpusApiRet_t OpusApiDec_c::Set(void* hd, OpusApi_DecSetChhoose_e choose, void* val) {
    OpusApiDec_c* decApi = (OpusApiDec_c*)hd;
    LOG(decApi->_basePort.print_cb, "opus api dec set (%p, %d, %d)", decApi, choose, (uint32_t)val);
    return decApi->_dec->Set(choose, val);
}

inline OpusApiRet_t OpusApiDec_c::Get(void* hd, OpusApi_DecGetChhoose_e choose, void* val) {
    OpusApiDec_c* decApi = (OpusApiDec_c*)hd;
    LOG(decApi->_basePort.print_cb, "opus api dec get (%p, %d, %d)", decApi, choose, (uint32_t)val);
    return decApi->_dec->Get(choose, val);
}

inline OpusApiRet_t OpusApiDec_c::Run(void* hd, unsigned char* in, int inByte, short* out, int* outSample, bool isPlc) {
#if 1
    if (!hd) {
        return OPUS_API_RET_FAIL;
    }
    OpusApiDec_c* decApi = (OpusApiDec_c*)hd;
    if (!isPlc) {
        if (!in
            || inByte < 0
            || !out
            || !outSample
            || *outSample <= 0) {
            LOG(decApi->_basePort.print_cb, "opua api dec fail, (%p,%d,%p,%p)", in, inByte, out, outSample);
        }
    }
#endif
    return decApi->_dec->Run(in, inByte, out, outSample, isPlc);
}


EXTERNC{
OpusApiRet_t opus_api_create_encoder(void** pHd, OpusApi_BasePort_t* basePort, int fs, int channels, bool isWithHead, OpusApi_EncChoose_c choose) {
    return OpusApiEnc_c::Create(pHd,basePort,fs,channels,isWithHead,choose);
}

OpusApiRet_t opus_api_destory_encoder(void* hd) {
    return OpusApiEnc_c::Destory(hd);
}

OpusApiRet_t opus_api_encoder_set(void* hd, OpusApi_EncSetChhoose_e choose, void* val) {
    return OpusApiEnc_c::Set(hd,choose,val);
}

OpusApiRet_t opus_api_encoder_get(void* hd, OpusApi_EncGetChhoose_e choose, void* val) {
    return OpusApiEnc_c::Get(hd,choose,val);
}

OpusApiRet_t opus_api_encoder_run(void* hd, short* in, int inSample, unsigned char* out, int* outByte) {
    return OpusApiEnc_c::Run(hd,in,inSample,out,outByte);
}

OpusApiRet_t opus_api_create_decoder(void** pHd, OpusApi_BasePort_t* basePort, int fs, int channels, OpusApi_DecChoose_c choose) {
    return OpusApiDec_c::Create(pHd,basePort,fs,channels,choose);
}

OpusApiRet_t opus_api_destory_decoder(void* hd) {
    return OpusApiDec_c::Destory(hd);
}

OpusApiRet_t opus_api_decoder_set(void* hd, OpusApi_DecSetChhoose_e choose, void* val) {
    return OpusApiDec_c::Set(hd,choose,val);
}

OpusApiRet_t opus_api_decoder_get(void* hd, OpusApi_DecGetChhoose_e choose, void* val) {
    return OpusApiDec_c::Get(hd,choose,val);
}

OpusApiRet_t opus_api_decoder_run(void* hd, unsigned char* in, int inByte, short* out, int* outSample, bool isPlc) {
    return OpusApiDec_c::Run(hd,in,inByte,out,outSample,isPlc);
}

}
