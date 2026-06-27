#include "SimpleHeapX.h"
#include "SmfDef.h"
#include "smf_debug.h"

using namespace smf;
SimpleHeapX::SimpleHeapX() {}
SimpleHeapX::SimpleHeapX(void* buff, int size, int nodemax) { Initialize(buff, size, nodemax); }
SimpleHeapX::~SimpleHeapX() {
	Deinitialize();
}
bool SimpleHeapX::Deinitialize() {
	if (_head) {
		smf::Free(_head);
		_head = 0;
	}
	return true;
}
bool SimpleHeapX::Initialize(void* buff, int size, int nodemax) {
	//dbgMemPXL("%p:%d", buff, size);
	returnIfErrC(false, !buff);
	returnIfErrC(false, !size);
	returnIfErrC(false, _head);
	_buff = (char*)alignNodeUp(buff);
	_total = toNodeSize((char*)buff + size - _buff);
	_free = _total;
	//
	if (!nodemax)nodemax = 4;
	_nodemax = nodemax;
	_head = (Node*)smf::Alloc(sizeof(Node) * _nodemax);
	returnIfErrC(false, !_head);
	memset(_head, 0, sizeof(Node) * _nodemax);
	//
	_head->refs = 0;
	_head->size = _total;
	_head->next = 0;
	_nodecount = 1;
	_freecount = 1;
	return true;
}

SimpleHeapX::Node* SimpleHeapX::NodeAlloc() {
	returnIfErrC(0, !_head);
	for (int i = 0; i < _nodemax; i++) {
		auto node = _head + i;
		if (!node->size) {
			_nodecount++;
			node->refs = 0;
			node->next = 0;
			return node;
		}
	}
	auto idx = _nodemax;
	_nodemax <<= 1;
	_head = (Node*)smf::Realloc(_head, sizeof(Node) * _nodemax);
	returnIfErrC(0, !_head);
	auto node = &_head[idx];
	memset(node, 0, sizeof(Node) * idx);
	_nodecount++;
	return node;
}

void SimpleHeapX::NodeFree(Node*node) {
	//smf::Free(node);
	node->size = 0;
	_nodecount--;
}

void* SimpleHeapX::Alloc(unsigned size_) {
	returnIfErrC(0, !_head);
	auto size = toNodeSizeUp(size_);
	auto node = _head;
	auto idx = 0; 
	while (1) {
		if (!node->refs && (size <= node->size)) {
			node->refs = 1;
			_freecount--;
			_free -= size;
			if (node->size > size) {
				auto left = NodeAlloc();
				_freecount++;
				left->refs = 0;
				left->size = node->size - size;
				left->next = node->next;
				node->next = left - _head;
				node->size = size;
			}
			return _buff + toSize(idx);
		}
		if (!node->next) {
			break;
		}
		idx += node->size;
		node = _head + node->next;
	}
	dbgWarnPXL("%u,%d/%d", toSize(size), toSize(_free), toSize(_total));
	return 0;
}
bool SimpleHeapX::Free(void* ptr) {
	Node* prev = 0;
	auto node = find(ptr, &prev);
	returnIfErrC(false, !node);

	if (!node->refs) {
		return true;
	}
	node->refs--;
	if (node->refs) {
		return true;
	}

	_free += node->size;
	_freecount++;
	
	if (prev && !prev->refs) {
		prev->size += node->size;
		prev->next = node->next;
		NodeFree(node);
		node = prev;
	}
	if (node->next) {
		auto next = _head + node->next;
		if (!next->refs) {
			node->size += next->size;
			node->next = next->next;
			NodeFree(next);
		}
	}
	return true;
}

uint32_t SimpleHeapX::GetSize(void* ptr) const {
	auto node = find(ptr);
	return node ? toSize(node->size) : 0;
}

SimpleHeapX::Node* SimpleHeapX::find(void* ptr, Node** prev_)const {
	returnIfErrC(0, !_head);
	auto offset = toNodeSize((char*)ptr - _buff);
	returnIfWarnC(0, offset > _total);
	returnIfWarnC(0, offset < 0);
	Node* prev = 0;
	auto node = _head;
	auto idx = 0;
	while (1) {
		if (offset >= idx && offset < idx + node->size) {
			if (prev_)*prev_ = prev;
			return node;
		}
		if (!node->next) {
			break;
		}
		idx += node->size;
		prev = node;
		node = _head + node->next;
	}
	return 0;
}
char* SimpleHeapX::Print(char* ptr, char* end) const {
	ptr = snprintf(ptr, end, "(%p,%u)(%u/%u/%u)"
		, _buff, toSize(_total)
		, _freecount, _nodecount, _nodemax
	);
#if 1
	auto node = _head;
	int i = 0;
	int idx = 0;
	while (1) {
		ptr += snprintf(ptr, end - ptr, ",%d(%d)%p/%u"
			, i++, node->refs, _buff + toSize(idx), toSize(node->size)
		);
		if (!node->next) {
			break;
		}
		idx += node->size;
		node = _head + node->next;
	}
#endif
	return ptr;
}