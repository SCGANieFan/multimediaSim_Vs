#pragma once

#include"Algo.Macro.h"
#include"Algo.BasePorting.h"

using namespace Algo;

class WavMem_c
{
public:
	WavMem_c() {};
	~WavMem_c() {};
public:
	INLINE void Init(AlgoBasePorting_t* basePorting) {
		_basePorting = basePorting;
		for (void*& p : _allocList) {
			p = 0;
		}
	}

	INLINE void* Malloc(int32_t size) {
		void* ptr = _basePorting->Malloc(size);
		for (void*& p : _allocList) {
			if (!p) {
				p = ptr;
				return p;
			}
		}
		return 0;
	}

	INLINE void Free(void* ptr) {
		for (void*& p : _allocList) {
			if (p == ptr) {
				_basePorting->Free(p);
				p = 0;
				return;
			}
		}
	}

	INLINE void FreeAll() {
		for (void*& p : _allocList) {
			if (p) {
				_basePorting->Free(p);
				p = 0;
			}
		}
	}
private:
	AlgoBasePorting_t* _basePorting;
	void* _allocList[10];
};

