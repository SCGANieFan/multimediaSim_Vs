#pragma once
#include"Ape.Inner.basic.h"

typedef struct {
    i32 nblocks;
    i32 skip;
} ApeFrameInfo;

typedef struct {
    u8 magic[4];
    i16 fileversion;
    i16 padding;
    u32 descriptorlength;
    u32 headerlength;
    u32 seektablelength;
    u32 wavheaderlength;
    u32 audiodatalength;
    u32 audiodatalength_high;
    u32 wavtaillength;
    u8 md5[16];
} ApeDescriptor;

typedef struct {
    u16 compressiontype;
    u16 formatflags;
    u32 blocksperframe;
    u32 finalframeblocks;
    u32 totalframes;
    u16 bps;
    u16 channels;
    u32 samplerate;
} ApeHeader;

typedef struct {
    /* Info from Descriptor Block */
    ApeDescriptor descriptor;
    /* Info from Header Block */
    ApeHeader header;
} ApeContext_t;

class ApeContext
{
public:
    ApeContext() {};
    ~ApeContext() {};
public:
    const STATIC INLINE i32 GetContextSize() { return sizeof(ApeContext_t); }
public:
    INLINE u16 GetChannels() { return _context.header.channels; }
    INLINE u16 GetCompressType() { return _context.header.compressiontype; }
    INLINE i16 GetFileVersion() { return _context.descriptor.fileversion; }
    INLINE ApeHeader* GetHeader() { return &_context.header; }
    INLINE ApeDescriptor* GetDescriptor() { return &_context.descriptor; }
    INLINE ApeContext_t* GetContext() { return &_context; }
    INLINE b1 Init() {
        ALGO_MEM_SET(this, 0, sizeof(this));
        return true;
    };
public:
#if 1
    void InitWithContext(ApeContext_t *context){
        _context = *context;
    };
#endif
    i32 Parser(u8* in, i32 inByte);

//private:
public:
    ApeContext_t _context;
    /* Seektable */
    u32 seektablePos;

    /* wav header */
    u32 waveHeaderPos;
};


class ApeSeekTable_c
{
public:
    ApeSeekTable_c() {};
    ~ApeSeekTable_c() {};
public:
    INLINE b1 Init() {
        ALGO_MEM_SET(this, 0, sizeof(this));
        return true;
    }
    void SetFirstFramePos(i32 firstFramePos) {
        _firstFramePos = firstFramePos;
        _lastPosTmp = _firstFramePos;
    }
    b1 GetStartPosFromFrame(u32 frame,u32 * pos) {
        u32 posTemp;
        posTemp = _seektable[frame - 1];
        int skip = (posTemp - _seektable[0]) & 3;
        *pos = posTemp - skip;
        return true;
    }
    b1 GetStartSkip(u32 frame,u32 *skip) {
        u32 frameTmp = frame;
        *skip = (_seektable[frameTmp - 1] - _seektable[0]) & 3;
        return true;
    }
    
    b1 GetStartInfoFromPos(i32* pos, i32 *frameOff, i32* skip) {
        if (!pos)
            return false;
        //*pos
        i32 frameOffTmp = 0;
        i32 skipTmp = 0;
        for (; frameOffTmp < _seektableNum; frameOffTmp++) {
            if (*pos < _seektable[frameOffTmp]){
                if (frameOffTmp == 0) {
                    *pos = _lastPos;
                }
                else {
                    *pos = _seektable[frameOffTmp-1];
                }

                skipTmp = (*pos - _firstFramePos) & 3;
                *pos -= skipTmp;
                if(frameOff)
                    *frameOff = frameOffTmp;
                if(skip)
                    *skip = skipTmp;
                return true;
            }
        }
        if (skip)
            *skip = (_seektable[_seektableNum - 1] - _firstFramePos) & 3;
        if (pos)
            *pos = _seektable[_seektableNum - 1] - *skip;
        return false;
    }

    void UpdataSeekTable(i32* seektable, i32 sizeInByte) {
        _seektable = seektable;
        _sizeInByte = sizeInByte;
        _seektableNum = _sizeInByte >> 2;
        _lastPos = _lastPosTmp;
        _lastPosTmp = seektable[_seektableNum - 1];
        if (_lastPos >= seektable[0]) {
            _lastPos = _firstFramePos;
        }
    }
private:
    i32* _seektable = 0;
    i32 _sizeInByte;
    i32 _seektableNum;
    i32 _firstFramePos;
    i32 _lastPos = 0;
    i32 _lastPosTmp = 0;
};
