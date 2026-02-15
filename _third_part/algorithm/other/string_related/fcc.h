#pragma once
#include "m_string.h"
//#define fcc4(c0,c1,c2,c3) (((unm_string.higned)c0)|((unsigned)c1<<8)|((unsigned)c2<<16)|((unsigned)c3<<24))
//#define fcc4x(c) fcc4(c[0],c[1],c[2],c[3])
template<class T, class X = XOther>
struct TFcc {
    static constexpr inline T fcc(T v, char c) {
        return (v << 8) | X::proc(c);
    }
    static constexpr inline T fcc(const char* s, T v = 0, uint32_t i = 0) {
        return (s && (i < sizeof(T)) && IsChar(*s)) ? fcc(fcc(s + 1, v, i + 1), *s) : 0;
    }
    static constexpr inline T fccall(const char* s, T v = 0, uint32_t i = 0) {
        return (s && (i < sizeof(T))) ? fcc(fccall(s + 1, v, i + 1), *s) : 0;
    }

    //static T fccx(const char*& s, T v = 0, int i = 0) {
    //	char c;
    //	return (s && (i < sizeof(T)) && (c = *s) && IsChar(c)) ? fcc(fccx(++s, v, i + 1), c) : 0;
    //}
    static T fccx(const char*& s) {
        T v = 0;
        if (s) {
            for (uint32_t i = 0; i < sizeof(T); i++) {
                if (!IsChar(*s))break;
                v |= ((T)X::proc(*s)) << (i << 3);
                s++;
            }
        }
        return v;
    }
    static int fccx(const char*& str, T* vals, int max, char splitchr = '-') {
        for (uint32_t i = 0; i < max; i++) {
            vals[i] = fccx(str);
            if (!str || !*str || *str != splitchr)
                return i + 1;
            str++;
        }
        return max;
    }
};
//
constexpr uint32_t fcc32(const char*s) {
    return TFcc<uint32_t, XOther>::fcc(s);
}
constexpr uint32_t fcc32up(const char*s) {
    return TFcc<uint32_t, XUpper>::fcc(s);
}
constexpr uint32_t fcc32low(const char*s) {
    return TFcc<uint32_t, XLower>::fcc(s);
}
constexpr uint64_t fcc64(const char* s) {
    return TFcc<uint64_t, XOther>::fcc(s);
}
constexpr uint64_t fcc64up(const char*s) {
    return TFcc<uint64_t, XUpper>::fcc(s);
}
constexpr uint64_t fcc64low(const char*s) {
    return TFcc<uint64_t, XLower>::fcc(s);
}

static inline uint32_t fcc32x(const char*& s) {
    return TFcc<uint32_t, XOther>::fccx(s);
}
static inline uint64_t fcc64x(const char*& s) {
    return TFcc<uint64_t, XOther>::fccx(s);
}

constexpr uint32_t fccall32(const char* s) {
    return TFcc<uint32_t, XOther>::fccall(s);
}
constexpr uint32_t fccall32up(const char* s) {
    return TFcc<uint32_t, XUpper>::fccall(s);
}
constexpr uint32_t fccall32low(const char* s) {
    return TFcc<uint32_t, XLower>::fccall(s);
}
constexpr uint64_t fccall64(const char* s) {
    return TFcc<uint64_t, XOther>::fccall(s);
}
constexpr uint64_t fccall64up(const char* s) {
    return TFcc<uint64_t, XUpper>::fccall(s);
}
constexpr uint64_t fccall64low(const char* s) {
    return TFcc<uint64_t, XLower>::fccall(s);
}
