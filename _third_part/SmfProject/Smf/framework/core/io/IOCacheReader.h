#pragma once
#include "Object.h"
#include "IO.h"
namespace smf {
	class IOCacheReader 
		: public IO
	{
	public:
		IOCacheReader();
		virtual ~IOCacheReader();
	public:
		typedef struct {
			IO* io; ///io instance, can read.
			int max;  ///size of buff, align to 2^param3
			int align; ///position align
			bool exclusive; ///excluse io mode
		}OpenParam;
	public:///status ctrl
		virtual bool Open(void*)override;
		virtual bool Close()override;
		virtual bool Seek(int offset, Position pos)override;
		virtual unsigned GetSize() const override;
		virtual unsigned GetOffset() const override;
		virtual unsigned Read(void* buff, unsigned size)override;
		unsigned Write(void* buff, unsigned size)override;
	protected:
		bool Reset(IO* io ///io instance, can read.
			, int max  ///size of buff, align to 2^param3
			, int align ///position align
			, bool exclusive ///excluse io mode
		);
		///read data from io.
		unsigned readIO(void*data,unsigned size);
		///sync _fi to _io, and seek io;
		bool syncIO();
		///read data from cache
		unsigned readCache(void* buff, unsigned size);
		///read data from io to buffer directly.
		unsigned readDirect(void*data,unsigned size);
		///read data from io to buffer directly, read until position alignment.
		unsigned readDirect(void*data);
		///read data from io to fill cache.
		bool syncFill();
	protected:///const
		IO* _io = 0;
		bool _exclusive = false;
		uint8_t* _buff = 0;
		//void* _buffx = 0;
		int _buffmax = 0;
		uint8_t* _end = 0;
		uint32_t _total = 0;
		uint32_t _mask = 512 - 1;///default align 512(FAT32 sector align)
		uint32_t _buffmask = 1024 - 1;
	protected:
		uint32_t _ri = 0;   ///read index
		uint32_t _bi = 0;	///cache index
		uint32_t _bi0 = 0;  ///cache index0
		uint32_t _fi = 0;   ///io index
	public:
		int Skip(int size = 0x7fffffff);
		bool SyncFill();
		uint8_t* Peek(int& size) const;
		int Peek(void*output,int size) const;
	};
}

