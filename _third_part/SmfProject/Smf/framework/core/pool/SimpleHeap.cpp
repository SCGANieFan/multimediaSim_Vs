#include "SimpleHeap.h"
#include "SimpleHeapNode.h"
#include "string.h"

using namespace smf;
#define __align_bits 3u
#define __align_size (1u<<__align_bits)
#define __align_mask ((1u<<__align_bits)-1u)
#define __node_min 2u

#define align_max(v) (((uint32_t)(v) + __align_mask)&~__align_mask)
#define align_min(v) (((uint32_t)(v))&~__align_mask)
#define size2index_max(v) (((uint32_t)(v) + __align_mask) >> __align_bits)
#define size2index_min(v) (((uint32_t)(v)) >> __align_bits)
#define index2size(v) (((uint32_t)(v)) << __align_bits)
#define check_align(v) (!((uint32_t)(v)&__align_mask))

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

#define min(x,y) ((x)<(y)?(x):(y))
#define locker() //unique_lock<mutex> lock(_mtx)

SimpleHeap::SimpleHeap()
{//dbgTestPL();
}
SimpleHeap::SimpleHeap(void* buff, int size, int thr)
{//dbgTestPL();
	Initialize(buff, size, thr);
}
SimpleHeap:: ~SimpleHeap() {

}
void SimpleHeap::SetThreshold(uint32_t thr) {
	_threshold = thr;
}
SimpleHeap::operator bool()const {
	return _buff;
}
bool SimpleHeap::IsValid(void*ptr) const {
	return ptr > _node0 && ptr < _node1;
}
bool SimpleHeap::IsValid()const {
	return _buff;
}
uint32_t SimpleHeap::GetFreeSize() const {
	return (_free - _freeCount) << __align_bits;
}
uint32_t SimpleHeap::GetUsedSize() const {
	return GetTotalSize() - GetFreeSize();
}
uint32_t SimpleHeap::GetTotalSize() const {
	return (_total - _totalCount) << __align_bits;
}
uint32_t SimpleHeap::GetFreeNodeCount() const {
	return _freeCount;
}
uint32_t SimpleHeap::GetUsedNodeCount() const {
	return _totalCount - _freeCount;
}
uint32_t SimpleHeap::GetTotalNodeCount() const {
	return _totalCount;
}
uint32_t SimpleHeap::GetSize(void* ptr) const {
	auto node = find0(ptr);
	if (node)return node->Size();
	return 0;
}
uint32_t SimpleHeap::GetRefs(void* ptr) const {
	auto node = find0(ptr);
	if (node)return node->refs();
	return 0;
}
void* SimpleHeap::GetBegin()const { return _node0; }
void* SimpleHeap::GetEnd()const { return _node1; }
bool SimpleHeap::Initialize(void* buff0, int size0, int thr) {
	//unique_lock<mutex> lock(_mtx);
	//dbgTestPXL("%u(%u,%u,%u)", sizeof(Node), __align_bits, __align_size, __align_mask);
	//dbgTestPXL("%p,%u,%u",buff0,size0,thr);
	returnIfErrC(false, !buff0);
	returnIfErrC(false, !size0);
	_buff = (uint8_t*)buff0;
	_buffSize = size0;
	_threshold = thr;
	auto buff = align_max(buff0);
	auto index = size2index_min(size0) - 2;
	_node0 = (Node*)buff;
	_node0->init(false, 0u, index);
	_node1 = _node0 + index;
	_node1->init(true, index, 0u);
	//
	_totalCount = 1;
	_freeCount = 1;
	_total = index;
	_free = index;
	//
	//dbgTestDump("node0", _node0, sizeof(Node));
	//dbgTestDump(_node0, sizeof(Node));
	//dbgTestDump("node1", _node1, sizeof(Node));
	//dbgTestDump(_node1, sizeof(Node));
	return true;
}
void SimpleHeap::Uninit() {

}
bool SimpleHeap::Reset() {
	return Initialize(_buff,_buffSize, _threshold);
}
bool SimpleHeap::Hold(void* ptr) {
	locker();
	auto node = getNode(ptr);
	returnIfErrC(false, !node);
	return node->hold();
}
void* SimpleHeap::Alloc(unsigned size, unsigned align) {
	locker();
	auto node = alloc(size, align);
	return node ? node->Buff() : 0;
}
void* SimpleHeap::Realloc(void* ptr, unsigned size) {
	locker();
	auto node = realloc(ptr, size);
	return node ? node->Buff() : 0;
}
void* SimpleHeap::Calloc(unsigned blknum, unsigned blksize) {
	locker();
	auto size = blksize * blknum;
	auto node = alloc(size);
	if (!node) return 0;
	auto ptr = node->Buff();
	memset(ptr, 0, size);
	return ptr;
}
bool SimpleHeap::Free(void* ptr) {
	locker();
	return free(getNode(ptr));
}

