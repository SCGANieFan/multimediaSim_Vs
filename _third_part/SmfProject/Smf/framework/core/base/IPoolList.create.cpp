#include "IPool.h"
#include "PoolAlias.h"
#include "PoolCallback.h"
#include "IPoolList.h"
#include "SmfHash.h"
#include "SmfString.h"
#include "SmfPorting.h"
#include "smf_debug.h"
#include "MemoryPoolHeap.h"
#include "MemoryPoolHeaps.h"
#include "MemoryPoolHeapX.h"
#include "MemoryPoolHeapXs.h"
#include "MemoryPoolBuffers.h"
#include <string>
using namespace smf;
// void* operator new(size_t size, void* ptr);
IPool* IPoolList::Register(const char* name, void* buff, unsigned size, unsigned thr, bool remote) {
	//dbgTestPXL("%s:%p,%u,%u,%u", name, buff, size, thr, remote);
	if (remote) {
		buff = addr_remap_to_cpu(buff);
		cache_invalid(buff, size);
	}
	auto pool = new (buff) MemoryPoolHeap();
	if (pool) {
		pool->Name(name);
		pool->Initialize((char*)buff + sizeof(MemoryPoolHeap), size - sizeof(MemoryPoolHeap), thr);
	}
	return pool;
}
IPool* IPoolList::RegisterX(const char* name, void* buff, unsigned size, unsigned node_count, bool remote) {
	//dbgTestPXL("%s:%p,%u,%u,%u", name, buff, size, node_count, remote);
	if (remote) {
		buff = addr_remap_to_cpu(buff);
		cache_invalid(buff, size);
	}
	auto pool = new MemoryPoolHeapX(); //dbgTestPPL(pool);
	if (pool) {
		pool->Name(name); //dbgTestPL();
		pool->Initialize((char*)buff, size, node_count);
	}
	return pool;
}
#if 0
IPool* IPoolList::RegisterXs(const char* name, smf_pair_t* pairs, unsigned count, bool remote) {
	if (remote) {
		for (int i = 0; i < count; i++) {
			auto pair = pairs + i;
			pair->data = addr_remap_to_cpu(pair->data);
			cache_invalid(pair->data, pair->size);
		}
	}
	auto pool = new MemoryPoolHeapXs(); //dbgTestPPL(pool);
	if (pool) {
		pool->Name(name); //dbgTestPL();
		pool->Initialize(pairs, count, 0);
	}
	return pool;
}
#endif
IPool* IPoolList::RegisterBuffers(const char* name, smf_pair_t* pairs, unsigned count, bool remote) {
	//dbgTestPXL("%s:%u,%u", name, count, remote);
	if (remote) {
		for (int i = 0; i < count; i++) {
			auto pair = pairs + i;
			//dbgTestPXL("%u:%p,%u", i, pair->data, pair->size);
			pair->data = addr_remap_to_cpu(pair->data);
			cache_invalid(pair->data, pair->size);
		}
	}
	auto pool = new MemoryPoolBuffers(); //dbgTestPPL(pool);
	if (pool) {
		pool->Name(name); //dbgTestPL();
		pool->Initialize(pairs, count);
	}
	return pool;
}
IPool* IPoolList::RegisterAlias(const char* alias, const char* name) {
	//dbgTestPXL("%s:%s", alias, name);
	auto pool = Find(name);
	returnIfErrC(0, !pool);
	return new PoolAlias(alias, pool);
}
IPool* IPoolList::RegisterGloble(void* buff, unsigned size, unsigned thr) {
	auto name = "gbl";
	//dbgTestPXL("%s:%u,%u", name, size, thr);
	auto pool = _gbl;//IPool::Find(name);
	int oft = 0;
	if (!pool) {
		pool = new (buff) MemoryPoolHeaps();
		pool->Name(name);
		oft = sizeof(MemoryPoolHeaps);
		_gbl = pool;
	}
	if (pool) {
		auto heaps = (MemoryPoolHeaps*)pool;
		returnIfErrC(0, heaps->_flags != MemoryPoolHeaps::eFlags);
		returnIfErrC(0, !heaps->Add((char*)buff + oft, size - oft, thr));
	}
	return pool;
}
bool IPoolList::UnregisterGloble(void* buff) {
	auto name = "gbl";
	auto pool = IPool::Find(name);
	returnIfErrC(false, !pool);
	auto heaps = (MemoryPoolHeaps*)pool;
	returnIfErrC(0, !heaps->Remove(buff));
	return true;
}
IPool* IPoolList::Register(const char* name
	, smf_cb_malloc cbAlloc
	, smf_cb_free cbFree
	, smf_cb_hold cbHold
	, smf_cb_realloc cbRealloc
	, smf_cb_getsize cbGetSize
	, smf_cb_gettotal cbGetTotal
) {
	returnIfErrC(0, !cbAlloc);
	auto buff = cbAlloc(sizeof(PoolCallback));
	auto pool = new(buff) PoolCallback(cbAlloc, cbFree, cbHold, cbRealloc, cbGetSize, cbGetTotal);
	if (pool) {
		pool->Name(name);
	}
	return pool;
}
bool IPoolList::Command(const char* cmd_) {
	char* cmd = (char*)cmd_;
	auto cmd0 = cmd;
	uint32_t hash = HashX((const char*&)cmd, 32);
	//returnIfErrC(false, *cmd != ':');
	//cmd++;
	switch (hash) {
	case Hash("addBuffers"): {//addBuffers:name,count,0x12345,250,0x12354,123,1
		auto name = splite(cmd, ','); *(char*)cmd++ = 0;
		auto count = strtoul(cmd, &cmd, 0); cmd++;
		smf_pair_t pairs[8];
		for (int i = 0; i < count; i++) {
			pairs[i].data = (void*)strtoul(cmd, &cmd, 0); cmd++;
			pairs[i].size = (int)strtoul(cmd, &cmd, 0); cmd++;
		}
		auto remote = (int)strtoul(cmd, &cmd, 0); cmd++;
		return RegisterBuffers(name, pairs, count, remote);
	}
	case Hash("addX"): {//addX:name,addr,size,remote
		auto name = splite(cmd, ','); *(char*)cmd++ = 0;
		auto addr = (void*)strtoul(cmd, &cmd, 0); cmd++;
		auto size = (int)strtoul(cmd, &cmd, 0); cmd++;
		auto remote = (int)strtoul(cmd, &cmd, 0);
		return RegisterX(name, addr, size, remote);
	}
	case Hash("add"): {//addX:name,addr,size,thr,remote
		auto name = splite(cmd, ','); *(char*)cmd++ = 0;
		auto addr = (void*)strtoul(cmd, &cmd, 0); cmd++;
		auto size = (int)strtoul(cmd, &cmd, 0); cmd++;
		auto thr = (int)strtoul(cmd, &cmd, 0);
		auto remote = (int)strtoul(cmd, &cmd, 0);
		return Register(name, addr, size, thr, remote);
	}
	default:
		dbgErrPXL("unknown cmd:%s",cmd0);
		return false;
	}
}

