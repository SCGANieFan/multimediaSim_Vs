#pragma once
#include"MTF.Type.h"
#include"MTF.Printer.h"
#include"MTF.Memory.h"
using namespace mtf_ns;
class MTF_Object
{
public:
	void *operator new(size_t size) {
		return Malloc(size);
	}
	void* operator new(size_t size, void *ptr) {
		//return Malloc(size);
		return ptr;
	}
	void operator delete(void *ptr) {
		Free(ptr);
	}
public:
	MTF_Object();
	virtual ~MTF_Object();
public:
	virtual mtf_i32 Set(const char*key, mtf_void *val);
	virtual mtf_i32 Get(const char*key, mtf_void *val);

private:

};

