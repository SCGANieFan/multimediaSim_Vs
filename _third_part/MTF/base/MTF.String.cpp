#include "MTF.String.h"
using namespace mtf_ns;
#if 1
static inline void memset_add64_inner(mtf_u32* p32, mtf_u32 v8, mtf_i32 length) {
	mtf_i32 lengthFrac = length & 15;
	mtf_i32 lengthInt = length - lengthFrac;
	for (mtf_i32 i = 0; i < lengthInt; i += 16) {
		*p32++ = 0;
		*p32++ = 0;
		*p32++ = 0;
		*p32++ = 0;
	}
	if (lengthFrac) {
		mtf_u8* p8 = (mtf_u8*)p32;
		for (mtf_i32 i = lengthInt; i < length; i += 1) {
			*p8++ = 0;
		}
	}
}

static void* memset_m(void* dst, mtf_u8 val, mtf_i32 length) {
	mtf_u32 addRem8 = (mtf_u32)dst & 7;
	if (addRem8 == 0) {
		memset_add64_inner((mtf_u32*)dst, val, length);
	}
	else {
		mtf_u8* p8 = (mtf_u8*)dst;
		mtf_i32 pendLen = 8 - addRem8;
		if (length > pendLen) {
			for (mtf_i32 i = 0; i < pendLen; i += 1) {
				*p8++ = val;
			}
			mtf_u32* p32 = (mtf_u32*)p8;
			length -= pendLen;
			memset_add64_inner(p32, val, length);
		}
		else {
			for (mtf_i32 i = 0; i < length; i += 1) {
				*p8++ = val;
			}
		}
	}
	return 0;
}

static void* memcpy_m(void* dst, const void* src, mtf_i32 length) {
	mtf_u32 addDstRem4 = (mtf_u32)dst & 3;
	mtf_u32 addSrcRem4 = (mtf_u32)src & 3;
	if (addDstRem4
		|| addSrcRem4) {
		const mtf_u8* pSrc8 = (const mtf_u8*)src;
		mtf_u8* pDst8 = (mtf_u8*)dst;
		for (mtf_i32 i = 0; i < length; i++) {
			*pDst8++ = *pSrc8++;
		}
	}
	else {
		const mtf_u32* pSrc32 = (const mtf_u32*)src;
		mtf_u32* pDst32 = (mtf_u32*)dst;
		mtf_i32 lengthFrac = length & 15;
		mtf_i32 lengthInt = length - lengthFrac;
		for (mtf_i32 i = 0; i < lengthInt; i += 16) {
			*pDst32++ = *pSrc32++;
			*pDst32++ = *pSrc32++;
			*pDst32++ = *pSrc32++;
			*pDst32++ = *pSrc32++;
		}
		if (lengthFrac) {
			mtf_u8* pSrc8 = (mtf_u8*)pSrc32;
			mtf_u8* pDst8 = (mtf_u8*)pDst32;
			for (mtf_i32 i = lengthInt; i < length; i += 1) {
				*pDst8++ = *pSrc8++;
			}
		}
	}
	return 0;
}

static inline void* memcpy_reverse_inner(void* dst, const void* src, mtf_i32 length) {
	mtf_u8* pSrc8 = (mtf_u8*)src;
	mtf_u8* pDst8 = (mtf_u8*)dst;
	mtf_i32 diffLen = pDst8 - pSrc8;
	pSrc8 += length - 1;
	pDst8 += length - 1;
	for (mtf_i32 i = 0; i < length; i++) {
		*pDst8-- = *pSrc8--;
	}
	return 0;
}

static void* memmove_m(void* dst, const void* src, mtf_i32 length) {
	if (src >= dst) {
		memcpy_m(dst, src, length);
	}
	else {
		memcpy_reverse_inner(dst, src, length);
	}
	return 0;
}

#endif