bool IPoolList::Set(uint32_t key, void* val) {
	switch (key) {
	case Hash("remotePoolAdd"):Register((const char*)val, ((void**)val)[2], ((uint32_t*)val)[3], ((uint32_t*)val)[4], true);return true;
	case Hash("remotePoolAddx"):RegisterX((const char*)val, ((void**)val)[2], ((uint32_t*)val)[3], 0, true);return true;
	//case Hash("remotePoolAddxs"):RegisterXs((const char*)val, &((smf_pair_t*)val)[2], ((uint32_t*)val)[2], true); return true;
	case Hash("remotePoolAddBuffers"):RegisterBuffers((const char*)val, &((smf_pair_t*)val)[2], ((uint32_t*)val)[2], true); return true;
	//case Hash("remotePoolAddNew"):Register((const char*)val, ((uint32_t*)val)[2]); return true;
	case Hash("remotePoolAddAlias"):RegisterAlias((const char*)val, (const char*)val + 8); return true;
	case Hash("remotePoolRemove"):Remove((const char*)val);return true;

	//case Hash("poolAdd"):Register((const char*)val, ((void**)val)[2], ((uint32_t*)val)[3], ((uint32_t*)val)[4], true); return true;
	//case Hash("poolAddx"):RegisterX((const char*)val, ((void**)val)[2], ((uint32_t*)val)[3], 0, true); return true;
	//	//case Hash("poolAddxs"):RegisterXs((const char*)val, &((smf_pair_t*)val)[2], ((uint32_t*)val)[2], true); return true;
	//case Hash("poolAddBuffers"):RegisterBuffers((const char*)val, &((smf_pair_t*)val)[2], ((uint32_t*)val)[2], ((uint32_t*)val)[3]); return true;
	//	//case Hash("poolAddNew"):Register((const char*)val, ((uint32_t*)val)[2]); return true;
	//case Hash("poolAddAlias"):RegisterAlias((const char*)val, (const char*)val + 8); return true;
	//case Hash("poolRemove"):Remove((const char*)val); return true;
	//
	case Hash("poolCmd"):Command((const char*)val); return true;
	default:
		return false;
	}
}
