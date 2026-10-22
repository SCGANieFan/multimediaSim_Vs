#pragma once
#include <stdint.h>
#include "codec_int.h"

namespace smf {
    class SmfIntCodec {
    public:
        using cb_open_t = uint32_t (*)(void);
        using cb_close_t = uint32_t (*)(enum CODEC_CLOSE_TYPE_T);
        using cb_stream_op_t = uint32_t (*)(enum AUD_STREAM_T);
        using cb_stream_open_t = cb_stream_op_t;
        using cb_stream_close_t = cb_stream_op_t;
        using cb_stream_start_t  = cb_stream_op_t;
        using cb_stream_stop_t  = cb_stream_op_t;
        using cb_stream_setup_t = uint32_t (*)(enum AUD_STREAM_T, struct HAL_CODEC_CONFIG_T*);
    public:
        SmfIntCodec():_id(0), _cbOpen(nullptr), _cbClose(nullptr), _cbStreamOpen(nullptr), _cbStreamClose(nullptr), _cbStreamStart(nullptr), _cbStreamStop(nullptr), _cbStreamSetup(nullptr) {}
        SmfIntCodec(int id,
            cb_open_t cbOpen,
            cb_close_t cbClose,
            cb_stream_open_t cbStreamOpen,
            cb_stream_close_t cbStreamClose,
            cb_stream_start_t cbStreamStart,
            cb_stream_stop_t cbStreamStop,
            cb_stream_setup_t cbStreamSetup
        ): _id(id), _cbOpen(cbOpen), _cbClose(cbClose), _cbStreamOpen(cbStreamOpen), _cbStreamClose(cbStreamClose), _cbStreamStart(cbStreamStart), _cbStreamStop(cbStreamStop), _cbStreamSetup(cbStreamSetup) {}
    public:
        int Start(enum AUD_STREAM_T stream, struct HAL_CODEC_CONFIG_T& codec_cfg);
        int Stop(enum AUD_STREAM_T stream);
        operator bool() const { return ( _id > 0 ); }
    public:
        int _id;
        cb_open_t _cbOpen;
        cb_close_t _cbClose;
        cb_stream_open_t _cbStreamOpen;
        cb_stream_close_t _cbStreamClose;
        cb_stream_start_t _cbStreamStart;
        cb_stream_stop_t _cbStreamStop;
        cb_stream_setup_t _cbStreamSetup;
    };
}
