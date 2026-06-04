#pragma once
#include "SmfString.h"
#ifndef __FILE_NAME__
#define __FILE_NAME__ __FILE__
#endif
namespace smf {
	const uint32_t _FNV_offset_basis = 2166136261U;//0x811c9dc5
	const uint32_t _FNV_prime = 16777619U;//0x01000193
	///
	constexpr inline uint32_t Hash(uint32_t v, char s) {
		return (v ^ (uint32_t)Lower(s)) * _FNV_prime;
	}
	///
	constexpr inline uint32_t Hash(uint32_t v, const char* s) {
		return *s ? Hash(Hash(v, *s), s + 1) : v;
	}
	constexpr inline uint32_t Hash(const char* s) {
		return s ? Hash(_FNV_offset_basis, s) : 0;
	}
	constexpr inline uint32_t Hash(const char* s0, const char* s1) {
		return Hash(Hash(s0),s1);
	}
	constexpr inline uint32_t Hash(const char* s0, const char* s1, const char* s2) {
		return Hash(Hash(s0, s1),s2);
	}
	constexpr inline uint32_t Hash(const char* s0, const char* s1, const char* s2, const char* s3) {
		return Hash(Hash(s0, s1, s2),s3);
	}
	constexpr inline uint32_t Hash(const char* s0, const char* s1, const char* s2, const char* s3, const char* s4) {
		return Hash(Hash(s0, s1, s2, s3), s4);
	}
	constexpr inline uint32_t Hash(const char* s0, const char* s1, const char* s2, const char* s3, const char* s4, const char* s5) {
		return Hash(Hash(s0, s1, s2, s3, s4), s5);
	}
	constexpr inline uint32_t Hash(const char* s0, const char* s1, const char* s2, const char* s3, const char* s4, const char* s5, const char* s6) {
		return Hash(Hash(s0, s1, s2, s3, s4, s5), s6);
	}
	constexpr inline uint32_t Hash(const char* s0, const char* s1, const char* s2, const char* s3, const char* s4, const char* s5, const char* s6, const char* s7) {
		return Hash(Hash(s0, s1, s2, s3, s4, s5, s6), s7);
	}
	constexpr inline uint32_t Hash(const char* s0, const char* s1, const char* s2, const char* s3, const char* s4, const char* s5, const char* s6, const char* s7, const char* s8) {
		return Hash(Hash(s0, s1, s2, s3, s4, s5, s6, s7), s8);
	}
	///
	constexpr inline uint16_t Hash16(const char* s) {
		return (Hash(s) >> 16) ^ (Hash(s) & 0xffff);
	}
	///
	constexpr inline uint32_t Hash(uint32_t v, const char* s, char end) {
		return (*s && *s != end) ? Hash(Hash(v, *s), s + 1, end) : v;
	}
	constexpr inline uint32_t Hash(const char* s, char end) {
		return s ? Hash(_FNV_offset_basis, s, end) : 0;
	}
	///
	constexpr inline uint32_t Hash(uint32_t v, const char* s, int max) {
		return (*s && max) ? Hash(Hash(v, *s), s + 1, max - 1) : v;
	}
	constexpr inline uint32_t Hash(const char* s, int max) {
		return (s && max) ? Hash(_FNV_offset_basis, s, max) : 0;
	}
	///
	constexpr inline uint32_t Hash(uint32_t v, const char* s, char end, int max) {
		return (*s && max && *s != end) ? Hash(Hash(v, *s), s + 1, end, max - 1) : v;
	}
	constexpr inline uint32_t Hash(const char* s, char end, int max) {
		return (s && max) ? Hash(_FNV_offset_basis, s, end, max) : 0;
	}
	///
	constexpr inline uint32_t Hash(uint32_t s) {
		return s;
	}
	constexpr inline uint32_t Hash(char* s) {
		return Hash((const char*)s);
	}
	constexpr inline uint32_t Hash(uint32_t v, const uint8_t* s, int size) {
		return size ? Hash(Hash(v, (char)*s), s + 1, size - 1) : v;
	}
	constexpr inline uint32_t Hash(const void* p, int size) {
		return Hash(_FNV_offset_basis,(const uint8_t*)p, size);
	}
	template<class T>
	constexpr inline uint32_t Hash(const T& p) {
		return Hash(0, (const uint8_t*)&p, sizeof(T));
	}
	static inline uint32_t Hash(uint64_t s) {
		return Hash((const char*)&s);
	}
	///
	static inline uint32_t HashX(uint32_t v, const char*& s, char end) {
		auto c = *s++;
		return (c && c != end) ? HashX(Hash(v, c), s, end) : v;
	}
	static inline uint32_t HashX(const char*& s, char end) {
		return s ? HashX(_FNV_offset_basis, s, end) : 0;
	}
	///
	static inline uint32_t HashX(uint32_t v, const char*& s, int max) {
		auto c = *s++;
		return (c && max) ? HashX(Hash(v, c), s, max - 1) : v;
	}
	static inline uint32_t HashX(const char*& s, int max) {
		return (s && max) ? HashX(_FNV_offset_basis, s, max) : 0;
	}
	///
	static inline uint32_t HashX(uint32_t v, const char*& s, char end, int max) {
		auto c = *s++;
		return (c && max && c != end) ? HashX(Hash(v, c), s, end, max - 1) : v;
	}
	static inline uint32_t HashX(const char* s, int& c, char end, int max) {
		return (s && max) ? HashX(_FNV_offset_basis, s, end, max) : 0;
	}
	///
	constexpr inline uint32_t Location32(const char* file, uint32_t line) {
		return ((Hash(file) & 0xffff0000) | line);
	}
	constexpr inline uint32_t Location32() {
		return Location32(__FILE_NAME__,__LINE__);
	}
}

