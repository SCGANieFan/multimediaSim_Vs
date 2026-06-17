#include "IPool.h"
#include "string.h"
#include "stdio.h"
#include "smf_debug.h"
#include "smf_common.h"
#include "unique_ptr.h"
#include "SmfFCC.h"
#include "SmfDef.h"
#include "SmfPorting.h"
#include "mutex.h"
#define dbgPoolPXL(s,...) dbgMemPXL(s,##__VA_ARGS__)
#define poolPrint(ptr, size, align) dbgPoolPXL("[%s]%p,%6u,%u,%u/%u,%u/%d,%p/%p", Name(), ptr, size, align, GetFree(), GetTotal(), GetUsed(), GetUsedMax(), _begin, _end)
using namespace smf;

IPool::IPool(bool record){	
	if (record) {
		Pools().Add(this);
	}
}
IPool::~IPool() {
	unique_lock<mutex> lck(Mutex());
	Pools().Remove(this);
}
const char* IPool::Name() const {
	return (const char*)&_id;
}
void IPool::ID(uint64_t id) {
	_id = id;
}
void IPool::Name(const char*name) {
	ID(fcc64(name));
}
void IPool::SetBackup(IPool* bkpool) { 
	_bkpool = bkpool; 
}
void IPool::SetBackup(const char* name) {
	_bkpool = Pools().Find(name);
}
void IPool::SetGloble() const {
	Pools()._gbl = (IPool*)this;
}
void* IPool::record(void* ptr, unsigned size, unsigned align) {
	returnIfWarnCS(0, !ptr, "%s,%d", Name(), size);
	if (_begin > ptr)_begin = (char*)ptr;
	if (_end < (char*)ptr + size)_end = (char*)ptr + size;
	auto used = GetUsed();
	if (_usedMax < used)_usedMax = used;
	return ptr;
}

