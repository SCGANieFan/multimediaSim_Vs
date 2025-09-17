#if 1
#include "MTF.MemoryBuffNode.h"
#include "MTF.String.h"
#include "MTF.Printer.h"
#include "MTF.Porting.h"
using namespace mtf_ns;
using Node = MTF_MemoryBuffNode_c;

#define dbgNodeX(...) //dbgTestX(##__VA_ARGS__)

void MTF_MemoryBuffNode_c::init(bool used, uint32_t prev, uint32_t next) {
	vs[0] = 0x5a000000 | prev;
	vs[1] = 0x50000000 | next | (used ? 0x01000000 : 0);
}
bool MTF_MemoryBuffNode_c::Check()const {
	//auto v0 = vs[0];
	//auto v1 = vs[1];
	if ((uint32_t)this & 0x7)
		return false;
	//if (v0&0xff000000 != __magic)
	//	return false;
	//if (v1&0xfe000000 != __magic_end)
	//	return false;
	if (!check()) {
		return false;
	}
	auto next = Next();
	if (next) {
		if (next->prev() != this->next())
			return false;
	}
	auto prev = Prev();
	if (prev) {
		if (prev->next() != this->prev())
			return false; 
	}
	return true;
}
char* MTF_MemoryBuffNode_c::Print(char* ptr, char* end) const {
	ptr += SnprintfPorting(ptr, end - ptr, "%08x,%08x", vs[0], vs[1]);
	return ptr;
}
void MTF_MemoryBuffNode_c::Print(uint32_t idx) const {
	dbgNodeX("%d,%p,%08x,%08x,%p,%u,%d\n", idx, this, vs[0], vs[1], Buff(), Size(),IsFree());
}
char* MTF_MemoryBuffNode_c::Dump(char* ptr, char* end) const {
	ptr += SnprintfPorting(ptr, end - ptr, "%08x %08x", vs[0], vs[1]);
	return ptr;
}
Node* MTF_MemoryBuffNode_c::Insert0(int index,bool used) {
	auto node0 = this;
	auto node1 = Next();
	auto size = next() - index;
	auto node = node0 + index;
	node->init(used, index, size);
	next(index);
	node1->prev(size);
	return node;
}
Node* MTF_MemoryBuffNode_c::Insert1(int index, bool used) {
	auto node0 = Prev();
	return node0 ? node0->Insert0(prev() - index, used) : 0;
}

#endif