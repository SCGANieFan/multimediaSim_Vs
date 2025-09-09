#include "MTF.String.h"
#include<string.h>
using namespace mtf_ns;
MTF_String::MTF_String()
{
}
MTF_String::~MTF_String()
{
}

void* MTF_String::MemCpy(mtf_i8* dst, mtf_i8* src, mtf_i32 size)
{
	return memcpy(dst, src, size);
}
void* MTF_String::MemSet(mtf_i8* dst, mtf_i32 val, mtf_i32 size)
{
	return memset(dst, val, size);
}
void* MTF_String::MemMove(mtf_i8* dst, mtf_i8* src, mtf_i32 size)
{
	return memmove(dst, src, size);
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