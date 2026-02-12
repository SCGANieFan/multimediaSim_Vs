#pragma once
#include"Ape.Inner.basic.h"

#if 1
class BufferRead
{
public:
    BufferRead() {};
    ~BufferRead() {};

public:
    u32 INLINE GetBufferUsed() {
        return sizeUsedByte;
    };
    u32 INLINE GetByteUsed() {
        return sizeUsedByteAlign1;
    }
    u32 INLINE GetBufferRemByte() {
        return sizeRemByte;
    }
    b1 INLINE Init()
    {
        ALGO_MEM_SET(this, 0, sizeof(BufferRead));
        offset = -1;
        return true;
    }
    b1 INLINE SetBufferIn(u8* in, u32 inSize) {
        //check
        if ((!in)
            || (inSize < 0))
            return false;
        buffer = in;
        sizeRemByte = inSize;
        sizeUsedByte = 0;
        sizeUsedByteAlign1 = 0;

        return true;
    }

    INLINE b1 BufferCacheReadByte(u8* readedByte)
    {
        if (offset < 0)
            return false;
        *readedByte = data8[offset];
        offset--;
        return true;
    }

    INLINE b1 BufferCacheUpdata(u8* in, u32 inSize)
    {
        ALGO_MEM_CPY(data8, in, 4);
        offset = 3;
        return true;
    }

    //not saftey
    INLINE u8 BufferReadByte()
    {
        u8 readedByte;
        if (!BufferCacheReadByte(&readedByte))
        {
            BufferCacheUpdata(buffer + sizeUsedByte, sizeRemByte);
            BufferCacheReadByte(&readedByte);
            sizeUsedByte += 4;
            sizeRemByte -= 4;
        }
        sizeUsedByteAlign1++;
        return readedByte;
    }


    u32 INLINE Read(u8 readByte)
    {
        //read
        u32 readedDoubleWord = 0;
        for (u8 i = 0; i < readByte; i++)
        {
            readedDoubleWord <<= 8;
            readedDoubleWord += (u32)BufferReadByte();
        }
        return readedDoubleWord;
    }
public:
    union {
        u32 data32;
        u8 data8[4];
    };
    int8_t offset = -1;
    u8* buffer = 0;
    u32 sizeUsedByte = 0;
    u32 sizeUsedByteAlign1 = 0;
    u32 sizeRemByte = 0;
};
#endif