SimpleHeap::Node* SimpleHeap::alloc(unsigned size0, unsigned align) {
	if (align <= __align_size)
		return alloc(size0);
	auto mask = align - 1;
	returnIfErrC(0, (align & mask));
	auto size = ((size0 + mask) & ~mask) + align;
	auto node = alloc(size);
	returnIfErrC(0,!node);
	//align
	auto addr = ((uintptr_t)(node->Buff()) + mask)&(~mask);
	auto nodex = ((Node*)addr) - 1;
	if (node != nodex) {
		if (!move(nodex, node)) {
			free(node);
			node = 0;
		}
		else {
			if (node->prev()) {
				if (!node->Prev()->used()) {
					_free += nodex - node;
				}
			}
			else {
				free(node);
			}
			node = nodex;
		}
	}
	return node;
}
SimpleHeap::Node* SimpleHeap::alloc(unsigned size) {
	return (size < _threshold) ? alloc0(size) : alloc1(size);
}
SimpleHeap::Node* SimpleHeap::alloc0(unsigned size) {//dbgTestPXL("%d,%p,%p,%p,%p",size, _node0, _node1, _node0->vs[1], _node1->vs[0]);
	auto index = size2index_max(size) + 1;
	auto node = _node0;
	while (node && (node < _node1)) {
		if (!node->check()) {
			returnIfErrC(NULL, !repair());
			returnIfErrC(NULL, !node->check());
		}
		if (node->IsFree() && index < node->next()) {
			return alloc0(node, index);
		}
		node = node->Next();
	}
	//
	dbgErrPXL("%d", size);
	//Print();
	return NULL;
}
SimpleHeap::Node* SimpleHeap::alloc1(unsigned size) {//dbgTestPXL("%d,%p,%p,%p,%p",size, _node0, _node1,_node0->vs[1], _node1->vs[0]);
	auto index = size2index_max(size) + 1;
	auto node = _node1->Prev();
	while (node && (node >= _node0)) {//dbgTestPXL("%p,%p,%p", node, node->vs[0], node->vs[1]);
		if (!node->check()) {
			returnIfErrC(NULL, !repair());
			returnIfErrC(NULL, !node->check());
		}
		if (node->IsFree() && index <= node->next()) {
			return alloc1(node, index);
		}
		node = node->Prev(); //dbgTestPPL(node);
	}
	//
	dbgErrPXL("%p,%d", node, size);
	return NULL;
}
SimpleHeap::Node* SimpleHeap::alloc0(Node* node, unsigned index) {
	if (index + __node_min < node->next()) {
		insert(node, index);
	}
	node->used(true);
	_free -= node->next();
	_freeCount--; //dbgTestPXL("%d,%p,%p,%p", node->Size(), node, node->vs[0], node->vs[1]);
	return node;
}
SimpleHeap::Node* SimpleHeap::alloc1(Node* node, unsigned index) {
	if (index + __node_min < node->next()) {
		node = insert(node, node->next() - index);
	}
	node->used(true);
	_free -= node->next();
	_freeCount--; //dbgTestPXL("%d,%p,%p,%p", node->Size(), node, node->vs[0], node->vs[1]);
	return node;
}
SimpleHeap::Node* SimpleHeap::realloc(void* ptr, unsigned size0) {
	returnIfErrC(0, !size0);
	if (!ptr) {
		return alloc(size0);
	}
	//
	auto size = align_max(size0);
	auto node = getNode(ptr);
	returnIfErrC(0, !node);
	auto request = size2index_max(size) + 1;
	auto next = node->Next();
	if (request == node->next()) {
		return node;
	}
	else if (request < node->next()) {
		if (request + __node_min < node->next()) {
			if (next && !next->used()) {
				move(next, node + request);
			}
			else {
				insert(node, request);
			}
			_free += next - (node + request);
		}
		return node;
	}
	else { //if (request > node->next)
		if (!next || next->IsUsed()) {//re alloc a buff
			return realloc(node, size);
		}
		if (request == node->next() + next->next()) {
			returnIfErrC(0, !merge0(node));
			_free -= next->next();
			return node;
		}
		else if (request < node->next() + next->next()) {
			auto curr = node + request;
			if (!move(curr, next)) {
				return realloc(node, size);
			}
			else {
				_free -= curr - next;
			}
			return node;
		}
		else {//if(request > node->next + next->next)
			return realloc(node,size);
		}
	}
	return 0;
}
SimpleHeap::Node* SimpleHeap::realloc(Node* node, unsigned size) {
	auto dst = alloc(size);
	if (!dst) return 0;
	auto len = min(size, node->Size());
	memcpy(dst->Buff(), node->Buff(), len);
	free(node);
	return dst;
}
bool SimpleHeap::free(Node* node) {
	returnIf(false, !node);
	if (!node->Check()){
		returnIfErrC(false, !repair());
	}
	if (node->IsFree()) {
		return true;
	}
	//free
	// node->used(false);
	node->release();
	if(node->refs()) return true;
	_freeCount++;
	_free += node->next(); //dbgTestPXL("%d,%p,%p,%p", node->Size(), node, node->vs[0], node->vs[1]);
	//
	merge0(node);
	merge1(node);
	return true;
}
SimpleHeap::Node* SimpleHeap::insert(Node* node, unsigned index) {
	returnIf(0, !node);
	returnIf(0, !node->next());
	returnIf(0, index > node->next());
	//new node
	auto curr = node + index;
	curr->init(false, index, node->next() - index);
	//next node
	auto next = node + node->next();
	next->prev(curr->next());
	//current node
	node->next(curr->prev());
	_totalCount++;
	_freeCount++;
	return curr;
}
bool SimpleHeap::merge0(Node* curr) {
	if (!curr)return true;
	auto node = curr->Next();
	if (!node)return true;
	if (node->used() != curr->used())return true;
	//
	auto next = node->Next();
	if (!next)return true;
	auto idx = next - curr;
	curr->next(idx);
	next->prev(idx);
	//
	_totalCount--;
	if (curr->IsFree()) {
		_freeCount--;
	}
	//
	return true;
}
bool SimpleHeap::merge1(Node * curr) {
	if (!curr)return true;
	return merge0(curr->Prev());
}

