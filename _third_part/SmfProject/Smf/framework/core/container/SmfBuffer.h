#pragma once
#include "unique_ptr.h"

namespace smf {
	class SmfBuffer :public VoidBuffer {
	public:
		using VoidBuffer::VoidBuffer;
	protected:
		int _offset = 0;
	public:
		bool append(const void* data, int size);
		bool append(const char* str);
		bool append(char str);
	public:
		using VoidBuffer::reset;
		void reset(int size = 0, int align = 0);
		int used() const { return _offset; }
		template<class T = void>
		T* current() const { return (T*)((char*)_data + _offset); }
	};
}