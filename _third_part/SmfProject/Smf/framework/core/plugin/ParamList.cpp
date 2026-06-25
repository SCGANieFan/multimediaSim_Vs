#include "ParamList.h"
#include "SmfHash.h"
#include "SmfFCC.h"
#include "SmfLayout.h"
using namespace smf;
EXTERNC void smf_param_list_register() {
	Object::Register<ParamListC<128>>("list-para-128");
	Object::Register<ParamListC<64>>("list-para-64");
}
ParamList::ParamList(item_t* items, uint32_t count)
	: _table(items,count)
{
	_id = fcc64("params");
}
bool ParamList::set(uint32_t key, void* val) {
	switch (key) {
	//case Hash("hold"): setHold(!!val); return true;
	case Hash("cfg"): return Deserialize(_table,(const char*)val);
	default: return Object::set(key, val);
	}
}
bool ParamList::get(uint32_t key, void* val) const{
	switch (key) {
	case Hash("table"):*(StringTable**)val = (StringTable*)&_table; return true;
	//case Hash("pair"):return SimpleTable<void*>::Get(Hash(((const char**)val)[0]),((void**)val)[1]);
	}
	if (_table.Get(key, (const char**)val)) {
		return true;
	}
	return Object::get(key, val);
}
