#include "Object.h"
#include "smf_api.h"
#include "smf_error.h"
#include "smf_debug.h"
#include "SmfPorting.h"
#include "Reflection.h"
#include "SmfFCC.h"
#include "SmfHash.h"
using namespace smf;

EXTERNC int smf_is_supported(const char* type) {
	return (bool)Object::IsSupported(type);
}
EXTERNC int smf_supports(const char* type,const char* keys[], int max) {
	auto items = (Reflection::item_t **)Alloc(sizeof(Reflection::item_t) * max);
	returnIfErrC(0, !items);
	auto n = Object::Reflect().Finds(type, items, max);
	for (int i = 0; i < n; i++) {
		keys[i] = items[i]->name;
	}
	Free(items);
	return n;
}
EXTERNC int smf_is_supported_(const char* type,const char* keywords) {
	auto size = strlen(type) + strlen(keywords) + 2;
	char* akeys = (char*)Alloc(size);
	returnIfErrC(0, !akeys);
	memset(akeys, 0, size);
	sprintf(akeys, "%s-%s",type, keywords);
	auto ret = smf_is_supported(akeys);
	Free(akeys);
	return ret;
}
EXTERNC int smf_supported_decoder(const char* keys[], int max) {
	return smf_supports("dec", keys, max);
}
EXTERNC int smf_supported_encoder(const char* keys[], int max) {
	return smf_supports("enc", keys, max);
}
EXTERNC int smf_supported_processer(const char* keys[], int max){
	return smf_supports("proc", keys, max);
}
EXTERNC int smf_supported_demuxer(const char*keys[],int max) {
	return smf_supports("dem", keys,max);
}
///
EXTERNC int smf_is_supported_decoder(const char* keywords) {
	return smf_is_supported_("dec",keywords);
}
EXTERNC int smf_is_supported_encoder(const char* keywords) {
	return smf_is_supported_("enc",keywords);
}
EXTERNC int smf_is_supported_processer(const char* keywords){
	return smf_is_supported_("proc",keywords);
}
EXTERNC int smf_is_supported_demuxer(const char*keywords) {
	return smf_is_supported_("dem",keywords);
}

EXTERNC void* smf_create(const char* type) {
	return Object::Create<Object>(type);
}
EXTERNC void smf_destroy(void* hd) {
	auto obj = (Object*)(hd);
	if (obj) {
		delete obj;
	}
}
EXTERNC void smf_destroy_with_name(const char* name) {
	smf_destroy(smf_find(name));
}
EXTERNC void* smf_find(const char* name) {
	return Object::Root().Child(name);
}
#if 0
///
EXTERNC const char* smf_get_keywords(void* hd) {	
	Object* obj = To<Object>(hd);
	returnIfErrC(NULL, !obj);
	return obj->Class()->Keys();
}
EXTERNC const char* smf_get_type(void* hd) {
	Object* obj = To<Object>(hd);
	returnIfErrC(NULL, !obj);
	return obj->Class()->Type();
}
#endif
///
EXTERNC bool smf_run(void* hd, void* para) {
	auto obj = (Object*)(hd);
	returnIfErrC(false, !obj);
	return obj->Run(para);
}
///
EXTERNC bool smf_set(void* hd, uint32_t key, void* val) {
	auto obj = (Object*)(hd);
	returnIfErrC(false, !obj);
	return obj->Set(key, val);
}
EXTERNC bool smf_get(void* hd, uint32_t key, void* val) {
	auto obj = (Object*)(hd);
	returnIfErrC(false, !obj);
	return obj->Get(key, val);
}
///
EXTERNC bool smf_setp(void* hd, const char* key, void* val) {
	auto obj = (Object*)(hd);
	returnIfErrC(false, !obj);
	return obj->Set(key, val);
}
EXTERNC bool smf_getp(void* hd, const char* key, void* val) {
	auto obj = (Object*)(hd);
	returnIfErrC(false, !obj);
	return obj->Get(key, val);
}
//
EXTERNC bool smf_sets(const char* key, void* val) {
	return Object::Root().Set(key, val);
}
EXTERNC bool smf_gets(const char* key, void* val) {
	return Object::Root().Get(key, val);
}
//
EXTERNC smf_error_t* smf_get_error(void* hd) {
	auto obj = (Object*)(hd);
	returnIfErrC(NULL, !obj);
	return (smf_error_t*)&obj->Error();
}
EXTERNC unsigned smf_get_err32(void* hd) {
	auto obj = (Object*)(hd);
	returnIfErrC(SMF_ERROR_INVALID_HANDLE, !obj);
	return obj->Error().err32[1];
}
EXTERNC void smf_print_error(void*hd) {
	smf_error_t* err = smf_get_error(hd);
	smf_error_print(err);
}
#if 0
//
EXTERNC void* smf_alloc(void* hd, int size) {
	auto obj = (Object*)(hd);
	returnIfErrC(NULL, !obj);
	return obj->Alloc(size);
}
EXTERNC void smf_free(void* hd, void* buff) {
	auto obj = (Object*)(hd);
	returnIfErrC0(!obj);
	return obj->Free(buff);
}
EXTERNC void* smf_alloc_open_param(void* hd) {
	auto obj = (Object*)(hd);
	returnIfErrC(NULL, !obj);
	int size = 0;;
	returnIfErrC(NULL, !obj->Get(SMF_PARAM_OpenParamSize,&size));
	returnIfErrC(NULL, !size);
	return obj->Pool()->Calloc(1, size);
}
#endif
#if 0
//
EXTERNC void* smf_get_pool(void* hd) {
	auto obj = (Object*)(hd);
	returnIfErrC(NULL, !obj);
	return obj->Pool();
}

EXTERNC bool smf_set_pool(void* hd, void* pool) {
	auto obj = (Object*)(hd);
	returnIfErrC(false, !obj);
	obj->PoolSet((IPool*)pool);
	return true;
}
#endif

EXTERNC void smf_reregister(const char* dst_type, const char* src_type) {
	Object::ReRegister(dst_type ,src_type);
}

EXTERNC void smf_unregister(const char* name) {
	Object::UnRegister(name);
}

EXTERNC bool smf_cmd_register(const char* name, smf_cb_entry cb) {
	return Register(name, cb);
}

EXTERNC int smf_cmd_entry(int argc, char** argv) {
	return Entry(argc, argv);
}

EXTERNC bool smf_invoke(bool(*func)(void*), void* priv, uint32_t delay) {
	return Invoke(func, priv, delay);
}

