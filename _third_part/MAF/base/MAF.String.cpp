#include "MAF.String.h"
#include<string.h>
MAF_String::MAF_String()
{
}
MAF_String::~MAF_String()
{
}

maf_void* MAF_String::MemCpy(maf_int8* dst, maf_int8* src, maf_int32 size)
{
	return memcpy(dst, src, size);
}
maf_void* MAF_String::MemSet(maf_int8* dst, maf_int32 val, maf_int32 size)
{
	return memset(dst, val, size);
}
maf_void* MAF_String::MemMove(maf_int8* dst, maf_int8* src, maf_int32 size)
{
	return memmove(dst, src, size);
}


maf_int32 MAF_String::StrLen(const maf_int8* str)
{
	maf_int32 strLen = 0;
	while (*str++ != '\0')
		strLen++;
	return strLen;
}

maf_int32 MAF_String::Search(const maf_int8* str, const maf_int8* temp, maf_int32 posStart, maf_int32 posEnd, maf_bool dir)
{
	str += posStart;
	maf_int32 strLen = posEnd - posStart + 1;
	maf_int32 tempLen = StrLen(temp);
	maf_int32 pos = -1;
	if (dir == true)//left -> right
	{
		for (maf_int32 sl = 0; sl <= strLen; sl++)
		{
			if (str[sl] == temp[0])
			{
				if ((strLen - sl + 1) < tempLen)
				{
					pos = -1;
					break;
				}
				maf_int32 off = 1;
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
		for (maf_int32 sl = strLen-1; sl >= 0; sl--)
		{
			if (str[sl] == temp[tempLen-1])
			{
				if ((sl + 1) < tempLen)
				{
					pos = -1;
					break;
				}
				maf_int32 off = tempLen - 2;
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

maf_int32 MAF_String::Search(const maf_int8* str, const maf_int8* temp, maf_bool dir)
{
	maf_int32 strLen = StrLen(str);
	return Search(str, temp, 0, strLen, dir);
}


maf_int32 MAF_String::Cut(const maf_int8* str, const maf_int8* characteristic, maf_int8* out, maf_int32 outByteMax)
{
	maf_int32	pos = Search(str, characteristic);
	if (pos < 0)
		return -1;
	maf_int32 outByte = pos;
	outByte = outByte > outByteMax ? outByteMax : outByte;
	MAF_MEM_CPY(out, (maf_int8*)str, outByte);
	return pos;
}
maf_int32 MAF_String::Cut(const maf_int8* str, const maf_int8* characteristic, maf_int8* out)
{
	return Cut(str, characteristic, out, 1024);
}

maf_int32 MAF_String::StrAppend(maf_int8* src, maf_int8* dst)
{
	//for
	return 0;
}

maf_bool MAF_String::StrCompare(const maf_int8* cmp0, const maf_int8* cmp1)
{
	
	maf_int32 len0 = StrLen(cmp0);
	maf_int32 len1 = StrLen(cmp1);
	maf_int32 lenMin = len0 > len1 ? len1 : len0;
	for (maf_int32 i = 0; i < lenMin; i++)
	{
		if (*cmp0++ != *cmp1++)
			return false;
	}
	return true;
}

const maf_int8* MAF_String::BaseName(const maf_int8* str)
{
	maf_int32  pos = Search(str, "\\", false);
	return str + pos + 1;
}


