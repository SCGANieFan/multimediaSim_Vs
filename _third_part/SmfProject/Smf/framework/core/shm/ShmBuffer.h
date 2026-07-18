#pragma once
#include "stdint.h"
#include "stdbool.h"
#include "stdlib.h"
#include "smf_media.h"
#define CacheLineMax 64
namespace smf{
	class ShmBuffer {
	public:
		struct head_t {
			uint64_t _name;
			uint32_t _magic;
			uint32_t _size;
			uint16_t _blksize;
			uint8_t _blknum;
			uint8_t _mediaSize;
			uint32_t _blockMs;
			uint32_t _blockMsDenum;
			uint32_t _firstTime;
			char _media[0];
		};
		union {///head, align CacheLineMax
			char _head[CacheLineMax*2];
			struct{
				uint64_t _name;
				uint32_t _magic;
				uint32_t _size;
				uint16_t _blksize;
				uint8_t _blknum;
				uint8_t _mediaSize;
				uint32_t _blockMs;
				uint32_t _blockMsDenum;
				uint32_t _timePointer;
				char _media[0];
			};
		};
		char* _buff[0];
	public:
		void* Get(uint32_t idx, bool cacheable = false);
		void* Get(uint32_t idx, uint32_t& size, bool cacheable = false);
		void Flush(void* data, bool cacheable = false);
		bool Set(void* media, uint32_t mediaSize, bool cacheable = false);
		bool Set(uint32_t _blockMs, uint32_t _blockMsDenum = 1, bool cacheable = false);
		bool Valid(bool cacheable = false)const;
		const head_t* Head(bool cacheable = false)const;
		uint32_t GetIndex(bool cacheable = false)const;
	public:
		static ShmBuffer* Create(const char* name
			, uint32_t blksize, uint32_t blknum
			, uint32_t blockMs=0, uint32_t blockMsDenum=0
			, void* media=0, uint32_t mediaSize=0
			, bool cacheable = false);
		static void Destroy(ShmBuffer*);
	};
}
