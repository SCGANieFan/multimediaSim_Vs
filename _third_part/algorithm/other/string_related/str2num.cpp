struct XLower {
	static constexpr inline char proc(char c) { return (c <= 'Z' && c >= 'A') ? (c + ('a' - 'A')) : c; };
};
struct XOther {
	static constexpr inline char proc(char c) { return c; };
};
constexpr inline bool IsChar(char c) {
	return (c <= 'Z' && c >= 'A') || (c <= 'z' && c >= 'a') || (c <= '9' && c >= '0') || (c == '_') || (c == '.') || (c == ' ');
}
constexpr inline char Lower(char c) { return XLower::proc(c); }//{ return (c <= 'Z' && c >= 'A') ? (c + ('a' - 'A')) : c; };

const uint32_t _FNV_offset_basis = 2166136261U;//0x811c9dc5
const uint32_t _FNV_prime = 16777619U;//0x01000193
static constexpr inline uint32_t Convert(unsigned v, char s) { return s == '-' ? v : ((v ^ (unsigned)Lower(s)) * _FNV_prime); }
static constexpr inline uint32_t Convert(unsigned v, const char* s) { return *s ? Convert(Convert(v, *s), s + 1) : v; }
static constexpr inline uint32_t Convert(const char* s0) { return Convert(_FNV_offset_basis, s0); }
//static constexpr inline uint32_t Convert(const char* s0, const char* s1) { return Convert(Convert(s0), s1); }
//static constexpr inline uint32_t Convert(const char* s0, const char* s1, const char* s2) { return Convert(Convert(s0, s1), s2); }
//static constexpr inline uint32_t Convert(const char* s0, const char* s1, const char* s2, const char* s3) { return Convert(Convert(s0, s1, s2), s3); }
static constexpr inline uint32_t Convert(uint32_t keys) { return keys; }

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

static inline uint64_t fcc64x(const char*& s) {
	return TFcc<uint64_t, XOther>::fccx(s);
}