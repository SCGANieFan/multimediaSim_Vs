#pragma once
#include "IPool.h"
#include "SimpleHeapX.h"
#include "mutex.h"
namespace smf{
	class MemoryPoolHeapX:public IPool {
	//public:
		//MemoryPoolHeapX();
		//virtual ~MemoryPoolHeapX();
	public:
		bool Initialize(void* buff, unsigned size, unsigned nodemax = 0) { return _heap.Initialize(buff, size, nodemax); }
	protected:
		virtual void* alloc(unsigned size, unsigned align)override { unique_lock<mutex> lck(_mtx); return _heap.Alloc(size); }
		virtual bool free(void* ptr)override { unique_lock<mutex> lck(_mtx); return _heap.Free(ptr); }
		//virtual bool hold(void* ptr)override;
		//virtual void* realloc(void* ptr, unsigned size)override;
		//virtual void* calloc(unsigned num, unsigned size)override;
		//virtual void* alloc_align(unsigned size, unsigned align)override;
		//virtual void print()const override;
		virtual char* print(char* ptr, char* end) const override { return _heap.Print(IPool::print(ptr, end), end); }
		virtual int get_free()const override { return _heap.GetFreeSize(); }
		virtual int get_total()const override { return _heap.GetTotalSize(); }
		virtual int get_size(void* ptr)const override { return _heap.GetSize(ptr); }
		//virtual int get_refs(void* ptr)const override;
		virtual bool is_valid()const override { return _heap; }
	protected:
		SimpleHeapX _heap;
		mutex _mtx;
	};

}
