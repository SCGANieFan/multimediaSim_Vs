#pragma once
#include<new>
#include"Algo.BasePorting.h"
#include"Algo.Macro.h"
#include"Algo.Type.h"

#define CHECK_STEP_ON_MEMORY 1

#if  CHECK_STEP_ON_MEMORY
#define CHECK_STEP_ON_MEMORY_MAGIC (0x0F0F0F0F0F0F0F0F)
#endif

namespace Algo{

class MemoryManger_c
{
public:
	MemoryManger_c() {};
	~MemoryManger_c() {};
public:
	INLINE void Init(AlgoBasePorting_c*basePorting) {
		_basePorting = basePorting;
		for (void*& p : _allocList) {
			p = 0;
		}
	}

	INLINE void* Malloc(i32 size) {
#if  CHECK_STEP_ON_MEMORY
		size += 8;
#endif
		void* ptr = _basePorting->Malloc(size);
		if (!ptr)
			return 0;
#if  CHECK_STEP_ON_MEMORY
		*((u64*)ptr) = CHECK_STEP_ON_MEMORY_MAGIC;
		ptr = (u8*)ptr + 8;
#endif
		for (void*& p : _allocList){
			if (!p){
				p = ptr;
				return p;
			}
		}
		return 0;
	}

	template<class T>
	INLINE void* New(i32 size){
		void *ptr = Malloc(size);
		if(!ptr)
			return 0;
		return new(ptr) T();
	}

	INLINE void Free(void *ptr) {
		for (void*& p : _allocList) {
			if (p== ptr) {
#if  CHECK_STEP_ON_MEMORY
				p = (u8*)p - 8;
				u64 magic = *(u64*)p;
				ALGO_ASSERT(magic == CHECK_STEP_ON_MEMORY_MAGIC);
#endif
				_basePorting->Free(p);
				p = 0;
				return;
			}
		}
	}

	template<class T>
	INLINE void* Delete(void *ptr){
		(T*)ptr->~T();
		Free(ptr);
	}

	INLINE void FreeAll () {
		for (void*& p : _allocList) {
			if (p) {
#if  CHECK_STEP_ON_MEMORY
				p = (u8*)p - 8;
				u64 magic = *(u64*)p;
				ALGO_ASSERT(magic == CHECK_STEP_ON_MEMORY_MAGIC);
#endif
				_basePorting->Free(p);
				p = 0;
			}
		}
	}
private:
	AlgoBasePorting_c* _basePorting;
	void* _allocList[100];
};



}







