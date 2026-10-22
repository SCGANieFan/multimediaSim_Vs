#include "smf_api_remote.h"
#include "SmfPorting.h"
#include "SmfDef.h"
#include "SmfFCC.h"
#include "SmfHash.h"
#include "smf_debug.h"
using namespace smf;

EXTERNC bool smf_remote_set(uint8_t cpuid, const char* path, const void* params, int size) {
	return RemoteSet(cpuid, path, params, size);
}
EXTERNC bool smf_remote_set32(uint8_t cpuid, const char* path, int vals) {
	return RemoteSet(cpuid, path, vals);
}
EXTERNC bool smf_remote_config(uint8_t cpuid, const char* script, const void* params, int size) {
	return RemoteConfig(cpuid, script, params, size);
}

EXTERNC bool smf_remote_assert(uint8_t cpuid) {
	return RemoteSet(cpuid, "assert", 1);
}
EXTERNC bool smf_remote_exception(uint8_t cpuid) {
	return RemoteSet(cpuid, "exception", 1);
}
EXTERNC bool smf_remote_log_mask(uint8_t cpuid, uint32_t chmask) {
	return RemoteSet(cpuid, "logMask", (int)chmask);
}
EXTERNC bool smf_remote_log_mask_enable(uint8_t cpuid, uint32_t chmask) {
	return RemoteSet(cpuid, "logMaskEnable", (int)chmask);
}
EXTERNC bool smf_remote_log_mask_disable(uint8_t cpuid, uint32_t chmask) {
	return RemoteSet(cpuid, "logMaskDisable", (int)chmask);
}
EXTERNC bool smf_remote_log_enable(uint8_t cpuid, uint32_t channel) {
	return RemoteSet(cpuid, "logEnable", (int)channel);
}
EXTERNC bool smf_remote_log_disable(uint8_t cpuid, uint32_t channel) {
	return RemoteSet(cpuid, "logDisable", (int)channel);
}
EXTERNC bool smf_remote_notify_log(uint8_t cpuid, bool enable){
	return RemoteSet(cpuid, "root/bg/notify/notifyEnable", (int)enable);
}
EXTERNC bool smf_remote_pool_add(uint8_t cpuid, const char* name, void* addr, uint32_t size, uint32_t thr) {
	auto addr64 = (void*)Align64((uint32_t)addr);
	auto size64 = size - ((char*)addr64 - (char*)addr);
	size64 = Align64Lower(size64);
	dbgApiPXL("[%u]%s,%p/%u=>%p/%u,%u", cpuid, name, addr, size, addr64, size64, thr);
	cache_invalid(addr64, size64);
	addr64 = addr_remap_to_dev(addr64);
	//
	uint32_t buff[5];
	((uint64_t*)buff)[0] = fcc64(name);
	buff[2] = (uint32_t)addr64;
	buff[3] = size64;
	buff[4] = thr;
	return RemoteSet(cpuid, "remotePoolAdd", buff, sizeof(buff));
}
EXTERNC bool smf_remote_pool_add_buffer(uint8_t cpuid, const char* name, void* addr, uint32_t size) {
	auto addr64 = (void*)Align64((uint32_t)addr);
	auto size64 = size - ((char*)addr64 - (char*)addr);
	size64 = Align64Lower(size64);
	dbgApiPXL("[%u]%s,%p/%u=>%p/%u", cpuid, name, addr, size, addr64, size64);
	cache_invalid(addr64, size64);
	addr64 = addr_remap_to_dev(addr64);
	//
	uint32_t buff[5];
	((uint64_t*)buff)[0] = fcc64(name);
	buff[2] = (uint32_t)addr64;
	buff[3] = size64;
	return RemoteSet(cpuid, "remotePoolAddx", buff, sizeof(buff));
}
EXTERNC bool smf_remote_pool_add_buffers(uint8_t cpuid, const char* name, smf_pair_t* pairs, uint32_t cnt) {
	dbgApiPXL("[%u]%s,%u", cpuid, name, cnt);
	returnIfErrC(false, cnt > 14);
	uint64_t buff[16];
	auto ptr64 = buff;
	*ptr64++ = fcc64(name);
	*ptr64++ = cnt;
	for (int i = 0; i < cnt; i++) {
		auto pair = pairs + i;
		returnIfErrC(false, !pair->data);
		returnIfErrC(false, !pair->size);
		auto data = (char*)pair->data;
		auto end = data + pair->size;
		data = (char*)Align64((uint32_t)data);
		end = (char*)Align64Lower((uint32_t)end);
		auto size = end - data;
		dbgApiPXL("%p/%u<=%p/%u", data, size, pair->data, pair->size);
		cache_invalid(data, size);
		pair->data = addr_remap_to_dev(data);
		pair->size = size;
		*ptr64++ = *((uint64_t*)pair);
	}
	auto size = (char*)ptr64 - (char*)buff;
	//
	return RemoteSet(cpuid, "remotePoolAddBuffers", buff, size);
}
EXTERNC bool smf_remote_pool_add_alias(uint8_t cpuid, const char* name, const char* src) {
	dbgApiPXL("[%u]%s<-%s", cpuid, name, src);
	uint64_t buff[2]{ fcc64(name),fcc64(src) };
	return RemoteSet(cpuid, "remotePoolAddAlias", buff, sizeof(buff));
}
EXTERNC bool smf_remote_pool_remove(uint8_t cpuid, const char* name) {
	dbgApiPXL("[%u]%s", cpuid, name);
	uint64_t id = fcc64(name);
	return RemoteSet(cpuid, "remotePoolremove", &id, sizeof(id));
}
