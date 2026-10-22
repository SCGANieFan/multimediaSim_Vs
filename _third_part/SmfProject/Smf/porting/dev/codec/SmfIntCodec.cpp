#include "SmfIntCodec.h"
#include "smf_debug.h"

using namespace smf;

int SmfIntCodec::Start(enum AUD_STREAM_T stream, struct HAL_CODEC_CONFIG_T& codec_cfg) {
    //dbgTestPXL("codec %d start", _id);
    int ret = -1;
    ret = _cbOpen ? _cbOpen() : -1;
    returnIfErrCS(ret, ret, "%d", ret);
    ret = _cbStreamOpen ? _cbStreamOpen(stream) : -1;
    returnIfErrCS(ret, ret, "%d", ret);
    ret = _cbStreamSetup ? _cbStreamSetup(stream, &codec_cfg) : -1;
    returnIfErrCS(ret, ret, "%d", ret);
    ret = _cbStreamStart ? _cbStreamStart(stream) : -1;
    returnIfErrCS(ret, ret, "%d", ret);
    return ret;
}

int SmfIntCodec::Stop(enum AUD_STREAM_T stream) {
    int ret = _cbStreamStop ? _cbStreamStop(stream) : -1;
    returnIfErrCS(ret, ret, "%d", ret);
    ret = _cbStreamClose ? _cbStreamClose(stream) : -1;
    returnIfErrCS(ret, ret, "%d", ret);
    ret = _cbClose ? _cbClose(CODEC_CLOSE_NORMAL) : -1;
    returnIfErrCS(ret, ret, "%d", ret);
    return ret;
}