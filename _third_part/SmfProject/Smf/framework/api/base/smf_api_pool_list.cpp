#include "smf_api_pool.h"
#include "SmfDef.h"
#include "SmfString.h"
#include "SmfPorting.h"
#include "IPool.h"
#include "smf_debug.h"
using namespace smf;
//
EXTERNC void smf_pool_print() {
	Pools().Print();
}

EXTERNC void smf_std_pool_register() {
	IPool::Pools().Register("gbl", (smf_cb_malloc)&malloc, (smf_cb_free)&free, 0, (smf_cb_realloc)&realloc);
}
EXTERNC void* smf_register_globle_pool(void* buff, int size) {
	return Pools().RegisterGloble(buff, size, 2048);
}

EXTERNC void* smf_register_pool(const char* name, void* buff, unsigned size, unsigned thr) {
	return Pools().Register(name, buff, size, thr);
}
EXTERNC void* smf_register_pool_alias(const char* alias, const char* name) {
	return Pools().RegisterAlias(alias, name);
}
EXTERNC void* smf_register_pool_text(const char* name, void* buff, unsigned size) {
	return Pools().RegisterX(name, buff, size);
}


EXTERNC void* smf_register_pool_callback(const char* name
	, void* (*cbAlloc)(uint32_t size), void(*cbFree)(void* ptr)
	, uint32_t(*cbGetSize)(), uint32_t(*cbGetTotal)()
) {
	return Pools().Register(name, (smf_cb_malloc)cbAlloc, (smf_cb_free)cbFree, 0,0, (smf_cb_getsize)cbGetSize, (smf_cb_gettotal)cbGetTotal);
}

EXTERNC void smf_designate_globle_pool(const char* name) {
	Pools().Globle(name);
}

EXTERNC void smf_register_backup_pool(const char* name, const char* bkpool) {
	auto pool = Pools().Find(name);
	if (pool)
		pool->SetBackup(bkpool);
}

