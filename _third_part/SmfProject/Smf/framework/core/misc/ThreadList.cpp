#include <memory>
#include "ThreadList.h"
#include "SmfFCC.h"
#include "SmfDef.h"
#include "SmfPorting.h"
#include "smf_debug.h"

using namespace smf;
using Item_t = ThreadList::Item_t;

void ThreadList::Init(Item_t* items, int cnt) {
	returnIfErrC0(!items);
	returnIfErrC0(!cnt);
	_items = items; 
	_cnt = cnt; 
	memset(items, 0, sizeof(Item_t) * cnt);
	_items[0]._name = fcc32("IDLE");
	_items[0]._id = SmfGetThreadID("IDLE");
}
Item_t* ThreadList::Hold(const char* name) {
	auto id = fcc64(name);
	for (int i = 0; i < _cnt; i++) {
		if (!_items[i]._name) {
			memset(&_items[i], 0, sizeof(Item_t));
			_items[i]._name = id;
			return &_items[i];
		}
	}
	dbgWarnPL();
	for (int i = 0; i < _cnt; i++) {
		if (_items[i]._name) {
			char name[9] = {0};
			memcpy(name, &_items[i]._name, 8);
			dbgWarnPXL("[%d] %s",i,name);
		}
	}
	return 0;
}
//void ThreadList::Release(const char* name) {
//	auto item = Find(name);
//	if (item)item->Release();
//}
//Item_t* ThreadList::Find(const char*name)const {
//	auto id = fcc64(name);
//	for (int i = 0; i < _cnt; i++) {
//		if (_items[i]._name == id) {
//			return &_items[i];
//		}
//	}
//	return 0;
//}
void ThreadList::Update() {
	for (int i = 0; i < _cnt; i++) {
		_items[i].Update();
	}
}
uint32_t ThreadList::Idle()const {
	auto item = _items[0];
	auto usage = item._usage;
	return usage;
}
uint32_t ThreadList::Usage()const {
	return 10000 - Idle();
}
uint32_t ThreadList::Workloads()const {
	uint32_t usage = 0;
	for (int i = 1; i < _cnt; i++) {
		usage += _items[i]._usage;
	}
	return usage;
}
char* ThreadList::Print(char* ptr, char* end)const {
	ptr = snprintf(ptr, end, "thread{");
	for (int i = 0; i < _cnt; i++) {
		if (_items[i]._name) {
			ptr=_items[i].Print(ptr,end);
		}
	}
	ptr = snprintf(ptr, end, "}");
	return ptr;
}

void ThreadList::Item_t::Update() {
	if (_name) {
		SmfThreadUpdateInfo((ThreadItem_t*)this);
	}
	else {
		_usage = 0;
	}
}

uint32_t ThreadList::Item_t::Usage() const {
	return _usage;
}
char* ThreadList::Item_t::Print(char* ptr, char* end)const {
	return snprintf(ptr, end, "%s(%u.%02u%%)%d/%d,", (char*)&_name, _usage / 100, _usage % 100, _stackFreeMin, _stackSize);
}

uint32_t smf::get_cpu_usage() {
	return ThreadList::List().Usage();
}

uint32_t smf::get_workloads() {
	return ThreadList::List().Workloads();
}

ThreadItem_t* smf::SmfThreadHold(const char* name) {
	unique_lock<mutex> lock( ThreadList::List()._mtx);
	return ThreadList::List().Hold(name);
}

void smf::SmfThreadRelease(ThreadItem_t* item) {
	unique_lock<mutex> lock( ThreadList::List()._mtx);
	((ThreadList::Item_t*)item)->Release();
}
