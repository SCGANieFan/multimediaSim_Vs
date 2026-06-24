#pragma once
#include "IPool.h"
#include "SmfDef.h"
namespace smf{
	template<class T>
	class TPool : public T {
	protected:
		IPool* _pool = 0;
	public:///pool
		IPool* Pool() const { return _pool ? _pool : &smf::BasePool(); }
		void Pool(IPool* pool) { _pool = pool; }
	public:
		void* Alloc(unsigned size, unsigned align = 0) { return Pool()->Alloc(size, align); }
		void* Calloc(unsigned size, unsigned align = 0) { return Pool()->Calloc(size, align); }
		void* Realloc(unsigned size, unsigned align = 0) { return Pool()->Realloc(size, align); }
		void Free(void*& ptr) { Pool()->Free(ptr); }
	};
}
