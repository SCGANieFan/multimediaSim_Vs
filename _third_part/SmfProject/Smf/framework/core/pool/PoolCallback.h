#pragma once
#include "IPool.h"

namespace smf{
	template<class CbAlloc, class CbFree
		, class CbHold, class CbRealloc
		, class CbGetSize, class CbGetTotal
	>
		class TPoolCb :public IPool {
		public:
			using FuncAlloc = CbAlloc;
			using FuncFree = CbFree;
			using FuncRealloc = CbRealloc;
			using FuncGetSize = CbGetSize;
			using FuncGetTotal = CbGetTotal;
			using FuncHold = CbHold;
			using PoolCb = TPoolCb<CbAlloc, CbFree, CbHold, CbRealloc, CbGetSize, CbGetTotal>;
		protected:
			CbAlloc _alloc = 0;
			CbFree _free = 0;
			CbRealloc _realloc = 0;
			CbGetSize _getSize = 0;
			CbGetTotal _getTotal = 0;
			CbHold _hold = 0;
		protected:
			virtual void* alloc(unsigned size, unsigned align)override { return _alloc ? _alloc(size) : 0; }
			virtual bool free(void* ptr)override { if (_free)_free(ptr); return _free; }
			virtual bool hold(void* ptr)override { if (_hold)return _hold(ptr); return false; }
			virtual void* realloc(void* ptr, unsigned size)override { return _realloc ? _realloc(ptr, size) : 0; }
			//virtual void* calloc(unsigned num, unsigned size)override { return _calloc ? _calloc(num,size) : 0; }
			//virtual void* alloc_align(unsigned size, int align)override;
			//virtual void print()const override;
			virtual char* print(char* ptr, char* end) const override {
				if (_getSize && _getTotal) {
					auto pool = (PoolCb*)this;
					auto total = GetTotal();
					auto used = total - GetFree();
					if (pool->_usedMax < used) {
						pool->_usedMax = used;
					}
				}
				return IPool::print(ptr, end);
			}
			virtual int get_free()const override { return _getSize ? _getSize() : 0; }
			virtual int get_total()const override { return _getTotal ? _getTotal() : 0; }
			//virtual int get_size(void* ptr)const override;
			//virtual bool is_valid()const override;
			virtual bool is_valid()const override { return _alloc != 0; }
		public:
			TPoolCb() {}
			TPoolCb(CbAlloc cbAlloc
				, CbFree cbFree
				, CbHold cbHold = 0
				, CbRealloc cbRealloc = 0
				, CbGetSize cbGetSize = 0
				, CbGetTotal cbGetTotal = 0

			) {
				Initialize(cbAlloc, cbFree, cbHold, cbRealloc, cbGetSize, cbGetTotal);
			}
		public:
			bool Initialize(CbAlloc cbAlloc
				, CbFree cbFree
				, CbHold cbHold = 0
				, CbRealloc cbRealloc = 0
				, CbGetSize cbGetSize = 0
				, CbGetTotal cbGetTotal = 0
			) {
				_alloc = cbAlloc;
				_free = cbFree;
				_realloc = cbRealloc;
				_getSize = cbGetSize;
				_getTotal = cbGetTotal;
				_hold = cbHold;
				return true;
			}
	};

	using PoolCallback = TPoolCb<
		smf_cb_malloc
		, smf_cb_free
		, smf_cb_hold
		, smf_cb_realloc
		, smf_cb_getsize
		, smf_cb_gettotal
	>;
}
