#include "string.hh"
#include "smf_debug.h"

using namespace smf;
///
bool string::reset(const char* str) {
	auto& pool = BasePool();
	if (_str) {
		pool.Free(_str);
	}
	if (str) {
		_str = pool.CloneString(str);
		returnIfErrC(false,!_str);
	}
	return true;
}

bool string::reset(int size, int align) {
	auto& pool = BasePool();
	if (_str) {
		pool.Free(_str);
	}
	if (size) {
		_str = (char*)pool.Alloc(size, align);
		returnIfErrC(false, !_str);
	}
	return true;
}

int string::length()const {
	return _str ? (strlen(_str) + 1) : 0;
}