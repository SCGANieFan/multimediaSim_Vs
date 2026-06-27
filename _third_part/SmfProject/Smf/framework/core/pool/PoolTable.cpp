#include "PoolTable.h"
#include "smf_debug.h"
#include "smf_arm_dsp.h"
#include "SmfString.h"
#include "ObjectStatic.h"
//
using namespace smf;
///
PoolTable::PoolTable(bool record):IPool(record){

}
PoolTable::~PoolTable() {
	if (_pool)_pool->Free(_items);
}
bool PoolTable::Initialize(item_t*items, IPool*pool) {
	returnIfErrC(false, !items);
	auto size = GetBuffSize(items);
	returnIfErrC(false, !size);
	if (!pool)pool = &BasePool();
	auto buff = pool->Alloc(size);
	returnIfErrC(false, !buff);
	returnIfErrC(false, !Initialize(buff, size, items));
	_pool = pool;
	return true;
}
bool PoolTable::Initialize(void*& buff, int size, item_t*items) {
	returnIfErrC(false, !buff);
	returnIfErrC(false, !size);
	returnIfErrC(false, !items);
	returnIfErrC(false, size<GetBuffSize(items));
	auto item = items;
	auto ptr = (char*)buff;
	auto end = (char*)buff + size;
	auto cnt = 0;
	while (item && item->size) {
		cnt++;
		item++;
	}
	//
	_items = (CItem*)ptr;
	_count = cnt;
	ptr += sizeof(CItem) * cnt;
	//
	auto total = 0;
	for (int i = 0; i < cnt; i++) {
		if (!_items[i].Initialize(ptr, end, items[i].size, items[i].num)) {
			return false;
		}
		total += _items[i].Size();
	}
	//
	buff = ptr;
	return true;
}
int PoolTable::GetBuffSize(item_t* items) {
	auto item = items;
	auto total = 0;//sizeof(CItem);

	while (item->size) {
		auto size = item->size * item->num + ((item->num + 31) >> 5 << 2);
		total += size + sizeof(CItem);
		item++;
	}
	
	return total;
}
void* PoolTable::alloc(unsigned size, unsigned align) {
	for (int i = 0; i < _count; i++) {
		auto& item = _items[i];
		if (size <= item._blksize) {
			auto ptr = item.Alloc();
			if (ptr) {
				//return Record(ptr, size);
				//dbgTestPXL("ptr %p size %d block %d %d", ptr, size, item._blksize, item._blknum);
				return ptr;
			}
		}
	};
	auto pptr = _pool->Alloc(size);
	//dbgTestPXL("pptr %p size %d", pptr, size);
	return pptr;
}
bool PoolTable::free(void* ptr) {
	//dbgTestPXL("ptr %p", ptr);
	for (int i = 0; i < _count; i++) {
		auto& item = _items[i];
		if (item.Free(ptr)) {
			return true;
		}
	};
	return _pool->Free(ptr);
}
int PoolTable::get_free()const {
	auto size = 0;
	for (int i = 0; i < _count; i++) {
		size += _items[i].FreeSize();
	}
	return size;
}
int PoolTable::get_total()const {
	auto size = 0;
	for (int i = 0; i < _count; i++) {
		size += _items[i].Size();
	}
	return size;
}
bool PoolTable::is_valid()const {
	return true;
}
void PoolTable::foreach(const std::function<bool(CItem&)>& func) {
	for (int i = 0; i < _count; i++) {
		func(_items[i]);
	};
}
int PoolTable::get_size(void* ptr) const {
	for (int i = 0; i < _count; i++) {
		auto& item = _items[i];
		if (item.IsValid(ptr)) {
			return item._blksize;
		}
	};
	return 0;
}
//void PoolTable::print()const {
//	for (int i = 0; i < _count; i++) {
//		_items[i].Print();
//	}
//}
char* PoolTable::print(char* ptr, char* end) const {
	for (int i = 0; i < _count; i++) {
		ptr = _items[i].Print(ptr, end);
	}
	return ptr;
}
///
bool PoolTable::CItem::Initialize(char*& buff, char* buff_end
	, int blksize, int blknum
) {
	//blknum = (blknum + 31) / 32 * 32;
	auto mapsize32 = ((blknum + 31) >> 5);
	auto used = blksize * blknum + (mapsize32<<2);
	auto size = blksize * blknum + (blknum >> 3);
	//dbgTestPDL(size);
	returnIfErrC(false, buff + size > buff_end);
	_blknum = blknum;
	_blksize = blksize;
	_maps = (uint32_t*)buff;
	//dbgTestPXL("blksize %d blknum %d _maps %p",blksize, blknum, _maps);
	_buff = (char*)((uint32_t*)buff + mapsize32);
	buff += used;
	memset(_maps, 0, (mapsize32<<2));
	return true;
}

void* PoolTable::CItem::Alloc() {
	for (int i = 0; i <= (_blknum >> 5); i++) {
		auto msk = _maps[i];
		if (msk != 0xffffffff) {
			for (int n = 0; n < 32; n++) {
				if (!(msk & 1)) {
					_maps[i] |= 1 << n;
					auto idx = (i << 5) + n;
					if (idx >= _blknum) {
						return NULL;
					}
					//dbgTestPDL(idx);
					return _buff + idx*_blksize;
				}
				msk >>= 1;
			}
		}
	}	
	return NULL;
}
bool PoolTable::CItem::Free(void*ptr) {
	if ((ptr < _buff) || (ptr >= _buff+ Size()))
		return false;
	int idx = ((char*)ptr - (char*)_buff)/_blksize;
	int i = idx >> 5;
	int ii = idx & 0x1f;	
	_maps[i] &= ~(1<<ii);
	//dbgTestPXL("%p,%u,,%u,%u,%08x,%d", ptr, _size, _used, _num, mask, i);
	return true;
}
bool PoolTable::CItem::IsValid(void*ptr)const {
	return (char*)ptr >= _buff && (char*)ptr < _buff + Size();
}
void PoolTable::CItem::Print() const {
	dbgTestPXL("[%d*%d]%d,", _blksize, _blknum, FreeBlockNum());
}
char* PoolTable::CItem::Print(char* ptr, char* end) const {
	auto freeNum = FreeBlockNum();
	ptr += snprintf(ptr, end - ptr, "[%d*%d]%d,", _blksize, _blknum, freeNum);
	return ptr;
}
uint16_t PoolTable::CItem::FreeBlockNum()const {
	auto cnt = 0;
	for (int i = 0; i < (_blknum >> 5); i++) {
		auto msk = _maps[i];
		if (msk != 0xffffffff) {
			for (int n = 0; n < 32; n++) {
				if (msk & 1) {
					cnt++;
				}
			}
		}
	}
	return cnt;
}
uint32_t PoolTable::CItem::FreeSize()const {
	return _blksize * FreeBlockNum();
}
uint32_t PoolTable::CItem::Size()const {
	return _blksize * _blknum;
}
