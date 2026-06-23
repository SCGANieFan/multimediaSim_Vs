#include "SmfDef.h"
#include "SmfDef.gbl.h"
#include "SmfRoot.h"
#include "SmfPorting.h"
#include "IPool.h"
#include "mutex.h"
#include <assert.h>

using namespace smf;
EXTERNC bool smf_initialize(bool);
EXTERNC const char* smf_version();
SmfRoot::SmfRoot(){
	_objs.Tags(this);
	smf_initialize(false);
	timer_sync();
	dbgInfoX("%s\n", smf_version());
}
SmfRoot::~SmfRoot(){
	_objs.Clear();
	_begin = _end = 0;
}
template<int C>
bool SmfRoot::TRoot<C>::get(uint32_t key, void* val)const {
	switch (key) {
	//case Hash("base"):*(void**)val = _tags; return true;
	//case Hash("log"):*(void**)val = &_log; return true;
	//case Hash("pools"):*(void**)val = &_pools; return true;
	//case Hash("Reflect"):*(void**)val = &_reflect; return true;
	//case Hash("threads"):*(void**)val = &_threads; return true;
	//case Hash("shm"):*(void**)val = _shm; return true;
	case Hash("stm"):*(void**)val = Global<void>(eGlobal::ics); return true;
	case Hash("vfmt"):*(void**)val = root()._vfmt; return true;
	case Hash("arescb"):*(void**)val = root()._ares; return true;
	case Hash("mtx"):*(void**)val = &root()._mtx; return true;
	case Hash("cpuid"):*(uint8_t*)val = root()._cpuid; return true;
	case Hash("script_version"):*(const char**)val = root()._script_version; return true;
	}
	return Base::get(key, val);
}
template<int C>
bool SmfRoot::TRoot<C>::set(uint32_t key, void* val) {
	switch (key) {
	case Hash("stm"):Global<void>(eGlobal::ics,(void*)val); return true;
	//case Hash("vfmt"):root()._cpuid = (uint32_t)val; return true;
	//case Hash("arescb"):root()._arescb = (uint32_t)val; return true;
	//case Hash("mtx"):root()._mtx = (uint32_t)val; return true;
	case Hash("cpu"):
	case Hash("cpuid"):root()._cpuid = (uint32_t)val; return true;
	case Hash("version"):root()._version = (const char*)val; return true;
	case Hash("script"):root()._script = (const char*)val; return true;
	case Hash("script_version"):root()._script_version = (const char*)val; return true;
	case Hash("assert"):if (val)assert(0); return true;
	case Hash("exception"):if (val) { *(uint32_t*)val = (uint32_t)val; }; return true;
	//case Hash("invoke"): Invoke(((CbInvoke*)val)[0], ((void**)val)[1]); return true;
	//case Hash("invokeDelay"):Invoke(((CbInvoke*)val)[0], ((void**)val)[1], ((uint32_t*)val)[2]); return true;
	case Hash("measureLock"): this->_flagsExt.Set(eGlobalFlags::measureLock, val); return true;
	case Hash("powerCtrl"):	returnIfErrC(false, (uint32_t)val > 7);
		if (val)	this->_flagsExt.Set((1u << (uint32_t)val), true);
		else this->_flagsExt.Set(0xfe, false);
		return true;
	}
	return root()._log.Set(key, val)
		|| root()._pools.Set(key, val)
		|| Base::set(key, val);
}
template<int C>
char* SmfRoot::TRoot<C>::print(char* ptr, char* end) const {
	unique_lock<mutex> lck(Mutex());
	return Base::print(ptr,end);
}
template<int C>
Object::ParamTable* SmfRoot::TRoot<C>::GetParamTable() const {
	auto obj = (Object*)this->Child("params");
	if (obj) {
		Object::ParamTable* table = 0;
		if (obj->Get("table", &table)) {
			return table;
		}
	}
	return 0;
}