SimpleHeap::Node* SimpleHeap::find0(void* ptr) const {
	returnIfErrC(0, ptr <= _node0);
	returnIfErrC(0, ptr >= _node1);
	auto node = _node0;
	while (node && node->next() && (node < _node1)) {
		auto next = node->Next();
		if (ptr < (void*)(next)) {
			return node;
		}
		node = next;
	}
	return NULL;
}
SimpleHeap::Node* SimpleHeap::find1(void* ptr) const {
	returnIfErrC(0, ptr <= _node0);
	returnIfErrC(0, ptr >= _node1);
	auto node = _node1;
	while (node && node->prev() && (node > _node0)) {
		auto prev = node->Prev();
		if (ptr >= (void*)(prev + 1)) {
			return prev;
		}
		node = prev;
	}
	return NULL;
}
SimpleHeap::Node* SimpleHeap::findBadNode0(uint32_t& cnt)const {
	auto node = _node0;
	auto prev = node;
	while (node && (node < _node1)) {
		if (!node->check()) {
			return prev;
		}
		cnt += node->IsFree();
		prev = node;
		node = node->Next();
	}
	return 0;
}
SimpleHeap::Node* SimpleHeap::findBadNode1(uint32_t&cnt)const {
	auto node = _node1;
	auto next = node;
	while (node && (node > _node0)) {
		if (!node->check()) {
			return next;
		}
		cnt += node->IsFree();
		next = node;
		node = node->Prev();
	}
	return 0;
}
SimpleHeap::Node* SimpleHeap::repair()const {
	dbgWarnPL();
	PrintError();
	//
	uint32_t cnt = 0ul;
	Node* prev = findBadNode0(cnt);
	returnIfErrC(0, !prev);
	Node* curr0 = prev->Next();
	//
	Node* next = findBadNode1(cnt);
	returnIfErrC(0, !next);
	Node* curr1 = next->Prev();
	//
	returnIfErrCS(0, curr0!=curr1,"%p<>%p",curr0,curr1);
	auto curr = curr0;
	auto status = 0;
	if (cnt == _freeCount) {
		status = 1;
	}
	else if (cnt + 1 == _freeCount) {
		status = 0;
	}
	else {
		dbgErrPXL("%u/%u/%u(%p,%p,%p)(%p,%p,%p)(%p,%p,%p)"
			, cnt, _freeCount, _totalCount
			, prev, prev->vs[0], prev->vs[1]
			, curr, curr->vs[0], curr->vs[1]
			, next, next->vs[0], next->vs[1]
		);
		return 0;
	}
	dbgWarnDump(curr, 16);
	curr->init(status, prev->next(), next->prev());
	dbgWarnPXL("%u/%u/%u(%p,%p,%p)(%p,%p,%p)(%p,%p,%p)"
		, cnt, _freeCount, _totalCount
		, prev, prev->vs[0], prev->vs[1]
		, curr, curr->vs[0], curr->vs[1]
		, next, next->vs[0], next->vs[1]
	);
	return curr;
}
bool SimpleHeap::move(Node* dst, Node* src)const {
	if (src == dst)return true;
	auto prev = (Node*)src->Prev();
	auto next = (Node*)src->Next();
	if(!prev) prev = src;
	if(!next) next = src;
	returnIfErrC(false, dst <= prev);
	returnIfErrC(false, dst >= next);
	dst->init(src->used(), dst - prev, next - dst);
	prev->next(dst->prev());
	next->prev(dst->next());
	//
	if (prev == src || next == src) {
		_totalCount++;
		if (!src->used()) {
			_freeCount++;
		}
	}
	return true;
}
SimpleHeap::Node* SimpleHeap::getNode(void* ptr)const {
	returnIfErrC(0, !ptr);
	returnIfErrC(0, ptr < (void*)(_node0 + 1));
	returnIfErrC(0, ptr >= (void*)(_node1));
	Node* node = (Node*)ptr - 1;
	if (node->Check()) {
		return node;
	}
	//
	node = find0(ptr);
	if (!node) {
		node = find1(ptr);
	}
	returnIfErrC(0, !node);
	//
	//dbgWarnPXL("%p,%p/%u",ptr,node->Buff(),node->Size());
	if (node->Check()) {
		return node;
	}
	//
	returnIfErrC(0, !repair());
	return node;
}

