#pragma once
#include "IPool.h"
#include "SmfDef.h"
namespace smf{
	class PoolAlias : public IPool {
	public:
		PoolAlias(const char* name, IPool* pool) :_pool(pool) { Name(name); }
	protected:
		IPool* _pool = 0;
	public:///pool
		IPool* Pool() const { return _pool; }
		void Pool(IPool* pool) { _pool = pool; }
	protected:
		virtual bool is_valid()const override { return _pool->IsValid(); }
		virtual void* alloc(unsigned size, unsigned align) override { return _pool->Alloc(size, align); }
		virtual bool free(void* ptr) override { return _pool->Free(ptr); }
		virtual bool hold(void* ptr)override { return _pool->Hold(ptr); }
		virtual void* realloc(void* ptr, unsigned size)override { return _pool->Realloc(ptr, size); }
		virtual void* calloc(unsigned num, unsigned size) override { return _pool->Calloc(num, size); }
		virtual int get_free()const override { return _pool->GetFree(); }
		virtual int get_total()const override { return _pool->GetTotal(); }
		virtual int get_size(void* ptr)const override { return _pool->GetSize(ptr); }
		virtual int get_refs(void* ptr)const override { return _pool->GetRefs(ptr); }
		virtual char* print(char* ptr, char* end) const override { return snprintf(ptr, end, "%s:%s,", Name(), _pool->Name()); }
	};
}