void* MTF_String::MemCpy(mtf_i8* dst, mtf_i8* src, mtf_i32 size)
{
	return memcpy_m(dst, src, size);
}
void* MTF_String::MemSet(mtf_i8* dst, mtf_i32 val, mtf_i32 size)
{
	return memset_m(dst, val, size);
}
void* MTF_String::MemMove(mtf_i8* dst, mtf_i8* src, mtf_i32 size)
{
	return memmove_m(dst, src, size);
}


mtf_i32 MTF_String::StrLen(const char* str)
{
	mtf_i32 strLen = 0;
	while (*str++ != '\0')
		strLen++;
	return strLen;
}

mtf_i32 MTF_String::Search(const char* str, const char* temp, mtf_i32 posStart, mtf_i32 posEnd, mtf_bool dir)
{
	str += posStart;
	mtf_i32 strLen = posEnd - posStart + 1;
	mtf_i32 tempLen = StrLen(temp);
	mtf_i32 pos = -1;
	if (dir == true)//left -> right
	{
		for (mtf_i32 sl = 0; sl <= strLen; sl++)
		{
			if (str[sl] == temp[0])
			{
				if ((strLen - sl + 1) < tempLen)
				{
					pos = -1;
					break;
				}
				mtf_i32 off = 1;
				for (off = 1; off < tempLen; off++)
				{
					if (str[sl + off] != temp[off])
					{
						break;
					}
				}
				if (off == tempLen)
				{
					pos = sl;
					break;
				}
			}
		}
	}
	else
	{
		for (mtf_i32 sl = strLen-1; sl >= 0; sl--)
		{
			if (str[sl] == temp[tempLen-1])
			{
				if ((sl + 1) < tempLen)
				{
					pos = -1;
					break;
				}
				mtf_i32 off = tempLen - 2;
				for (off = tempLen-2; off >= 0; off--)
				{
					if (str[sl + off - (tempLen - 1)] != temp[off])
					{
						break;
					}
				}
				if (off == -1)
				{
					pos = sl - (tempLen - 1);
					break;
				}
			}
		}
	}
	return pos + posStart;
}

mtf_i32 MTF_String::Search(const char* str, const char* temp, mtf_bool dir)
{

	mtf_i32 strLen = StrLen(str);
	return Search(str, temp, 0, strLen, dir);
}


mtf_i32 MTF_String::Cut(const char* str, const char* characteristic, mtf_i8* out, mtf_i32 outByteMax)
{
	mtf_i32	pos = Search(str, characteristic);
	if (pos < 0)
		return -1;
	mtf_i32 outByte = pos;
	outByte = outByte > outByteMax ? outByteMax : outByte;
	MTF_MEM_CPY(out, (mtf_i8*)str, outByte);
	return pos;
}
mtf_i32 MTF_String::Cut(const char* str, const char* characteristic, mtf_i8* out)
{
	return Cut(str, characteristic, out, 1024);
}

mtf_i32 MTF_String::StrAppend(mtf_i8* src, mtf_i8* dst)
{
	//for
	return 0;
}

bool MTF_String::StrCompare(const char* cmp0, const char* cmp1)
{
	
	mtf_i32 len0 = StrLen(cmp0);
	mtf_i32 len1 = StrLen(cmp1);
	mtf_i32 lenMin = len0 > len1 ? len1 : len0;
	for (mtf_i32 i = 0; i < lenMin; i++)
	{
		if (*cmp0++ != *cmp1++)
			return false;
	}
	return true;
}

const char* MTF_String::BaseName(const char* str)
{
	mtf_i32  pos = Search(str, "\\", false);
	return str + pos + 1;
}


namespace mtf_ns {
	void ID2StringNoLose(mtf_u64 id, char* s, mtf_u16 len) {
		if (len < 8) {
			s[0] = '\0';
		}
		MTF_MEM_CPY(s, &id, 8);
	}
	void ID2StringNoLose(mtf_u32 id, char* s, mtf_u16 len) {
	}
}