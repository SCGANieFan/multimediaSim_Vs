#include "SmfDef.h"
#include "SmfDef.gbl.h"
#include "Object.h"
#include "IPool.h"
#include "IntrLock.h"
#include "string.h"
#include "thread.h"
#include "SmfLayout.h"
#include "SmfPorting.h"
#include "BackGround.h"
using namespace smf;

int smf::snprintf(char*ptr, int n, const char*fmt, ...) {
	va_list ap;
	va_start(ap,fmt);
	auto x = vsnprintf(ptr, n, fmt, ap);
	va_end(ap);
	return x <= n ? x : n;
}
char* smf::snprintf(char*ptr, const char*end, const char*fmt, ...) {
	auto n = end - ptr;
	va_list ap;
	va_start(ap, fmt);
	auto x = vsnprintf(ptr, n, fmt, ap);
	va_end(ap);
	return ptr + (x <= n ? x : n);
}

uint64_t smf::UniqueID(const char* pre) {
	return Combine64(pre, UniqueID());
}

uint64_t smf::UniqueID(char pre) {
	return Combine64(pre, UniqueID());
}

//uint32_t smf::UniqueID() {
//	static uint32_t _idx = 1;
//	IntrLock lock;
//	return _idx++;
//}

uint64_t smf::Combine64(const char* pre, uint32_t idx) {
	uint64_t id = 0;
	snprintf((char*)&id, 8, "%s%d", pre, idx);
	return id;
}
uint64_t smf::Combine64(char pre, uint32_t idx) {
	uint64_t id = 0;
	snprintf((char*)&id, 8, "%c%d", pre, idx);
	return id;
}

//Object& smf::Root() {
//	return Object::Root();
//}
static BackGround* __bgstart() {
	auto bg = Global<BackGround>(eGlobal::background);
	if (!bg) {
		bg = (BackGround*)Object::Root().Child("bg");
		Global<BackGround>(eGlobal::background, bg);
	}
	if (!bg) {
		bg = new BackGround();
		returnIfErrC(0, !bg);
		Global<BackGround>(eGlobal::background, bg);
	}
	return bg;
}

//BackGround& smf::BackGroundTask() {
//	auto bg = __bgstart();
//	return *bg;
//}
//bool smf::Invoke(const std::function<bool()>& func, uint32_t delay) {
//	auto bg = __bgstart();
//	return bg ? bg->Invoke(func, delay) : false;
//}
bool smf::Invoke(bool(*func)(void*), void* priv, uint32_t delay, uint32_t interval) {
	auto bg = __bgstart();
	return bg ? bg->Invoke((BackGround::Func)func, priv, delay, interval) : false;
}
//bool smf::Invoke(bool(*func)(void*&), void* priv, uint32_t delay) {
//	auto bg = __bgstart();
//	return bg ? bg->Invoke(func, priv, delay) : false;
//}
bool smf::InvokeDelete(Object* obj, uint32_t delay) {
	auto bg = __bgstart();
	return bg?bg->InvokeDelete(obj,delay):false;
}
bool smf::InvokeFree(void* buff, uint32_t delay) {
	auto bg = __bgstart();
	return bg?bg->InvokeFree(buff, delay):false;
}

void* smf::Alloc(unsigned size, unsigned align) {
	return Pools().Globle().Alloc(size, align);
}
void* smf::Calloc(unsigned count, unsigned size) {
	return Pools().Globle().Calloc(count, size);
}
void* smf::Realloc(void* ptr, unsigned size) {
	return Pools().Globle().Realloc(ptr, size);
}
//void* smf::Realloc(void* ptr, unsigned size, unsigned oldsize) {
//	return Pools().Globle().Realloc(ptr, size, oldsize);
//}
bool smf::Hold(void* ptr) {
	return Pools().Globle().Hold(ptr);
}
void smf::Free(void*& ptr) {
	if (!Pools().Globle().Free(ptr)) {
		//dbgWarnPXL("%p", __builtin_return_address(0));
	}
}

Object* smf::Deserialize(const char* serial, void* params, Object* parent) {
	return Layout::Create(serial, parent, params);
}
Object* smf::Deserialize(const char* serial, void* params, const char* parent) {
	return Layout::Create(serial, Root().Child(parent), params);
}

Object* smf::Deserialize(mutex& mtx, const char* serial, void* params, Object* parent) {
	return Layout::Create(mtx, serial, parent, params);
}
Object* smf::Deserialize(mutex& mtx, const char* serial, void* params, const char* parent) {
	return Layout::Create(mtx, serial, Root().Child(parent), params);
}

bool smf::Deserialize(uint8_t cpuid, const char* serial, void* params, uint32_t parasize) {
	if (cpuid == 0xff) {
		return Deserialize(serial, params);
	}
	else {
		uint32_t param[4]{ (uint32_t)serial, (uint32_t)params, parasize, cpuid};
		return Root().Set("msvc/sendCfg4", param);
	}
}

void smf::ErrorProcess(smf_error_t& err, const char* file, unsigned line, unsigned error) {
	//dbgNotePXL(Name());
	if (!err.err) {
		err.file = Hash16(file);
		err.line = (uint16_t)line;
		//_error.inst = (uint16_t)(uint32_t)(void*)this;
		err.sub = (uint16_t)(error >> 16);
		err.err = (uint16_t)(error & 0xffff);
	}
	else if (err.err == 1 && error != 1) {
		err.err = (uint16_t)(error & 0xffff);
		dbgTestPXL("error:%u,err.err:%u,", error, err.err);
	}
}
