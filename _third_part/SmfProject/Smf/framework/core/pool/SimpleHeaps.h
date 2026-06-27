#pragma once
#include "SimpleHeap.h"
#include "mutex.h"
namespace smf{
	class SimpleHeaps {
	public:
		SimpleHeaps();
		~SimpleHeaps();
	public:
		bool Add(void* buff, unsigned size, unsigned thr = 0);
		bool Remove(void* buff);
	public:
		void* Alloc(unsigned size);
		bool Free(void* ptr);
		bool Hold(void* ptr);
		void* Realloc(void* ptr, unsigned size);
		void* Calloc(unsigned num, unsigned size);
		void* Alloc(unsigned size, unsigned align);
		//void print()const ;
		char* Print(char* ptr, char* end) const ;
		int GetFree()const ;
		int GetTotal()const ;
		int GetSize(void* ptr)const ;
		bool IsValid()const ;
	protected:
		SimpleHeap* _heaps[8];
		//mutable mutex _mtx;
	};

}
