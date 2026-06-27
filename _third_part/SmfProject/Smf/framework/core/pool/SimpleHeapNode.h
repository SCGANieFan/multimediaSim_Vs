#pragma once
#include <stdint.h>

namespace smf {
#define __node_magic0 0x5a000000
#define __node_magic1 0x5a000000
#define __node_used 0x01000000
	class SimpleHeapNode {
	public:
		using Node = SimpleHeapNode;
	public:
		//uint32_t magic : 8;
		//uint32_t prev : 24;	
		//uint32_t magic_end : 7;
		//uint32_t status : 1;
		//uint32_t next : 24;
		uint32_t vs[2];
	public:
		inline bool check() const {
			return ((vs[0] & 0xff000000) == 0x5a000000)
				&& ((vs[1] & 0xf0000000) == 0x50000000);
		}
		void init(bool used, uint32_t prev, uint32_t next);
		inline void prev(uint32_t v) { vs[0] = (vs[0] & 0xff000000) | v; }
		inline void next(uint32_t v) { vs[1] = (vs[1] & 0xff000000) | v; }
		inline void used(bool v) { vs[1] = (vs[1] & ~0x0f000000) | ( v ? 0x01000000 : 0); }
		inline void refs(uint32_t v) { vs[1] = (vs[1] & ~0x0f000000) | (v << 24); }
		inline bool hold() { if (vs[1] >= 0x01000000) vs[1] += 0x01000000; return vs[1] & 0x0f000000;}
		inline bool release() {if (vs[1] >= 0x01000000) vs[1] -= 0x01000000; return vs[1] & 0x0f000000;}
	public:
		inline uint32_t prev() const {return vs[0] & 0x00ffffff;};
		inline uint32_t next() const {return vs[1] & 0x00ffffff;};
		//inline uint32_t size() const {return next() - 1;};
		inline bool used() { return (vs[1] & 0x0f000000); }
		inline uint32_t refs() { return ((vs[1] & 0x0f000000) >> 24); }
	public:
		inline uint32_t Size()const { return next()?((next()-1)<<3):0; }
		inline void* Buff()const { return (void*)(this + 1); }
		inline Node* Next()const { return next()?((Node*)this + next()):0; }
		inline Node* Prev()const { return prev()?((Node*)this - prev()):0; }
	public:
		bool Check()const;
		inline bool IsUsed()const { return vs[1] & 0x0f000000;}
		inline bool IsFree()const { return !IsUsed(); }
	public:
		Node* Insert0(int index,bool used);
		Node* Insert1(int index,bool used);
	public:
		char* Print(char* ptr, char* end) const;
		void Print(uint32_t idx) const;
		char* Dump(char* ptr, char* end) const;
	};
}
