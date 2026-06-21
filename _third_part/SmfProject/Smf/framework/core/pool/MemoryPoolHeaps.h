#pragma once
#include "IPool.h"
#include "SimpleHeap.h"
#include "mutex.h"
namespace smf{
	class MemoryPoolHeaps:public IPool {
	public:
		MemoryPoolHeaps();
		virtual ~MemoryPoolHeaps();
	public:
		bool Add(void* buff, unsigned size, unsigned thr = 0);
		bool Remove(void* buff);
	protected:
		virtual void* alloc(unsigned size, unsigned align)override;
		virtual bool free(void* ptr)override;
		virtual bool hold(void* ptr)override;
		virtual void* realloc(void* ptr, unsigned size)override;
		//virtual void* calloc(unsigned num, unsigned size)override;
		//virtual void* alloc_align(unsigned size, unsigned align)override;
		//virtual void print()const override;
		virtual char* print(char* ptr, char* end) const override;
		virtual int get_free()const override;
		virtual int get_total()const override;
		virtual int get_size(void* ptr)const override;
		virtual int get_refs(void* ptr)const override;
		virtual bool is_valid()const override;
	public:
		enum {
			eFlags = 0xffff55aa,
		};
	protected:
		uint32_t _flags = eFlags;
		SimpleHeap* _heaps[8];
		mutable mutex _mtx;
		friend class IPoolList;
	};

}
