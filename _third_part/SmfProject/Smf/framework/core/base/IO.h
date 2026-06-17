#pragma once
#include "Object.h"
namespace smf {
	enum class Position {
		current = SEEK_CUR,
		front = SEEK_SET,
		end = SEEK_END,
	};
	class IO 
		: public Object
	{
	public:
		using IOReadParam_t = smf_io_read_para_t;
	public:
		IO();
	public:///status ctrl
		virtual bool Open(void* para);
		virtual bool Close();
		virtual bool Seek(int offset, Position pos);
		virtual unsigned GetSize() const;
		virtual unsigned GetOffset() const;
		virtual unsigned Read(void* buff, unsigned size);
		virtual unsigned Write(void* buff, unsigned size);
		virtual bool Reads(IOReadParam_t* paras, int count);
	public:
		bool IsOpen() const;
	public:
		enum ExFlags{
			IS_Open = 1u << 16,
		};
	public:
		bool URLParse(char* url, const char*& type, const char*& path, const char*& ext);
	};
}

