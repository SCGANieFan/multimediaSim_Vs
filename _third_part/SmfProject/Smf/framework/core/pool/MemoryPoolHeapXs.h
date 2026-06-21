#pragma once
#include "IPool.h"
#include "SimpleHeapXs.h"
#include "mutex.h"
namespace smf{
	class MemoryPoolHeapXs :public IPool {
	public:
		MemoryPoolHeapXs();
		virtual ~MemoryPoolHeapXs();
	public:
		bool Initialize(smf_pair_t* pairs, unsigned count, unsigned nodemax = 0);
	protected:
		virtual void* alloc(unsigned size, unsigned align)override;
		virtual bool free(void* ptr)override;
		//virtual bool hold(void* ptr)override;
		//virtual void* realloc(void* ptr, unsigned size)override;
		//virtual void* calloc(unsigned num, unsigned size)override;
		//virtual void* alloc_align(unsigned size, unsigned align)override;
		//virtual void print()const override;
		virtual char* print(char* ptr, char* end) const override;
		virtual int get_free()const override;
		virtual int get_total()const override;
		virtual int get_size(void* ptr)const override;
		//virtual int get_refs(void* ptr)const override;
		virtual bool is_valid()const override;
	protected:
		SimpleHeapXs _heap;
		mutex _mtx;
	};

}
