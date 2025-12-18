#include "gaapi_os.h"
using namespace plc_gaapi_ns;

void GaapiMutex_c::Lock() {
    uint32_t backoff = 1;
    const uint32_t MAX_BACKOFF = 128;
    while (true) {
        if (lock_.exchange(1, std::memory_order_acquire) == 0) {
            break;
        }
#if defined(__CC_ARM) || defined(__ARMCC_VERSION)
#if defined(__GNUC__)
        __asm__ __volatile__("yield" ::: "memory");
#else
        __yield();
#endif
#elif defined(__XTENSA__) && (defined(__GNUC__) || defined(__clang__))
        __asm__ __volatile__("memw" ::: "memory");
#elif defined(__GNUC__) || defined(__clang__)
        __asm__ __volatile__("yield" ::: "memory");
#elif defined(_MSC_VER)
        _mm_pause();
#endif

        backoff = (backoff * 2 < MAX_BACKOFF) ? backoff * 2 : MAX_BACKOFF;
        for (uint32_t i = 0; i < backoff; i++) {
#if defined(__XTENSA__) && (defined(__GNUC__) || defined(__clang__))
            __asm__ __volatile__("nop");
#elif defined(__CC_ARM) && !defined(__GNUC__)
            __nop();
#elif defined(__GNUC__) || defined(__clang__)
            __asm__ __volatile__("nop");
#endif
        }
    }
}

void GaapiMutex_c::Unlock() {
    lock_.store(0, std::memory_order_release);
#if defined(__CC_ARM) || defined(__ARMCC_VERSION)
    __dsb(0xF);
#elif defined(__XTENSA__) && (defined(__GNUC__) || defined(__clang__))
    __asm__ __volatile__("memw" ::: "memory");
#elif defined(__GNUC__) || defined(__clang__)
    __asm__ __volatile__("dsb sy" ::: "memory");
#endif
}

bool GaapiMutex_c::TryLock() {
    return lock_.exchange(1, std::memory_order_acquire) == 0;
}


