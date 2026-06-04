#pragma once
#include "smf_common.h"
#include "IPoolShared.h"

namespace smf{
	class IPool;
	class IPoolList{
	public:
		IPoolList(IPool** pools, int cnt);
	protected:
		IPool** _pools;
		int _cnt = 0;
		IPool* _gbl = 0;
		IPoolShared* _shared[4]{ 0,0 };
	public:
		int Count()const { return _cnt; }
	private:
		IPool** find(uint64_t) const;
		IPool** find(const char*) const;
	public:
		void Add(IPool* pool);
		void Remove(IPool* pool);
		void Remove(const char* name);
		void Remove(uint64_t name);
		void Delete(const char* name);
		IPool* Find(const char*) const;
		IPool* Find(uint64_t) const;
		void* Alloc(const char* pool, int size, int align = 0);
		void* Calloc(const char* pool, int num, int size);
		void* Realloc(const char* pool, void* ptr, int size);
		void Hold(const char* pool, void* data);
		void Free(const char* pool, void*& data);
		void Print()const;
		char* Print(char* ptr, char* end)const;
		bool Set(uint32_t key, void* val);
	public:
		IPool* Register(const char* name, void* buff, unsigned size, unsigned thr = 0, bool remote = false);
		IPool* RegisterX(const char* name, void* buff, unsigned size, unsigned node_count = 0, bool remote = false);
		//IPool* RegisterXs(const char* name, smf_pair_t* pairs, unsigned count, bool remote = false);
		IPool* RegisterBuffers(const char* name, smf_pair_t* pairs, unsigned count, bool remote = false);
		IPool* Register(const char* name
			, smf_cb_malloc
			, smf_cb_free
			, smf_cb_hold = 0
			, smf_cb_realloc = 0
			, smf_cb_getsize = 0
			, smf_cb_gettotal = 0
		);
		IPool* RegisterGloble(void* buff, unsigned size, unsigned thr = 0);
		bool UnregisterGloble(void* buff);
		IPool* RegisterAlias(const char* name, const char* src);
		bool Command(const char* cmd);
	public:
		void Globle(const char* name) { _gbl = Find(name); }
		IPool& Globle() const { return (IPool&)*_gbl; }
		bool HasGloble() const { return _gbl; }
		IPoolShared& Shared(int idx = 0) const { return (IPoolShared&)*_shared[idx]; }
		bool HasShared(int idx = 0) { return _shared[idx]; }
	protected:
		friend class IPool;
		friend class IPoolShared;
	};

	template<int C>
	class TPoolList :public IPoolList {
	private:
		IPool* _items[C];
	public:
		TPoolList() : IPoolList{ _items,C } {}
	};
}
