#include "SimpleHeapNode.h"

using namespace smf;
using Node = SimpleHeapNode;

#if 1
#include "smf_debug.h"
#else
#define dbgTestPXL(fmt,...) printf("[test]%d,%s()" fmt "\n",__LINE__,__FUNCTION__,##__VA_ARGS__)
#define dbgInfoPXL(fmt,...) printf("[info]%d,%s()" fmt "\n",__LINE__,__FUNCTION__,##__VA_ARGS__)
#define dbgErrPXL(fmt,...) printf("[err]%d,%s()" fmt "\n",__LINE__,__FUNCTION__,##__VA_ARGS__)
#define dbgTestDump(x,p,s) { printf("[test]" x "%p(%u):",p,s); char* ptr = (char*)(p);char* end =ptr+(s); while(ptr<end){printf("%02x ",*ptr++);} printf("\n"); }
#define returnIf(v,c) if(c)return v
#define returnIfErrC(v,c) if(c){dbgErrPXL(#c);return v;}
#define returnIfErrCS(v,c,s,...) if(c){dbgErrPXL(#c s,##__VA_ARGS__);return v;}
#endif

#define dbgNodeX(fmt,...) dbgMemX(fmt,##__VA_ARGS__)

void SimpleHeapNode::init(bool used, uint32_t prev, uint32_t next) {
	vs[0] = 0x5a000000 | prev;
	vs[1] = 0x50000000 | next | (used ? 0x01000000 : 0);
}
bool SimpleHeapNode::Check()const {
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
char* SimpleHeapNode::Print(char* ptr, char* end) const {
	ptr += snprintf(ptr, end - ptr, "%08x,%08x", vs[0], vs[1]);
	return ptr;
}
void SimpleHeapNode::Print(uint32_t idx) const {
	dbgNodeX("%d,%p,%08x,%08x,%p,%u,%d\n", idx, this, vs[0], vs[1], Buff(), Size(),IsFree());
}
char* SimpleHeapNode::Dump(char* ptr, char* end) const {
	ptr += snprintf(ptr, end - ptr, "%08x %08x", vs[0], vs[1]);
	return ptr;
}
Node* SimpleHeapNode::Insert0(int index,bool used) {
	auto node0 = this;
	auto node1 = Next();
	auto size = next() - index;
	auto node = node0 + index;
	node->init(used, index, size);
	next(index);
	node1->prev(size);
	return node;
}
Node* SimpleHeapNode::Insert1(int index, bool used) {
	auto node0 = Prev();
	return node0 ? node0->Insert0(prev() - index, used) : 0;
}
