#pragma once
#include <stdint.h>
#include "smf_common.h"
#include "Element.h"

namespace smf
{
    class TimeSync
    {
    protected:
        void reset();
        uint32_t sync();
    protected:
        uint64_t _ms0 = 0;
        smf_fraction_t _scale;
        int _threshold = 15;
		uint32_t _fcount = 0;
        bool _reset = false;
    };
} // namespace smf
