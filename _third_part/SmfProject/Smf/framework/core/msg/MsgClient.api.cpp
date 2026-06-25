#include "MsgClient.h"
#include "SmfDef.gbl.h"

using namespace smf;
using namespace smf::msg;

bool smf::RemoteSet(uint8_t cpuid, const char* path, const void* params, int size) {
	auto obj = Global<MsgClient>(eGlobal::msgcore);
	returnIfErrCS(false, !obj, "%u,%s", cpuid, path);
	return obj->MsgSet(cpuid, path, params, size);
}
bool smf::RemoteSet(uint8_t cpuid, const char* path, int vals) {
	auto obj = Global<MsgClient>(eGlobal::msgcore);
	returnIfErrCS(false, !obj, "%u,%s", cpuid, path);
	return obj->MsgSet(cpuid, path, vals);
}
bool smf::RemoteConfig(uint8_t cpuid, const char* script, const void* params, int size) {
	auto obj = Global<MsgClient>(eGlobal::msgcore);
	returnIfErrCS(false, !obj, "%u,%s", cpuid, script);
	return obj->MsgConfig(cpuid, script, params, size);
}
bool smf::MsvcInvoke(smf_cb_priv0 cb, void* priv) {
	auto obj = Global<MsgClient>(eGlobal::msgcore);
	returnIfErrC(false, !obj);
	return obj->MsgInvoke(cb, priv);
}
bool smf::MsvcInvokeFree(void* buff) {
	auto obj = Global<MsgClient>(eGlobal::msgcore);
	returnIfErrC(false, !obj);
	return obj->MsgInvokeFree(buff);
}
bool smf::MsvcInvokeDelete(Object* object) {
	auto obj = Global<MsgClient>(eGlobal::msgcore);
	returnIfErrC(false, !obj);
	return obj->MsgInvokeDelete(object);
}