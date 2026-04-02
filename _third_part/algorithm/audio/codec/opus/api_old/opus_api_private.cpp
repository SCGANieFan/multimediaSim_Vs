#include <stdint.h>
#include "opus_api_private.h"
#include "opus_api.h"

void* OpusApiBase_c::operator new(size_t size, void* buf) {
    return buf;
}
void OpusApiBase_c::operator delete(void* buf, size_t size) {
    return;
}
void OpusApiBase_c::operator delete(void* buf, void* place) {
    return;
}
void OpusApiBase_c::operator delete(void* buf) {
    return;
}

struct OpusCreater_t {
    int id;
    void* (*creater)(OpusApi_BasePort_t* bp);
};

static OpusCreater_t creatersList[10];

void OpusCodecCreaterRegister(OpusCreater_t* opusCreater, uint32_t id, void* (*creater)(OpusApi_BasePort_t* bp)) {
    opusCreater->id = id;
    opusCreater->creater = creater;
}

OpusCreater_t* OpusCodecCreaterFind(uint32_t id) {
    for (uint8_t n = 0; n < sizeof(creatersList) / sizeof(creatersList[0]); n++) {
        if (id == creatersList[n].id) {
            return &creatersList[n];
        }
    }
    return 0;
}
void* OpusCodecCreaterDoCreate(uint32_t id, OpusApi_BasePort_t* bp) {
    OpusCreater_t* cl = OpusCodecCreaterFind(id);
    if (!cl) return 0;
    return cl->creater(bp);
}

void OpusEncoderNoneRegister() {
    OpusCodecCreaterRegister<OpusEnc_c>(OPUS_ENCODE_TO_ID(OpusApi_EncChoose_e::OPUS_API_ENC_CHOOSE_NONE));
}

void OpusDecoderNoneRegister() {
    OpusCodecCreaterRegister<OpusDec_c>(OPUS_DECODE_TO_ID(OpusApi_DecChoose_e::OPUS_API_DEC_CHOOSE_NONE));
}








