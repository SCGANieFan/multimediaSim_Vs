#include "gaapi_static.h"
using namespace gaapi_ns;

static uint64_t buff[(sizeof(GaapiStatic_c) + 7) >> 3] = { 0 };


GaapiGafRegister_c::GaapiGafRegister_c(Item_c* items, uint32_t itemNum) {
	_items = items;
	_itemNum = itemNum;
}
GaapiGafRegister_c::~GaapiGafRegister_c() {}
void GaapiGafRegister_c::Register(const char* type, FuncCreate_t funcCreate) {
	GaapiAutoMutex_c autoMtx(&_mtx);
	uint64_t id = Str2Key(type);
	for (uint32_t n = 0; n < _itemNum; n++) {
		if (_items[n]._id == id) {
			_items[n]._funcCreate = funcCreate;
			break;

		}
	}
	for (uint32_t n = 0; n < _itemNum; n++) {
		if (!_items[n]._id) {
			_items[n]._id = id;
			_items[n]._funcCreate = funcCreate;
			break;
		}
	}

}
GaapiGaf_c* GaapiGafRegister_c::Create(const char* type, GaapiBasePort_t* bp) {
	uint64_t id = Str2Key(type);
	for (uint32_t n = 0; n < _itemNum; n++) {
		if (_items[n]._id == id) {
			return _items[n]._funcCreate(bp);
		}
	}
	return 0;
}

GaapiIdManager_c::GaapiIdManager_c(Item_c* items, uint32_t itemNum) {
	_items= items;
	_itemNum= itemNum;
}
GaapiIdManager_c::~GaapiIdManager_c() {

}

uint32_t GaapiIdManager_c::Add(class Gaapi_c* api) {
	GaapiAutoMutex_c autoMtx(&_mtx);
	for (uint32_t n = 0; n < _itemNum; n++) {
		if (!_items[n]._id) {
			uint32_t id = n + 1;
			_items[n]._id = id;
			_items[n]._api = api;
			return id;
		}
	}
	return 0;
}

uint32_t GaapiIdManager_c::Api2Id(class Gaapi_c* api) {
	GaapiAutoMutex_c autoMtx(&_mtx);
	for (uint32_t n = 0; n < _itemNum; n++) {
		if (_items[n]._api == api) {
			return _items[n]._id;
		}
	}
	return 0;
}
Gaapi_c* GaapiIdManager_c::Id2Api(uint32_t id) {
	GaapiAutoMutex_c autoMtx(&_mtx);
	if (!id) return 0;
	if (id > _itemNum) return 0;
	return _items[id - 1]._api;
}
Gaapi_c* GaapiIdManager_c::Remove(Gaapi_c* api) {
	GaapiAutoMutex_c autoMtx(&_mtx);
	if (!api) return 0;
	for (uint32_t n = 0; n < _itemNum; n++) {
		if (_items[n]._api == api) {
			_items[n]._api = 0;
			_items[n]._id = 0;
			return api;
		}
	}
	return 0;
}
uint32_t GaapiIdManager_c::Remove(uint32_t id) {
	GaapiAutoMutex_c autoMtx(&_mtx);
	if (!id) return 0;
	if (id > _itemNum) return 0;
	uint32_t index = id - 1;
	if (_items[index]._api) {
		_items[index]._api = 0;
		_items[index]._id = 0;
		return id;
	}
	return 0;
}

GaapiStatic_c::GaapiStatic_c()
{
	_magic0 = Str2Key("gaapi000");
	_magic1 = Str2Key("gaapi001");
	_magic2 = Str2Key("gaapi002");
	_magic3 = Str2Key("gaapi003");
}

GaapiStatic_c::~GaapiStatic_c()
{
	_magic0 = 0;
	_magic1 = 0;
	_magic2 = 0;
	_magic3 = 0;
}


namespace gaapi_ns {
static bool GaapiCheck() {
	GaapiStatic_c* GaapiStatic = (GaapiStatic_c*)buff;
	if (GaapiStatic->_magic0 != GaapiStatic->Str2Key("gaapi000")
		|| GaapiStatic->_magic1 != GaapiStatic->Str2Key("gaapi001")
		|| GaapiStatic->_magic2 != GaapiStatic->Str2Key("gaapi002")
		|| GaapiStatic->_magic3 != GaapiStatic->Str2Key("gaapi003")) {
		return false;
	}
	return true;
}
void GaapiInit() {
	static bool isInit = false;
	if (isInit) return;
	isInit = true;
	new(buff) GaapiStatic_c();
}
void GaapiDeinit() {
	if (GaapiCheck()) { 
		((GaapiStatic_c*)buff)->~GaapiStatic_c();
		return;
	}
}

GaapiStatic_c* GaapiStatic() {
	return (GaapiStatic_c*)buff;
}

GaapiIdManager_c* GaapiIdManager() {
    return &((GaapiStatic_c*)buff)->_infos;
}

GaapiGafRegister_c* GaapiGafRegister() {
	return &((GaapiStatic_c*)buff)->_gafRoot;
}


}