char* SimpleHeap::Print(char* ptr, char* end) const {
	ptr += snprintf(ptr, end - ptr, "(%u)%u/%u,%u/%u"
		, _threshold
		,_freeCount, _totalCount
		,_free, _total
	);
	//auto node = _node0;
	//for (int i = 0; i < _totalCount + 1; i++) {
	//	ptr += snprintf(ptr,end-ptr,"[%d]",i);
	//	ptr = node->Print(ptr, end); *ptr++ = '\n';
	//	node = node->Next();
	//}
	//node = _node1;
	//for (int i = _totalCount; i >=0; i--) {
	//	ptr += snprintf(ptr, end - ptr, "[%d]", i);
	//	ptr = node->Print(ptr, end); *ptr++ = '\n';
	//	node = node->Prev();
	//}
	//*ptr++ = 0;
	return ptr;
}
void SimpleHeap::Print() const {
	dbgTestPXL("%u/%u,%u/%u,%u/%u"
		, _freeCount, _totalCount
		, _free, _total
		, GetFreeSize(), GetTotalSize()
	);
	auto node = _node0;
	int i = 0;
	while (node && node <= _node1 && node->check()) {
		node->Print(i++);
		node = node->Next();
	}
	if(node)node->Print(i);
	//
	node = _node1;
	i = _totalCount;
	while (node && node >= _node0 && node->check()) {
		node->Print(i--);
		node = node->Prev();
	}
	if(node)node->Print(i);
}
void SimpleHeap::PrintError() const {
	dbgTestPXL("%u/%u,%u/%u,%u/%u"
		, _freeCount, _totalCount
		, _free, _total
		, GetFreeSize(), GetTotalSize()
	);
	auto node = _node0;
	auto prev = node;
	int i = 0;
	while (node && node <= _node1 && node->check()) {
		prev = node;
		i++;
		node = node->Next();
	}
	if (prev)prev->Print(i - 1);
	if (node)node->Print(i);
	//
	node = _node1;
	auto next = node;
	i = _totalCount;
	while (node && node >= _node0 && node->check()) {
		//node->Print(i--);
		next = node;
		i--;
		node = node->Prev();
	}
	if (node)node->Print(i);
	if (next)next->Print(i + 1);
}

SimpleHeap* SimpleHeap::Create(void* buff0, int size0, int thr) {
	returnIfErrC(0, !buff0);
	returnIfErrC(0, size0 < sizeof(SimpleHeap) + 16);
	auto heap = (SimpleHeap*)(((uintptr_t)buff0 + 3u) & ~3u);
	auto buff = heap + 1;
	auto size = size0 - ((char*)heap - (char*)buff0) - sizeof(SimpleHeap);
	returnIfErrC(0, !heap->Initialize(buff, size, thr));
	return heap;
}
void SimpleHeap::Destroy(SimpleHeap*heap) {
	if(heap)
		heap->Uninit();
}
