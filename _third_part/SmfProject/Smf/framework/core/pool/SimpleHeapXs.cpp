#include "SimpleHeapXs.h"
#include "SmfDef.h"
#include "IPool.h"
#include "smf_debug.h"

using namespace smf;
SimpleHeapXs::SimpleHeapXs() {
}
SimpleHeapXs::~SimpleHeapXs() {
	Deinitialize();
}
bool SimpleHeapXs::Deinitialize() {
	if (_pairs) {
		smf::Free(_pairs);
		_pairs = 0;
	}
	if (_heads) {
		smf::Free(_heads);
		_heads = 0;
	}
	return true;
}
static int compare(const void* item, const void* src) {
	return ((smf_pair_t*)item)->size - ((smf_pair_t*)src)->size;
}
bool SimpleHeapXs::Initialize(smf_pair_t* pairs, unsigned count, unsigned nodemax) {
	//dbgMemPXL("%p:%d", buff, size);
	returnIfErrC(false, !pairs);
	returnIfErrC(false, !count);
	returnIfErrC(false, _pairs);
	//
	_pairs = (smf_pair_t*)smf::Alloc(sizeof(smf_pair_t) * (count));
	returnIfErrC(false, !_pairs);
	memcpy(_pairs, pairs, sizeof(smf_pair_t) * (count));
	qsort(pairs, sizeof(smf_pair_t), count, &compare);
	//
	if (nodemax < count)nodemax = count * 2;
	_nodemax = nodemax;
	_heads = (Node*)smf::Alloc(sizeof(Node) * nodemax);
	returnIfErrC(false, !_heads);
	memset(_heads, 0, sizeof(Node) * nodemax);
	//
	auto total = 0;
	for (int i = 0; i < count; i++) {
		auto& node = _heads[i];
		auto& pair = _pairs[i];
		node.refs = 0;
		node.size = toNodeSize(pair.size);
		node.next = 0;
		total += toNodeSize(pair.size);
	}
	//
	_pairc = count;
	_total = total;
	_free = total;
	_nodecount = count;
	_freecount = count;
	return true;
}

SimpleHeapXs::Node* SimpleHeapXs::NodeAlloc() {
	returnIfErrC(0, !_heads);
	for (int i = 0; i < _nodemax; i++) {
		auto node = _heads + i;
		if (!node->size) {
			_nodecount++;
			node->refs = 0;
			node->next = 0;
			return node;
		}
	}
	auto idx = _nodemax;
	_nodemax <<= 1;
	_heads = (Node*)Realloc(_heads, sizeof(Node) * _nodemax);
	returnIfErrC(0, !_heads);
	auto node = &_heads[idx];
	memset(node, 0, sizeof(Node) * idx);
	_nodecount++;
	return node;
}

void SimpleHeapXs::NodeFree(Node*node) {
	//Free(node);
	node->size = 0;
	_nodecount--;
}

void* SimpleHeapXs::Alloc(unsigned size_) {
	returnIfErrC(0, _heads);
	auto size = toNodeSizeUp(size_);
	returnIfErrC(0, !size);
	for (int i = 0; i < _pairc; i++) {
		auto& pair = _pairs[i];
		if (size_ <= pair.size) {
			auto node = _heads + i;
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
						node->next = left - _heads;
						node->size = size;
					}
					return (char*)pair.data + toSize(idx);
				}
				if (!node->next) {
					break;
				}
				idx += node->size;
				node = _heads + node->next;
			}
		}
	}
	dbgWarnPXL("%u,%d/%d", toSize(size), toSize(_free), toSize(_total));
	return 0;
}
bool SimpleHeapXs::Free(void* ptr) {
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
		auto next = _heads + node->next;
		if (!next->refs) {
			node->size += next->size;
			node->next = next->next;
			NodeFree(next);
		}
	}
	return true;
}

uint32_t SimpleHeapXs::GetSize(void* ptr) const {
	auto node = find(ptr);
	return node ? toSize(node->size) : 0;
}

SimpleHeapXs::Node* SimpleHeapXs::find(void* ptr, Node** prev_)const {
	returnIfErrC(0, !_heads);
	auto data = (char*)ptr;
	for (int i = 0; i < _pairc; i++) {
		auto& pair = _pairs[i];
		if (data >= (char*)pair.data && data < (char*)pair.data + pair.size) {
			auto index = 0;
			Node* prev = 0;
			auto offset = toNodeSize((char*)ptr - (char*)pair.data);
			auto node = _heads + i;
			while (1) {
				if (offset >= index && offset < index + node->size) {
					if (prev_)*prev_ = prev;
					return node;
				}
				if (!node->next) {
					break;
				}
				prev = node;
				index += node->size;
				node = _heads + node->next;
			}
			break;
		}
	}
	return 0;
}
char* SimpleHeapXs::Print(char* ptr, char* end) const {
	ptr = snprintf(ptr, end, "(%u/%u)(%u/%u/%u/%u)"
		, _free, _total
		,_pairc, _freecount, _nodecount, _nodemax
	);
	for (int i = 0; i < _pairc; i++) {
		auto& pair = _pairs[i];
		ptr = snprintf(ptr, end, "%u(%p,%u)"
			, i,pair.data, pair.size
		);
#if 1
		ptr = snprintf(ptr, end, "{");
		auto node = _heads + i;
		int n = 0;
		int idx = 0;
		while (1) {
			ptr = snprintf(ptr, end, "(%d,%d,%p/%u)"
				, n++, node->refs, (char*)pair.data + toSize(idx), toSize(node->size)
			);
			if (!node->next) {
				break;
			}
			idx += node->size;
			node = _heads + node->next;
		}
		ptr = snprintf(ptr, end, "}");
#endif
	}
	return ptr;
}