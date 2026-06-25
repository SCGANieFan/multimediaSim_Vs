#include "Remote.h"
#include "SmfHash.h"
#include "SmfDef.h"
#include "smf_debug.h"
#include "SmfLayout.h"

using namespace smf;
Remote::~Remote() {
	destroy();
}
bool Remote::create() {
	if (_cpuid && _create) {
		return RemoteConfig(_cpuid, _create, 0, 0);
	}
	return false;
}
bool Remote::destroy() {
	if (_cpuid && _destroy) {
		auto cpuid = _cpuid; _cpuid = 0;
		return RemoteConfig(cpuid, _destroy, 0, 0);
	}
	return false;
}
bool Remote::set(uint32_t keys, void* vals) {
	switch (keys) {
	case Hash("cpuid"): _cpuid = (uint32_t)vals; return true;
	case Hash("create"): _create = (const char*)vals; return true;
	case Hash("destroy"): _destroy = (const char*)vals; return true;
	default:return false;
	}
}
bool Remote::set(const char* script) {
	Layout layout(script, false, true, false);
	return layout.Parse([](void* priv, const char* keys, void* vals, char rst) {
		auto remote = (Remote*)priv;
		remote->set(Hash(keys), vals);
		}, this);
}
