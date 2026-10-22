#include "smf_common.h"
#include "SmfPorting.h"
using namespace smf;
template<class T>static inline
T smf_atomic_fetch_add(T& v, T x) {
    int lock = intr_lock();
    auto v0 = v;
    v += x;
    intr_unlock(lock);
    return v0;
}
template<class X>static inline
bool smf_atomic_compare_exchange(void* ptr0, void* expected0, X desired) {
    auto ptr = (volatile X*)ptr0;
    auto expected = (X*)expected0;
    bool rst = false;
    int lock = intr_lock();
    if (*ptr == *expected) {
        *ptr = desired;
        rst = true;
    }
    else {
        *expected = *ptr;
        rst = false;
    }
    intr_unlock(lock);
    return true;
}
extern "C" {
    //WEAK int8_t __atomic_fetch_add_1(int8_t * p, int8_t v, int model) {
    //    return smf_atomic_fetch_add(*p, v);
    //}
    //WEAK int16_t __atomic_fetch_add_2(int16_t * p, int16_t v, int model) {
    //    return smf_atomic_fetch_add(*p, v);
    //}
    //WEAK int32_t __atomic_fetch_add_4(int32_t * p, int32_t v, int model) {
    //    return smf_atomic_fetch_add(*p, v);
    //}
    //WEAK int64_t __atomic_fetch_add_8(int64_t * p, int64_t v, int model) {
    //    return smf_atomic_fetch_add(*p, v);
    //}
    //WEAK int8_t __atomic_fetch_sub_1(int8_t* p, int8_t v, int model) {
    //    return smf_atomic_fetch_add(*p, (int8_t)-v);
    //}
    //WEAK int16_t __atomic_fetch_sub_2(int16_t* p, int16_t v, int model) {
    //    return smf_atomic_fetch_add(*p, (int16_t)-v);
    //}
    //WEAK int32_t __atomic_fetch_sub_4(int32_t* p, int32_t v, int model) {
    //    return smf_atomic_fetch_add(*p, -v);
    //}
    //WEAK int64_t __atomic_fetch_sub_8(int64_t* p, int64_t v, int model) {
    //    return smf_atomic_fetch_add(*p, -v);
    //}
    WEAK bool __atomic_compare_exchange_1(void* ptr, void* expected, void* desired, bool weak, int success_memorder, int failure_memorder) {
        return smf_atomic_compare_exchange(ptr,expected, *(uint8_t*)desired);
    }
    WEAK bool __atomic_compare_exchange_2(void* ptr, void* expected, void* desired, bool weak, int success_memorder, int failure_memorder) {
        return smf_atomic_compare_exchange(ptr, expected, *(uint16_t*)desired);
    }
    WEAK bool __atomic_compare_exchange_4(void* ptr, void* expected, void* desired, bool weak, int success_memorder, int failure_memorder) {
        return smf_atomic_compare_exchange(ptr, expected, *(uint32_t*)desired);
    }
    WEAK bool __atomic_compare_exchange_8(void* ptr, void* expected, void* desired, bool weak, int success_memorder, int failure_memorder) {
        return smf_atomic_compare_exchange(ptr, expected, *(uint64_t*)desired);
    }
}
