#pragma once
#include <cstdint>
#include <atomic>
#include "gaapi_base.h"
namespace gaapi_ns {

    class GaapiMutex_c {
    private:
        std::atomic<uint32_t> lock_{ 0 };
    public:
        void Lock();
        void Unlock();
        bool TryLock();
    };

    class GaapiAutoMutex_c {
    public:
        GaapiAutoMutex_c(GaapiMutex_c* mtx) { _mtx = mtx; if (_mtx)_mtx->Lock(); }
        ~GaapiAutoMutex_c() { if (_mtx)_mtx->Unlock(); }
    public:
        GaapiMutex_c* _mtx = 0;
    };
};

