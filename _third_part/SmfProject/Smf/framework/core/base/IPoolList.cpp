#include "IPool.h"
#include "IPoolList.h"
#include "smf_debug.h"
#include "SmfFCC.h"
#include "SmfDef.h"
#include "mutex.h"

using namespace smf;
IPoolList::IPoolList(IPool** pools, int cnt) {
	memset(pools, 0, sizeof(IPool*) * cnt);
	_pools = pools;
	_cnt = cnt;
}
IPool** IPoolList::find(const char* name) const {
	return find(fcc64(name));
}
IPool** IPoolList::find(uint64_t id) const{
	for (int i = 0; i < _cnt; i++) {
		auto& pool = _pools[i];
		if (pool && pool->ID() == id) {
			return (IPool**)&pool;
		}
	}
	return 0;
}
void IPoolList::Add(IPool* pool_) {
	if (!_gbl)_gbl = pool_;
	for (int i = 0; i < _cnt; i++) {
		auto& pool = _pools[i];
		if (!pool) {
			pool = pool_;
			return;
		}
	}
}
IPool* IPoolList::Find(const char*name) const {
	auto ppool = find(name);
	return ppool ? *ppool : 0;
}
IPool* IPoolList::Find(uint64_t name) const {
	auto ppool = find(name);
	return ppool ? *ppool : 0;
}
void IPoolList::Remove(IPool* pool) {
	Remove(pool->Name());
}
void IPoolList::Remove(const char* name) {
	auto ppool = find(name);
	if(ppool)*ppool = 0;
}
void IPoolList::Remove(uint64_t name) {
	auto ppool = find(name);
	if (ppool)*ppool = 0;
}
void IPoolList::Delete(const char* name) {
	auto ppool = find(name);
	if (ppool && *ppool) {
		delete *ppool;
	}
}
void* IPoolList::Alloc(const char* name, int size, int align){
	auto ppool = find(name);
	return ppool?(*ppool)->Alloc(size,align):0;
}
void* IPoolList::Calloc(const char* name, int num, int size) {
	auto ppool = find(name);
	return ppool ? (*ppool)->Calloc(num, size) : 0;
}
void* IPoolList::Realloc(const char* name, void* ptr, int size) {
	auto ppool = find(name);
	return ppool ? (*ppool)->Realloc(ptr, size) : 0;
}
void IPoolList::Hold(const char* name, void* data) {
	auto ppool = find(name);
	if (ppool)
		(*ppool)->Hold(data);
}
void IPoolList::Free(const char* name, void*& data){
	auto ppool = find(name);
	if(ppool)
		(*ppool)->Free(data);
}
char* IPoolList::Print(char* ptr, char* end) const {
	//dbgMemPL();
	auto ptr0 = ptr + 5;
	ptr += snprintf(ptr, end - ptr, "pool{00(%s):", _gbl ? _gbl->Name() : "");
	int cnt = 0;
	for (int i = 0; i < _cnt; i++) {
		auto& pool = _pools[i];
		if (pool) {
			ptr = pool->Print(ptr, end);
			cnt++;
		}
	}
	ptr += snprintf(ptr, end - ptr, "},");
	//snprintf(ptr0, 2, "%02d", cnt);
	*ptr0++ = '0' + cnt / 10;
	*ptr0++ = '0' + cnt % 10;
	return ptr;
}
void IPoolList::Print() const {
	//dbgMemPL();
	unique_lock<mutex> lck(Mutex());
	for (int i = 0; i < _cnt; i++) {
		auto& pool = _pools[i];
	//for (auto pool : _pools) {
		if (pool) {
			pool->Print();
		}
	}
}
