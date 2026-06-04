#pragma once
#include "smf_common.h"
#include "IPoolList.h"
#include "IPoolShared.h"
namespace smf{
	class IPool {
	public:
		IPool(bool record = true);
		virtual ~IPool();
	public:
		uint64_t ID() const { return _id; }
		void ID(uint64_t id);
		const char* Name() const;
		void Name(const char*);
		void SetBackup(IPool* bkpool);
		void SetBackup(const char* name);
		void SetGloble() const;
	public:
		void* Alloc(unsigned size, unsigned align = 0);
		void* Realloc(void* ptr, unsigned size, unsigned oldsize = 0);
		void* Calloc(unsigned num, unsigned size);
		bool Free(void*& ptr);
		bool Hold(void* ptr);
	public:
		void* AllocUncache(unsigned size);
		bool FreeUncache(void*& ptr);
	public:
		template<class T>inline
		T* AllocT(unsigned num = 1) { return (T*)Alloc(sizeof(T) * num); }
		template<class T>inline
		void Free(T*& ptr) { Free((void*&)ptr); }
	public:
		int GetFree()const { return get_free(); }
		int GetUsed()const { return get_total() - get_free(); }
		int GetTotal()const { return get_total(); }
		int GetUsedMax()const { return _usedMax; }
	public:
		int GetTotalByte()const { return GetTotal(); }
		int GetUsedByte()const { return GetUsed(); }
		int GetFreeByte()const { return GetFree(); }
	public:
		bool IsValid()const { return is_valid(); }
		bool IsInside(void* ptr)const { return (char*)ptr >= _begin && ptr < _end; }
		unsigned GetSize(void* ptr)const { return get_size(ptr); }
		unsigned GetRefs(void* ptr)const { return get_refs(ptr); }
	public:
		void Print() const;
		char* Print(char* ptr, char* end) const;
	public:
		void* CloneMemory(const void*data, int len);
		void* CloneMemory(const smf_pair_t& pair);
		char* CloneString(const char*);
	protected:
		virtual bool is_valid()const { return true; }
		virtual void* alloc(unsigned size, unsigned align)=0;
		virtual bool free(void* ptr)=0;
		virtual bool hold(void* ptr) { return false; }
		virtual void* realloc(void*ptr, unsigned size);
		virtual void* calloc(unsigned num, unsigned size);
		virtual int get_free()const;
		virtual int get_total()const;
		virtual int get_size(void* ptr)const;
		virtual int get_refs(void* ptr)const { return -1; }
		virtual char* print(char* ptr, char* end) const;
	protected:
		uint64_t _id = 0;
		uint32_t _usedMax = 0;
		char* _begin = (char*)0xffffffff;
		char* _end = 0;
		IPool* _bkpool = 0;
	protected:
		void* record(void* ptr, unsigned size, unsigned align);
	public:
		static IPoolList& Pools();
		static IPool& Globle() { return Pools().Globle(); }
		static bool HasGloble() { return Pools().HasShared(); }
		static IPoolShared& Shared(int idx = 0) { return Pools().Shared(idx); }
		static bool HasShared(int idx = 0) { return Pools().HasShared(idx); }
	public:
		static IPool* Find(const char* name) { return Pools().Find(name); }
		static char* PrintS(char* ptr, char* end) { return Pools().Print(ptr, end); }
		static void PrintS() { Pools().Print(); }
		static void* AllocS(const char* pool, int size, int algin = 0) { return Pools().Alloc(pool, size, algin); }
		static void HoldS(const char* pool, void* data) { return Pools().Hold(pool, data); }
		static void FreeS(const char* pool, void*& data) { Pools().Free(pool, data); }
	};
}
