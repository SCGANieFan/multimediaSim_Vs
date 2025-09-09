#pragma once
#include"MTF.Type.h"
namespace mtf_ns {
class MTF_String
{
public:
	MTF_String();
	~MTF_String();

public:
	static void* MemCpy(mtf_i8* dst, mtf_i8* src, mtf_i32 size);
	static void* MemSet(mtf_i8* dst, mtf_i32 val, mtf_i32 size);
	static void* MemMove(mtf_i8* dst, mtf_i8* src, mtf_i32 size);

	static mtf_i32 StrLen(const char* str);
	static mtf_i32 Search(const char* str, const char* temp, mtf_i32 posStart, mtf_i32 posEnd, mtf_bool dir = true);
	static mtf_i32 Search(const char* str, const char* temp, mtf_bool dir = true);
	static mtf_i32 Cut(const char* str, const char* characteristic, mtf_i8* out, mtf_i32 outByteMax);
	static mtf_i32 Cut(const char* str, const char* characteristic, mtf_i8* out);
	static mtf_i32 StrAppend(mtf_i8* src, mtf_i8* dst);

	static bool StrCompare(const char* cmp0, const char* cmp1);
	static const char* BaseName(const char* str);
private:

};



constexpr mtf_u64 String2ID64NoLose(const char* s, uint16_t n) {
	return (*s == '\0' || n == 0) ? 0 : (((mtf_u64)(*s)) << ((8 - n) << 3)) + String2ID64NoLose(s + 1, n - 1);
}
constexpr mtf_u64 String2ID64NoLose(const char* s) {
	return String2ID64NoLose(s, 8);
}

constexpr mtf_u32 String2ID32NoLose(const char* s, uint16_t n) {
	return (*s == '\0' || n == 0) ? 0 : (((mtf_u32)(*s)) << ((4 - n) << 3)) + String2ID32NoLose(s + 1, n - 1);
}
constexpr mtf_u32 String2ID32NoLose(const char* s) {
	return String2ID32NoLose(s, 4);
}

constexpr mtf_u32 String2ID32Lose(const char* s, uint16_t n) {
	return (*s == '\0' || n == 0) ? 0 : (((mtf_u32)(*s) * 10 * n) << ((n % 3) << 3)) + String2ID32Lose(s + 1, n - 1);
}
constexpr mtf_u32 String2ID32Lose(const char* s) {
	return String2ID32Lose(s, 32);
}


void ID2StringNoLose(mtf_u64 id, char* s, mtf_u16 len);
void ID2StringNoLose(mtf_u32 id, char* s, mtf_u16 len);

#if 1
#define MTF_MEM_CPY(dst,src,size) MTF_String::MemCpy((mtf_i8*)dst,(mtf_i8*)src,(mtf_i32)size)
#define MTF_MEM_SET(dst,val,size) MTF_String::MemSet((mtf_i8*)dst,(mtf_i32)val,(mtf_i32)size)
#define MTF_MEM_MOVE(dst,src,size) MTF_String::MemMove((mtf_i8*)dst,(mtf_i8*)src,(mtf_i32)size)

#define STR2ID64NL(str) String2ID64NoLose(str)
#define STR2ID32NL(str) String2ID32NoLose(str)
#define STR2ID32L(str) String2ID32Lose(str)
#define ID2STRNL(id,s,len) ID2StringNoLose(id,s,len);



#else
#include<string.h>
#define MTF_MCM_CPY(dst,src,size) memcpy(dst,src,size)
#define MTF_MCM_SET(dst,val,size) memset(dst,val,size)
#define MTF_MCM_MOVE(dsr,src,size) memmove(dsr,src,size)
#endif

};





