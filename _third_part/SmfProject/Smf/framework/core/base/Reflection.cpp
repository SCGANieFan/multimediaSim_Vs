#include "Reflection.h"
#include "Object.h"
#include "SmfFCC.h"
#include "SmfHash.h"
#include "SmfDef.h"
#include "smf_debug.h"
#include "chip.h"

using namespace smf;

Reflection::Reflection(item_t* items, int cnt) {
	memset((void*)items, 0, cnt * sizeof(item_t));
	_items = items;
	_count = cnt;
}
int Reflection::Finds(const char* type, item_t* items[], int max) const {
	return Finds(Convert(type),items,max);
}
int Reflection::Finds(uint32_t type, item_t* items[], int max) const {
	int n = 0;
	for (auto i = 1; i < _count; i++) {
		auto& item = _items[i];
		if (!item.id || !item.name || !item.creater)return n;
		auto temp = item.Type();
		if ( Convert((char*)(&temp)) == type) {
			items[n++] = (item_t*)&item;
			if (n >= max)return n;
		}
	}
	return n;
}
Reflection::item_t* Reflection::Register(const char* name, FuncCreate creater, void* param) {
	auto id = Convert(name);
	for (auto i = 0; i < _count; i++) {
		auto& item = _items[i];
		if (!item.id || (item.id == id)) {
			item.id = id;
			item.name = name;
			item.creater = creater;
			item.param = param;
			return &item;
		}
	}
	dbgErrPXL("%s:%08x,%p(%p)",name,id,creater,param);
	return NULL;
}

Reflection::item_t* Reflection::ReRegister(const char* name, uint32_t src) {
	auto item = Find(src);
	returnIfErrC(NULL, !item);
	return Register(name,item->creater);
}

bool Reflection::UnRegister(const char* name) {
	returnIfWarnC(false, !name);
	auto id = Convert(name);
	for (auto i = 0; i < _count; i++) {
		auto& item = _items[i];
		if (item.id == id){
			item.id = 0;
			item.creater = 0;
			return true;
		}
	}
	dbgWarnPXL("fail:%s", name);
	return false;
}

Reflection::item_t* Reflection::Find(uint32_t id)const {
	for (auto i = 0; i < _count; i++) {
 		auto& item = _items[i];
		if (item.id == id){
			return (item_t*)&item;
		}
	}
	dbgErrPXL("fail:%08x",id);
	return NULL;
}
Object* Reflection::Create(uint32_t id)const {
	Load(id);
	auto item = Find(id);
	return Create(item);
}
Object* Reflection::Create(item_t* item)const {
	if (item) {
		if(item->creater) {
			Object* obj = item->creater();
			if (obj) {
				obj->initialize(item);
				return obj;
			}
		}
	}
	return NULL;
}
uint64_t Reflection::item_t::IDs(int idx) const {
	auto ptr = name;
	auto rst = 0ull;
	for (int i = 0; i <= idx; i++) {
		if (!*ptr)return 0;
		rst = fcc64x(ptr);
		ptr++;
	}
	return rst;
}

bool Reflection::Load(uint32_t id, void* para) const {
	ILoader* loader = 0;
	return  _loaders.Get(id, loader) ? loader->Load(para) : false;
}
bool Reflection::Unload(uint32_t id, void* para, bool invoke) const {
	ILoader* loader = 0;
	if (_loaders.Get(id, loader)) {
		if (invoke) {
			Invoke([](void* priv) {
				((ILoader*)priv)->Unload(0);
				return true;
				},loader, 1);
		}
		else {
			loader->Unload(para);
		}
	}
	return true;
}

bool Reflection::item_t::Load() const {
	return Reflect().Load(id);
}
bool Reflection::item_t::Unload() const {
	return Reflect().Unload(id, 0, true);
}
