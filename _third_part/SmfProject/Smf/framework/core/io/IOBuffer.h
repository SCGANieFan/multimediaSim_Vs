#pragma once
#include "IO.h"

namespace smf {
	class IOBuffer :public IO {
	public:
		IOBuffer();
		IOBuffer(void* _data, int _size);
		virtual ~IOBuffer();
	protected:
		char* _data = 0;
		int _size = 0;
		int _offset = 0;	
	public:
		operator bool() const { return _data; }
		void Reset(void* data=0, int size=0);
	public:///status ctrl
		virtual bool Open(void*)override;
		virtual bool Close()override;
	public:///status ctrl
		virtual bool Seek(int offset, Position pos)override;
		virtual unsigned GetSize() const override;
		virtual unsigned GetOffset() const override;
		virtual unsigned Read(void* buff, unsigned size)override;
		virtual unsigned Write(void* buff, unsigned size)override;
	};
}
