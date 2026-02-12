#pragma once

#include<stdint.h>
#include<string.h>
#include<stdio.h>
#include"ApeCom.h"

#define STATIC static
#define INLINE inline

#define ALIGEN8(x) (((x+7)>>3)<<3)


#define MAX(a,b) ((a)>(b)?(a):(b))
#define MIN(a,b) ((a)>(b)?(b):(a))

#define ALGO_MEM_CPY(dst,src,size)	memcpy((void*)dst,(void*)src,(i32)size)
#define ALGO_MEM_SET(dst,val,size)	memset((void*)dst,(i32)val,(i32)size)
#define ALGO_MEM_MOVE(dst,src,size) memmove((void*)dst,(void*)src,(i32)size)

#if WIN32
#define ALGO_PRINT(fmt,...)		printf("[%s](%d)" fmt "\n",__func__, __LINE__, ##__VA_ARGS__)
#else
#define ALGO_PRINT(fmt,...)
#endif


typedef uint8_t u8;
typedef uint16_t u16;
typedef uint32_t u32;
typedef uint64_t u64;

typedef int8_t i8;
typedef int16_t i16;
typedef int32_t i32;
typedef int64_t i64;

typedef float f32;
typedef double f64;
typedef bool b1;


class MemoryManger_c
{
public:
	MemoryManger_c() {};
	~MemoryManger_c() {};
public:
	INLINE AlgoBasePorting_c* GetBasePorting() { return _basePorting; }
public:
	INLINE void Init(AlgoBasePorting_c* basePorting) {
		_basePorting = basePorting;
		for (void*& p : _allocList) {
			p = 0;
		}
	}

	INLINE void* Malloc(i32 size) {
		void* ptr = _basePorting->Malloc(size);
		for (void*& p : _allocList) {
			if (!p) {
				p = ptr;
				return p;
			}
		}
		return 0;
	}

	INLINE void Free(void* ptr) {
		for (void*& p : _allocList) {
			if (p == ptr) {
				_basePorting->Free(p);
				p = 0;
				return;
			}
		}
	}

	INLINE void FreeAll() {
		for (void*& p : _allocList) {
			if (p) {
				_basePorting->Free(p);
				p = 0;
			}
		}
	}
private:
	AlgoBasePorting_c* _basePorting;
	void* _allocList[10];
};


class Buffer
{
public:
	Buffer() {};
	Buffer(u8* buf, i32 max) { _buf = buf; _max = max; };
	~Buffer() {};
public:
	INLINE void Init(u8* buf, i32 max) { _buf = buf; _max = max; };
public:
	u8* _buf = 0;
	i32 _max = 0;
};


class Data
{
public:
	Data() {};
	~Data() {};
public:
	//set
	INLINE void SetFlags(u32 flags) {
		_flags |= flags;
	};
	INLINE void ClearFlags(u32 flags) {
		_flags &= ~flags;
	};

	//get
	INLINE u8* GetData() {
		return _buff + _off;
	};

	INLINE i32 GetSize() {
		return _size;
	};

	INLINE u8* GetLeftData() {
		return GetData() + _size;
	};

	INLINE i32 GetLeftSize() {
		return _max - _off - _size;
	};

	INLINE u32 GetFlags() {
		return _flags;
	};

	INLINE u8* GetBuf() {
		return _buff;
	};


	INLINE b1 Init(Buffer* buffer) {
		_buff = buffer->_buf;
		_off = 0;
		_size = 0;
		_max = buffer->_max;
		_flags = 0;
		return true;
	};

	INLINE b1 Append(u8* buf, i32 size) {
		ALGO_MEM_CPY(GetLeftData(), buf, size);
		_size += size;
		return true;
	};

	INLINE b1 Append(i32 size) {
		_size += size;
		return true;
	};

	INLINE b1 AppendFully(u8* buf, i32 size, i32* usedSize) {
		i32 appendSize = GetLeftSize();
		appendSize = appendSize > size ? size : appendSize;
		Append(buf, appendSize);
		if (usedSize)
			*usedSize = appendSize;
		return true;
	};

