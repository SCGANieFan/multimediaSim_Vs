#pragma once
#include <stdint.h>
#include "ObjectStatic.h"
#include "smf_debug.h"

#define __align_bits 6u
#define __align_size (1u<<__align_bits)
#define __align_mask ((1u<<__align_bits)-1u)
#define alignNodeUp(x) (((uint32_t)(x)+__align_mask)&~__align_mask)
#define toNodeSize(x) ((x)>>__align_bits)
#define toNodeSizeUp(x) (((x)+__align_mask)>>__align_bits)
#define toSize(x) ((x)<<__align_bits)

namespace smf {
	class SimpleHeapX {
	public:
		SimpleHeapX();
		SimpleHeapX(void* buff, int size, int nodemax = 0);
		~SimpleHeapX();
	protected:
		struct Node {
			uint8_t refs;
			uint32_t next : 24;
			uint32_t size;
		};
	protected:
		char* _buff = 0;
		uint32_t _total = 0;
		uint32_t _free = 0;
		Node* _head = 0;
	protected:
		uint32_t _nodemax = 0;
		uint32_t _nodecount = 0;
		uint32_t _freecount = 0;
	public:
		bool Initialize(void* buff, int size, int nodemax = 0);
		bool Deinitialize();
	public:
		bool IsBuffer(void* ptr)const { return (char*)ptr == _buff; }
		bool IsContain(void* ptr)const { return (char*)ptr >= _buff && (char*)ptr < _buff + GetTotalSize(); }
		bool IsValid()const { return _head; }
		operator bool()const { return _head; }
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
