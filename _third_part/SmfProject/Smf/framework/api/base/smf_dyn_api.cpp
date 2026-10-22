#include "Object.h"
#include "SmfHash.h"

#define DLL_SVC_NAME "dlv2/"
#define DLL_SET(key, val) smf::Object::Root().Set(DLL_SVC_NAME key, val);

EXTERNC bool smf_load_dll(const char* name, bool entry) {
	uint32_t paras[] = {(uint32_t)name, (uint32_t)entry};
	return DLL_SET("load", paras);
}

EXTERNC bool smf_unload_dll(const char* name, bool exit) {
	uint32_t paras[] = {(uint32_t)name, (uint32_t)exit};
	return DLL_SET("unload", paras);
}

EXTERNC bool smf_entry_dll(const char* name, void* para) {
	uint32_t paras[] = {(uint32_t)name, (uint32_t)para};
	return DLL_SET("entry", paras);
}

EXTERNC bool smf_exit_dll(const char* name, void* para) {
	uint32_t paras[] = {(uint32_t)name, (uint32_t)para};
	return DLL_SET("exit", paras);
}

EXTERNC bool smf_load_dll_id(uint32_t dynID, bool entry) {
	uint32_t paras[] = {(uint32_t)dynID, (uint32_t)entry};
	return DLL_SET("loadID", paras);
}

EXTERNC bool smf_unload_dll_id(uint32_t dynID, bool exit) {
	uint32_t paras[] = {(uint32_t)dynID, (uint32_t)exit};
	return DLL_SET("unloadID", paras);
}

EXTERNC bool smf_entry_dll_id(uint32_t dynID, void* para) {
	uint32_t paras[] = {(uint32_t)dynID, (uint32_t)para};
	return DLL_SET("entryID", paras);
}

EXTERNC bool smf_exit_dll_id(uint32_t dynID, void* para) {
	uint32_t paras[] = {(uint32_t)dynID, (uint32_t)para};
	return DLL_SET("exitID", paras);
}