#pragma once
#include <memory>
#include <utility>
#include "IPool.h"
#include "ObjectStatic.h"
namespace smf {
	class string{
	public:
		string() {}
		string(int size, int align = 0) { reset(size, align); }
		string(const char* s) { reset(s); }
		string(char* s) { reset(s); }
		string(string& s) { reset(s._str); }
		string(string&& s) { std::swap(_str, s._str); }
		~string() { reset(0); }
	protected:
		char* _str = 0;
	public:
		char* c_str()const { return _str; }
		int length()const;
		char* release() { auto str = _str; _str = 0; return str; }
		bool reset(const char* str = 0);
		bool reset(int size, int align = 0);
		operator bool()const { return (bool)_str; }
		operator char* ()const { return _str; }
		operator const char* ()const { return _str; }
		string& operator=(string&& s) { std::swap(_str, s._str); return *this; }
		string& operator=(const string& s) { reset(s._str); return *this; }
		string& operator=(const char* s) { reset(s); return *this; }
		string& operator=(char* s) { reset(s); return *this; }
		char operator[](int idx) const { return _str[idx]; }
		char& operator[](int idx) { return _str[idx]; }
	};
}
