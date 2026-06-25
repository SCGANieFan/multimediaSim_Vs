#pragma once
#include "ObjectStatic.h"
namespace smf {
	class RingBufferPool {
	public:
		~RingBufferPool();
	public:
		bool Reset(int max, void* buff = 0);
		void* Alloc(unsigned size);
		bool Free(void* ptr);
		operator bool() { return _buff; }
	protected:
		char* _buff0 = 0;
		char* _buff = 0;
		char* _ptr = 0;
		char* _end = 0;
	};
}