	INLINE void Used(i32 usedSize)
	{
		_off += usedSize;
		_size -= usedSize;
	}

	INLINE void ClearUsed()
	{
		if (_off)
		{
			ALGO_MEM_MOVE(_buff, GetData(), _size);
			_off = 0;
		}
	}

	INLINE b1 CheckFlag(u32 flag)
	{
		return (b1)(_flags & flag);
	}

protected:
	u8* _buff = 0;
	i32 _off = 0;
	i32 _size = 0;
	i32 _max = 0;
	u32 _flags = 0;
};

#include"ApeCom.h"
#ifndef WIN32
#define __ALIGN8__ __attribute__ ((aligned (8)))
#else
#define __ALIGN8__
#endif


#define APE_RET_SUCCESS					 0
#define APE_RET_NOT_ENOUGH_INPUT		-1
#define APE_RET_ENTROPY_DECODE_ERROR	-2
#define APE_RET_PRASE_ERROR				-3
#define APE_RET_NOT_ENOUGH_OUTPUT       -4
#define APE_RET_INPUT_ERROR		        -5
#define APE_RET_FINISH		            -6
#define APE_RET_INVALID_INPUT_FILE		-7
#define APE_RET_CONTINUE        		-8


#define APE_BLOCKS_MAX   256
#define APE_FILTER_LEVELS 3
#define APE_MAX_CHANNELS 2

#define APE_MIN_VERSION 3950
#define APE_MAX_VERSION 3990

#define APE_NN_FILTER_ROLL_BUFFER_EXTRA_LENGTH (30*4)

#define APE_FRAMECODE_MONO_SILENCE    1
#define APE_FRAMECODE_STEREO_SILENCE  3
#define APE_FRAMECODE_PSEUDO_STEREO   4


#define APE_COMPRESSION_LEVEL_FAST          1000
#define APE_COMPRESSION_LEVEL_NORMAL        2000
#define APE_COMPRESSION_LEVEL_HIGH          3000
#define APE_COMPRESSION_LEVEL_EXTRA_HIGH    4000
#define APE_COMPRESSION_LEVEL_INSANE        5000

#define APE_FORMAT_FLAG_8_BIT               (1 << 0)    // is 8-bit [OBSOLETE]
#define APE_FORMAT_FLAG_CRC                 (1 << 1)    // uses the new CRC32 error detection [OBSOLETE]
#define APE_FORMAT_FLAG_HAS_PEAK_LEVEL      (1 << 2)    // uint32 nPeakLevel after the header [OBSOLETE]
#define APE_FORMAT_FLAG_24_BIT              (1 << 3)    // is 24-bit [OBSOLETE]
#define APE_FORMAT_FLAG_HAS_SEEK_ELEMENTS   (1 << 4)    // has the number of seek elements after the peak level
#define APE_FORMAT_FLAG_CREATE_WAV_HEADER   (1 << 5)    // create the wave header on decompression (not stored)
#define APE_FORMAT_FLAG_AIFF                (1 << 6)    // the file is an AIFF that was compressed (instead of WAV)
#define APE_FORMAT_FLAG_W64                 (1 << 7)    // the file is a W64 (instead of WAV)
#define APE_FORMAT_FLAG_SND                 (1 << 8)    // the file is a SND (instead of WAV)
#define APE_FORMAT_FLAG_BIG_ENDIAN          (1 << 9)    // flags that the file uses big endian encoding
#define APE_FORMAT_FLAG

#define MODEL_ELEMENTS 64
#define ONE_MILLION                  1000000

#define APE_MAX(a,b) ((a) > (b) ? (a) : (b))
#define APE_MIN(a,b) ((a) > (b) ? (b) : (a))
#define APE_SIGN(x)  ((x < 0) - (x > 0))
#define APE_ABS(a) ((a) >= 0 ? (a) : (-(a)))


typedef i16 APE_RET_t;

