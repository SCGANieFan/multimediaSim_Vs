#include "SmfPorting.h"
#include "SmfPortingFs.h"
#include "SmfHash.h"
#include "SmfFCC.h"
#include "smf_debug.h"
#include "string.h"
using namespace smf;

EXTERNC void smf_sleep_for(uint32_t ms) {
	smf::sleep_for(ms);
}
EXTERNC uint64_t smf_get_us64() {
	return get64_us();
}
EXTERNC uint32_t smf_hash(const char* str) {
	return Hash(str);
}
EXTERNC uint32_t smf_hashx(const char* str, const char** next) {
	auto rst = HashX(str, 0);
	if (next)*next = str;
	return rst;
}
EXTERNC uint32_t smf_fcc32(const char* str) {
	return fcc32(str);
}
EXTERNC uint64_t smf_fcc64(const char* str) {
	return fcc64(str);
}

EXTERNC void* smf_addr_to_device(const void* addr) {
	return addr_remap_to_dev((void*)addr);
}
EXTERNC void* smf_addr_to_cpu(const void* addr) {
	return addr_remap_to_cpu((void*)addr);
}
EXTERNC void* smf_addr_to_cache(const void* addr) {
	return address_to_cache((void*)addr);
}
EXTERNC void* smf_addr_to_uncache(const void* addr) {
	return address_to_uncache((void*)addr);
}
EXTERNC void smf_cache_writeback(void* addr, uint32_t size) {
	cache_writeback(addr, size);
}
EXTERNC void smf_cache_flush(void* addr, uint32_t size) {
	cache_flush(addr, size);
}
EXTERNC void smf_cache_invalid(void* addr, uint32_t size) {
	cache_invalid(addr, size);
}
EXTERNC void smf_folder_scan(const char* lpPath, void(*cb)(const char* fname, void* priv), bool recursion, char skipFolderPrefix, void* priv) {
	folder_scan(lpPath, cb, recursion, skipFolderPrefix, priv);
}
EXTERNC void smf_print_file_hash(const char* path) {
	uint32_t index = 0;
	folder_scan(path, [](const char* fname, void* priv) {
		auto ext = strrchr(fname, '.');
		if (ext && (Hash(ext) == Hash(".cpp"))) {
			auto name = dbgPath(fname);
			dbgTestL("%8u,%08x,%04x,%s", (*(uint32_t*)priv)++, Hash(name), Hash16(name), name);
		}
		}, true, '_', &index);
}