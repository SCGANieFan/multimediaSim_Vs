#pragma once
#include "IPool.h"
namespace smf {
	class MemoryPoolBuffer
		:public IPool
	{
	public:
		MemoryPoolBuffer();
		MemoryPoolBuffer(void* buff,int size);
		virtual ~MemoryPoolBuffer();
	public:
		virtual bool Initialize(void* buff,int size);
		bool Reset();
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
		uint32_t* _begin = 0;
		uint32_t* _end = 0;
		uint32_t* _ptr = 0;

	};

}
