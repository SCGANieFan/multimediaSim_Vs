#include <memory>
#include "SmfDef.h"
#include "MemoryNamed.h"
#include "smf_debug.h"
#include "SmfFCC.h"
#include "SmfHash.h"
#include "SmfPorting.h"

using namespace smf;
#define dbgPoolPXL(s,...) //dbgMemPXL(s,##__VA_ARGS__)
MemoryNamed::~MemoryNamed() {
	if (_items && _pool) {
		_pool->Free(_items);
	}
}
bool MemoryNamed::Init(IPool* pool, int cnt) {
	returnIfErrC(false, !pool);
	returnIfErrC(false, !cnt);
	_pool = pool;
	_itemsCnt = cnt;
	_items = (item_t**)_pool->Calloc(sizeof(item_t*), cnt);
	returnIfErrC(false, !_items);
	return true;
}
void* MemoryNamed::alloc(const char* name, void* buff, unsigned& size, unsigned align) {
	unique_lock<mutex> lck(_mtx);
	if (name && size)return alloc(Hash(name, 8), size, align);
	else if (name) return alloc(Hash(name, 8), size);
	else if (buff) return alloc(buff, size);
	else if (size) return alloc(UniqueID(), size, align);
	else return 0;
}
bool MemoryNamed::free(const char* name, void* buff) {
	unique_lock<mutex> lck(_mtx);
	//dbgTestPXL("%p,%p", name, buff);
	return free(name ? find(Hash(name, 8)) : find(buff));
}
//
void* MemoryNamed::alloc(uint32_t hash, unsigned size, unsigned align) {
	auto item_ = find(hash);
	if (item_) {
		auto item = *item_;
		item->cnt++;
		//dbgTestPXL("[%08x]%p,%u,%u,%u", item->hash, item->ptr(), item->size, item->offset, item->cnt);
		return item->ptr();
	}
	auto item = allocX(hash,size,align);
	returnIfErrC(0, !item);
	//dbgTestPXL("[%08x]%p,%u,%u,%u", item->hash, item->ptr(), item->size, item->offset, item->cnt);
	return item->ptr();
}
void* MemoryNamed::alloc(uint32_t hash, unsigned& size) {
	auto item_ = find(hash);
	if (item_) {
		auto item = *item_;
		item->cnt++;
		size = item->size;
		//dbgTestPXL("[%08x]%p,%u,%u,%u", item->hash, item->ptr(), item->size, item->offset, item->cnt);
		return item->ptr();
	}
	return 0;
}
void* MemoryNamed::alloc(void* buff, unsigned& size) {
	auto item_ = find(buff);
	if (item_) {
		auto item = *item_;
		item->cnt++;
		size = item->size;
		//dbgTestPXL("[%08x]%p,%u,%u,%u", item->hash, item->ptr(), item->size, item->offset, item->cnt);
		return item->ptr();
	}
	return 0;
}

MemoryNamed::item_t** MemoryNamed::find(uint32_t hash) {
	returnIfErrC(0, !_items);
	//for (auto& item : _items) {
	for (auto i = 0; i < _itemsCnt;i++) {
		auto& item = _items[i];
		if (item && (item->hash == hash)) {
			return &item;
		}
	}
	return 0;
}
MemoryNamed::item_t** MemoryNamed::find(void* buff) {
	returnIfErrC(0, !_items);
	//for (auto& item : _items) {
	for (auto i = 0; i < _itemsCnt; i++) {
		auto& item = _items[i];
		if (item && ((item->buff == buff) || (item->ptr() == buff))) {
			return &item;
		}
	}
	return 0;
}

MemoryNamed::item_t* MemoryNamed::allocX(uint32_t hash, unsigned size, unsigned align) {
	returnIfErrC(0, !_pool);
	returnIfErrC(0, !_items);
	//for (auto& item : _items) {
	size = (size + 63) & ~63;
	if (align < 64)align = 64;
	for (auto i = 0; i < _itemsCnt; i++) {
		auto& item = _items[i];
		if (!item) {
			//item = (item_t*)BasePool().Alloc(sizeof(item_t) + size + align + align);
			item = (item_t*)_pool->Alloc(sizeof(item_t) + size + align + align);
			returnIfErrC(0, !item);
			memset(item, 0, sizeof(item_t) + size + align + align);
			item->hash = hash;
			item->size = size;
			item->cnt = 1;
			if (align) {
				auto addr = (uint32_t)item->buff;
				auto mask = align - 1;
				auto algn = (addr + mask) & ~mask;
				item->offset = algn - addr;
			}
			cache_flush(item->ptr(), size);
			//dbgTestPXL("[%08x]%p,%u,%u,%u", item->hash, item->ptr(), item->size, item->offset, item->cnt);
			return item;
		}
	}
	return 0;
}
bool MemoryNamed::free(item_t** item_) {
	returnIfErrC(false, !_pool);
	returnIfErrC(false, !item_);
	auto& item = *item_;
	//dbgTestPXL("[%08x]%p,%u,%u,%u", item->hash, item->ptr(), item->size, item->offset, item->cnt);
	if (!item->cnt) {
		dbgWarnPL();
		return true;
	}
	item->cnt--;
	if (!item->cnt) {
		//dbgTestPXL("[%08x]%p,%u,%u,%u", item->hash, item->ptr(), item->size, item->offset, item->cnt);
		_pool->Free(item);
		item = 0;
	}
	return true;
};
