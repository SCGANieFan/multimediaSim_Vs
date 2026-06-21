#pragma once
#include "IPool.h"
#include "unique_ptr.h"
namespace smf {
	class MemoryPoolBuffers
		:public IPool
	{
	public:
		MemoryPoolBuffers();
		MemoryPoolBuffers(smf_pair_t* pairs, uint32_t count);
		virtual ~MemoryPoolBuffers();
	public:
		bool Initialize(smf_pair_t* pairs, uint32_t count);
		bool Deinitialize();
	protected:
		virtual bool free(void* ptr)override;
		//virtual void* realloc(void* ptr, unsigned size)override;
		//virtual void* calloc(unsigned num, unsigned size)override;
		virtual void* alloc(unsigned size, unsigned align)override;
		//virtual void print()const override;
		virtual char* print(char* ptr, char* end) const override;
		virtual int get_free()const override;
		virtual int get_total()const override;
		virtual bool is_valid()const override;
	protected:
		smf_pair_t* _pairs = 0;
		uint32_t _pairc = 0;
		uint32_t _total = 0;
		uint32_t _free = 0;
	};
}