bool IPool::Free(void*& ptr) {
	if (!ptr) {
		return true;
	}
	if (IsInside(ptr)) {
		auto ptr0 = ptr;
		auto size = get_size(ptr);
		if (free(ptr)) {
			ptr = 0;
		}
		poolPrint(ptr0, size, 0);
		return true;
	}
	else if (_bkpool) {
		return _bkpool->Free(ptr);
	}
	else {
		dbgWarnPXL("nomem[%s]%p", Name(), ptr);
		//dbgWarnPXL("nomem[%s]%p,%p", Name(), ptr, __builtin_return_address(0));
		return false;
	}
}
bool IPool::Hold(void* ptr) {
	if (IsInside(ptr)) {
		poolPrint(ptr, 0, 0);
		return hold(ptr);
	}
	else if (_bkpool) {
		return _bkpool->Hold(ptr);
	}
	else {
		dbgWarnPXL("nomem[%s]%p", Name(), ptr);
		return false;
	}
}
void* IPool::Alloc(unsigned size, unsigned align) {
	// dbgPoolPXL("%u",size);
	if(size == 0){return 0;}
	auto rst = alloc(size, align);
	if (rst) {
		record(rst, size, align);
		poolPrint(rst, size, align);
		return rst;
	}
	else if(_bkpool){
		return _bkpool->Alloc(size, align);
	}
	else {
		dbgWarnPXL("nomem[%s]%6u,%u,%u/%u,%u/%d"
			, Name(),size, align
			, GetFree(), GetTotal()
			, GetUsed(), GetUsedMax()
		);
		return 0;
	}
}
void* IPool::Realloc(void* ptr, unsigned size, unsigned oldsize) {
	if (!ptr) {
		return Alloc(size);
	}
	else if (IsInside(ptr)) {
		Hold(ptr);
		auto buf = realloc(ptr, size);
		if (buf) {
			record(buf, size, 0);
			poolPrint(buf, size, 0);
			Free(ptr);
			return buf;
		}
		else if (_bkpool) {
			void* rst = _bkpool->Alloc(size, 0);
			if (!rst) {
				dbgWarnPXL("nomem[%s]%p,%d", rst, size);
				Free(ptr);
				return NULL;
			}
			else {
				auto size0 = GetSize(ptr);
				memcpy(rst, ptr, size0);
				dbgMemPXL("%p~%p,%u~%u", ptr, rst, size0, size);
				Free(ptr);
				poolPrint(rst, size, 0);
				record(rst, size, 0);
				return rst;
			}
		}
		else {
			Free(ptr);
			return 0;
		}
	}
	else if (_bkpool) {
		return _bkpool->Realloc(ptr, size);
	}
	else {
		dbgWarnPXL("nomem[%s]%p,%6u,%u,%u/%u,%u/%d"
			, Name(), ptr, size, oldsize
			, GetFree(), GetTotal()
			, GetUsed(), GetUsedMax()
		);
		return 0;
	}
}
void* IPool::Calloc(unsigned num, unsigned size) {
	// dbgPoolPXL("%u",size);
	if((num * size) == 0){return 0;}
	auto rst = calloc(num, size);
	if (rst) {
		record(rst, size* num, 0);
		poolPrint(rst, size * num, 0);
		return rst;
	}
	else if (_bkpool) {
		return _bkpool->Calloc(num, size);
	}
	else {
		dbgWarnPXL("nomem[%s]%6u=%u*%u,%u/%u,%u/%d"
			, Name(), size* num, num, size
			, GetFree(), GetTotal()
			, GetUsed(), GetUsedMax()
		);
		return 0;
	}
}
void* IPool::realloc(void* ptr, unsigned size) {
	if (!ptr) {
		return Alloc(size);
	}
	int size0 = GetSize(ptr);
	if ((int)size <= size0) {
		///unable free up extra space;
		return ptr;
	}
	void* rst = alloc(size, 0);
	if (!rst) {
		dbgWarnPXL("nomem[%s]%p,%d", rst, size);
		Free(ptr);
		return NULL;
	}
	int cpsize = size0;
	if (!cpsize) {///unable get the old buffer size
		cpsize = size;
	}
	memcpy(rst, ptr, cpsize);
	dbgMemPXL("%p~%p,%u~%u", ptr, rst, size0, size);
	Free(ptr);
	return rst;
}
void* IPool::calloc(unsigned num, unsigned size) {
	void* ptr = alloc(size * num, 8);
	if (!ptr) {
		return NULL;
	}
	memset(ptr, 0, size * num);
	return ptr;
}
char* IPool::print(char* ptr, char* end) const {
	ptr = snprintf(ptr, end, "%s:%d/%d/%d,", Name()
		, GetFree(), GetTotal(), GetUsedMax());
	return ptr; 
}
void IPool::Print() const {
	char buff[512];
	memset(buff, 0, 512);
	auto ptr = (char*)buff;
	auto end = ptr + sizeof(buff) - 2;
	auto ptr0 = ptr;
	ptr=Print(ptr, end);
	*ptr++ = '\n';
	dbgOutput(dbg_chn_test, ptr0, ptr - ptr0);
}
char* IPool::Print(char* ptr, char* end) const {
	return print(ptr,end);
}
int IPool::get_free()const {
	return 0x7fffffff;//_usedLimit;
}
int IPool::get_total()const {
	return 0x7fffffff;//_usedLimit;
}
int IPool::get_size(void* ptr)const {
	return 0;
}
void* IPool::CloneMemory(const smf_pair_t& pair) {
	return CloneMemory(pair.data, pair.size);
}
void* IPool::CloneMemory(const void*src, int len) {
	auto ptr = Alloc(len);
	returnIfErrC(0, !ptr);
	memcpy(ptr, src, len);
	return ptr;
}
char* IPool::CloneString(const char*str) {
	return (char*)CloneMemory(str, strlen(str) + 1);
}
void* IPool::AllocUncache(unsigned size_) {
	auto size = Align64(size_);
	auto addr = Alloc(size, 64);
	returnIfErrC(0, !addr);
	auto uaddr = address_to_uncache(addr);
	if (!uaddr) {
		Free(addr);
	}
	return uaddr;
}
bool IPool::FreeUncache(void*& uaddr) {
	returnIfErrC(false, !uaddr);
	auto addr = address_to_cache(uaddr);
	returnIfErrC(false, !addr);
	Free(addr);
	uaddr = 0;
	return true;
}