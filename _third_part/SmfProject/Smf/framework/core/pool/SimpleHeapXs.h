#pragma once
#include <stdint.h>
#include "smf_common.h"

#define __align_bits 6u
#define __align_size (1u<<__align_bits)
#define __align_mask ((1u<<__align_bits)-1u)
#define alignNodeUp(x) (((uint32_t)(x)+__align_mask)&~__align_mask)
#define toNodeSize(x) ((x)>>__align_bits)
#define toNodeSizeUp(x) (((x)+__align_mask)>>__align_bits)
#define toSize(x) ((x)<<__align_bits)

namespace smf {
	class SimpleHeapXs {
	public:
		SimpleHeapXs();
		~SimpleHeapXs();
	protected:
		smf_pair_t* _pairs;
		uint32_t _pairc = 0;
	protected:
		struct Node {
			uint8_t refs;
			uint32_t next : 24;
			uint32_t size;
		};
	protected:
		Node* _heads = 0;
		uint32_t _nodemax = 0;
		uint32_t _nodecount = 0;
		uint32_t _freecount = 0;
		uint32_t _total = 0;
		uint32_t _free = 0;
	public:
		bool Initialize(smf_pair_t* pairs, unsigned count, unsigned nodemax = 0);
		bool Deinitialize();
	public:
		bool IsValid()const { return _heads; }
		operator bool()const { return _heads; }
		char* Print(char* ptr, char* end) const;
		//void Print() const;
		//void PrintError() const;
		uint32_t GetFreeSize() const { return toSize(_free); }
		uint32_t GetTotalSize() const { return toSize(_total); }
		uint32_t GetSize(void* ptr) const;
	public:
		void* Alloc(unsigned size);
		bool Free(void* ptr);
	protected:
		Node* NodeAlloc();
		void NodeFree(Node*);
	protected:
		Node* find(void* ptr, Node** prev = 0)const;
	};
}
